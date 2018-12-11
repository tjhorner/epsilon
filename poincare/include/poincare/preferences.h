#ifndef POINCARE_PREFERENCES_H
#define POINCARE_PREFERENCES_H

namespace Poincare {

class Preferences final {
public:
  enum class EditionMode {
    Edition2D,
    Edition1D
  };
  /* The 'PrintFlotMode' refers to the way to display float 'scientific' or
   * 'auto'. The scientific mode returns float with style -1.2E2 whereas the
   * auto mode tries to return 'natural' float like (0.021) and switches to
   * scientific mode if the float is too small or too big regarding the number
   * of significant digits. */
  enum class PrintFloatMode {
    Decimal = 0,
    Scientific = 1,
  };
  enum class ComplexFormat {
    Cartesian = 0,
    Polar = 1
  };
  enum class AngleUnit {
    Degree = 0,
    Radian = 1
  };
  Preferences();
  static Preferences * sharedPreferences();
  AngleUnit angleUnit() const { return m_angleUnit; }
  void setAngleUnit(AngleUnit angleUnit) { m_angleUnit = angleUnit; }
  PrintFloatMode displayMode() const { return m_displayMode; }
  void setDisplayMode(PrintFloatMode mode) { m_displayMode = mode; }
  EditionMode editionMode() const { return m_editionMode; }
  void setEditionMode(EditionMode editionMode) { m_editionMode = editionMode; }
  ComplexFormat complexFormat() const { return m_complexFormat; }
  void setComplexFormat(Preferences::ComplexFormat complexFormat) { m_complexFormat = complexFormat; }
  char numberOfSignificantDigits() const { return m_numberOfSignificantDigits; }
  void setNumberOfSignificantDigits(char numberOfSignificantDigits) { m_numberOfSignificantDigits = numberOfSignificantDigits; }
private:
  AngleUnit m_angleUnit;
  PrintFloatMode m_displayMode;
  EditionMode m_editionMode;
  ComplexFormat m_complexFormat;
  char m_numberOfSignificantDigits;
};

}

#endif
