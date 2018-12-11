#coding=utf-8

import sys
import re
import unicodedata
import argparse
import io

ion_special_characters = {
    u'Δ': "Ion::Charset::CapitalDelta",
    u'Σ': "Ion::Charset::CapitalSigma",
    u'λ': "Ion::Charset::SmallLambda",
    u'μ': "Ion::Charset::SmallMu",
    u'σ': "Ion::Charset::SmallSigma",
    u'≤': "Ion::Charset::LessEqual",
    u'≈': "Ion::Charset::AlmostEqual",
    u'ø': "Ion::Charset::Empty",
    u'•': "Ion::Charset::MiddleDot"
}

def ion_char(i18n_letter):
    if i18n_letter == '\'':
        return "'\\\''"
    if ord(i18n_letter) < 128:
        return "'" + i18n_letter + "'"
    if i18n_letter in ion_special_characters:
        return ion_special_characters[i18n_letter]
    normalized = unicodedata.normalize("NFD", i18n_letter).encode('ascii', 'ignore')
    #sys.stderr.write("Warning: Normalizing unicode character \"" + i18n_letter + "\" -> \"" + normalized + "\"\n")
    return "'" + normalized.decode() + "'"

def source_definition(i18n_string):
    ion_characters = []
    i = 0
    while i < len(i18n_string):
        if i18n_string[i] == '\\':
            i = i+1
            newChar = "'\\"+i18n_string[i]+"'"
            ion_characters.append(newChar)
        else:
            ion_characters.append(ion_char(i18n_string[i]))
        i = i+1
    ion_characters.append("0")
    return "{" + ", ".join(ion_characters) + "}"

def split_line(line):
    match = re.match(r"^(\w+)\s*=\s*\"(.*)\"$", line)
    if not match:
        sys.stderr.write("Error: Invalid line \"" + line + "\"\n")
        sys.exit(-1)
    return (match.group(1), source_definition(match.group(2)))

def locale_from_filename(filename):
    return re.match(r".*\.([a-z]+)\.i18n", filename).group(1)

def parse_files(files):
    data = {}
    messages = set()
    universal_messages = set()
    for path in files:
        locale = locale_from_filename(path)
        if locale not in data:
            data[locale] = {}
        with io.open(path, "r", encoding='utf-8') as file:
            for line in file:
                name,definition = split_line(line)
                if locale == "universal":
                    if name in messages:
                        sys.stderr.write("Error: Redefinition of message \"" + name + "\" as universal\n")
                        sys.exit(-1)
                    if name in universal_messages:
                        sys.stderr.write("Error: Redefinition of universal message \"" + name + "\"\n")
                        sys.exit(-1)
                    universal_messages.add(name)
                else:
                    messages.add(name)
                data[locale][name] = definition
    return {"messages": sorted(messages), "universal_messages": sorted(universal_messages), "data": data}

def print_header(data, path, locales):
    f = open(path, 'w')
    f.write("#ifndef APPS_I18N_H\n")
    f.write("#define APPS_I18N_H\n\n")
    f.write("// This file is auto-generated by i18n.py\n\n")
    f.write("#include <escher.h>\n\n")
    f.write("namespace I18n {\n\n")
    f.write("constexpr static int NumberOfLanguages = %d;\n\n" % len(locales))

    # Messages enumeration
    f.write("enum class Message : uint16_t {\n")
    f.write("  Default = 0,\n")
    for message in data["universal_messages"]:
        f.write("  " + message + ",\n")
    f.write("\n")
    f.write("  LocalizedMessageMarker,\n\n")
    for message in data["messages"]:
        f.write("  " + message + ",\n")
    f.write("};\n\n")

    # Languages enumeration
    f.write("enum class Language : uint16_t {\n")
    f.write("  Default = 0,\n")
    for locale in locales:
        f.write("  " + locale.upper() + ",\n")
    f.write("};\n\n")

    # Language names
    f.write("constexpr const Message LanguageNames[NumberOfLanguages] = {\n");
    for locale in locales:
        f.write("  Message::Language" + locale.upper() + ",\n")
    f.write("};\n\n")
    f.write("}\n\n")
    f.write("#endif\n")
    f.close()

def print_implementation(data, path, locales):
    f = open(path, 'w')
    f.write("#include \"i18n.h\"\n")
    f.write("#include \"global_preferences.h\"\n")
    f.write("#include <assert.h>\n\n");
    f.write("namespace I18n {\n\n")

    # Write the default message
    f.write("constexpr static char universalDefault[] = {0};\n")
    # Write the universal messages
    for message in data["universal_messages"]:
        f.write("constexpr static char universal" + message + "[] = " + data["data"]["universal"][message] + ";\n")
    f.write("\n")
    f.write("constexpr static const char * universalMessages[%d] = {\n" % (len(data["universal_messages"])+1))
    f.write("  universalDefault,\n")
    for message in data["universal_messages"]:
        f.write("  universal" + message + ",\n")
    f.write("};\n\n")

    # Write the localized messages
    for message in data["messages"]:
        for locale in locales:
            if not locale in data["data"]:
                sys.stderr.write("Error: Undefined locale \"" + locale + "\"\n")
                sys.exit(-1)
            if not message in data["data"][locale]:
                sys.stderr.write("Error: Undefined key \"" + message + "\" for locale \"" + locale + "\"\n")
                sys.exit(-1)
            f.write("constexpr static char " + locale + message + "[] = " + data["data"][locale][message] + ";\n")
    f.write("\n")
    f.write("constexpr static const char * messages[%d][%d] = {\n" % (len(data["messages"]), len(locales)))
    for message in data["messages"]:
        f.write("  {")
        for locale in locales:
            f.write(locale + message + ", ")
        f.write("},\n")
    f.write("};\n\n")

    # Write the translate method
    f.write("const char * translate(Message m, Language l) {\n")
    f.write("  assert(m != Message::LocalizedMessageMarker);\n")
    f.write("  int localizedMessageOffset = (int)Message::LocalizedMessageMarker+1;\n")
    f.write("  if ((int)m < localizedMessageOffset) {\n")
    f.write("    assert(universalMessages[(int)m] != nullptr);\n")
    f.write("    return universalMessages[(int)m];\n")
    f.write("  }\n")
    f.write("  int languageIndex = (int)l;\n")
    f.write("  if (l == Language::Default) {\n")
    f.write("    languageIndex = (int) GlobalPreferences::sharedGlobalPreferences()->language();\n")
    f.write("  }\n")
    f.write("  assert(languageIndex > 0);\n")
    f.write("  int messageIndex = (int)m - localizedMessageOffset;\n")
    f.write("  assert((messageIndex*NumberOfLanguages+languageIndex-1)*sizeof(char *) < sizeof(messages));\n")
    f.write("  return messages[messageIndex][languageIndex-1];\n")
    f.write("}\n\n")
    f.write("}\n")
    f.close()

parser = argparse.ArgumentParser(description="Process some i18n files.")
parser.add_argument('--header', help='the .h file to generate')
parser.add_argument('--implementation', help='the .cpp file to generate')
parser.add_argument('--locales', nargs='+', help='locale to actually generate')
parser.add_argument('--files', nargs='+', help='an i18n file')

args = parser.parse_args()
data = parse_files(args.files)
if args.header:
    print_header(data, args.header, args.locales)
if args.implementation:
    print_implementation(data, args.implementation, args.locales)
