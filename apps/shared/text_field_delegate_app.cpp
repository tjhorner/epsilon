#include "text_field_delegate_app.h"
#include <apps/apps_container.h>
#include <apps/constant.h>
#include <apps/shared/poincare_helpers.h>
#include <cmath>
#include <string.h>

using namespace Poincare;

namespace Shared {

Context * TextFieldDelegateApp::localContext() {
  return container()->globalContext();
}

char TextFieldDelegateApp::XNT() {
  return 'X';
}

bool TextFieldDelegateApp::textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) {
  return isFinishingEvent(event);
}

bool TextFieldDelegateApp::textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) {
  if (textField->isEditing() && textField->shouldFinishEditing(event)) {
    if (!isAcceptableText(textField->text())) {
      textField->app()->displayWarning(I18n::Message::SyntaxError);
      return true;
    }
  }
  if (fieldDidReceiveEvent(textField, textField, event)) {
    return true;
  }
  return false;
}


bool TextFieldDelegateApp::isAcceptableText(const char * text) {
  Expression exp = Expression::Parse(text);
  return isAcceptableExpression(exp);
}

/* Protected */

TextFieldDelegateApp::TextFieldDelegateApp(Container * container, Snapshot * snapshot, ViewController * rootViewController) :
  InputEventHandlerDelegateApp(container, snapshot, rootViewController),
  TextFieldDelegate()
{
}

bool TextFieldDelegateApp::fieldDidReceiveEvent(EditableField * field, Responder * responder, Ion::Events::Event event) {
  if (event == Ion::Events::XNT) {
    if (!field->isEditing()) {
      field->setEditing(true);
    }
    const char xnt[2] = {field->XNTChar(XNT()), 0};
    return field->handleEventWithText(xnt);
  }
  return false;
}

bool TextFieldDelegateApp::isFinishingEvent(Ion::Events::Event event) {
  return event == Ion::Events::OK || event == Ion::Events::EXE;
}

bool TextFieldDelegateApp::isAcceptableExpression(const Expression exp) {
  if (exp.isUninitialized()) {
    return false;
  }
  if (!storeExpressionAllowed() && exp.type() == ExpressionNode::Type::Store) {
    return false;
  }
  return true;
}

bool TextFieldDelegateApp::ExpressionCanBeSerialized(const Expression expression, bool replaceAns, Expression ansExpression) {
  if (expression.isUninitialized()) {
    return false;
  }
  Expression exp = expression;
  if (replaceAns){
    exp = expression.clone();
    Symbol ansSymbol = Symbol::Ans();
    exp = exp.replaceSymbolWithExpression(ansSymbol, ansExpression);
  }
  constexpr int maxSerializationSize = Constant::MaxSerializedExpressionSize;
  char buffer[maxSerializationSize];
  int length = PoincareHelpers::Serialize(exp, buffer, maxSerializationSize);
  /* If the buffer is totally full, it is VERY likely that writeTextInBuffer
   * escaped before printing utterly the expression. */
  if (length >= maxSerializationSize-1) {
    return false;
  }
  if (replaceAns) {
    exp = Expression::Parse(buffer);
    if (exp.isUninitialized()) {
      // The ans replacement made the expression unparsable
      return false;
    }
  }
  return true;
}

}
