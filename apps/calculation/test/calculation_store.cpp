#include <quiz.h>
#include <apps/shared/global_context.h>
#include <poincare/test/helper.h>
#include <string.h>
#include <assert.h>
#include "../calculation_store.h"

using namespace Poincare;
using namespace Calculation;

void assert_store_is(CalculationStore * store, const char * result[10]) {
  for (int i = 0; i < store->numberOfCalculations(); i++) {
    quiz_assert(strcmp(store->calculationAtIndex(i)->inputText(), result[i]) == 0);
  }
}

QUIZ_CASE(calculation_store_ring_buffer) {
  Shared::GlobalContext globalContext;
  CalculationStore store;
  quiz_assert(CalculationStore::k_maxNumberOfCalculations == 10);
  for (int i = 0; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    char text[2] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
    quiz_assert(store.numberOfCalculations() == i+1);
  }
  /* Store is now {0, 1, 2, 3, 4, 5, 6, 7, 8, 9} */
  const char * result[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
  assert_store_is(&store, result);

  store.push("10", &globalContext);
  /* Store is now {1, 2, 3, 4, 5, 6, 7, 8, 9, 10} */
  const char * result1[10] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
  assert_store_is(&store, result1);

  for (int i = 9; i > 0; i = i-2) {
   store.deleteCalculationAtIndex(i);
  }
  /* Store is now {1, 3, 5, 7, 9} */
  const char * result2[10] = {"1", "3", "5", "7", "9", "", "", "", "", ""};
  assert_store_is(&store, result2);

  for (int i = 5; i < CalculationStore::k_maxNumberOfCalculations; i++) {
    char text[3] = {(char)(i+'0'), 0};
    store.push(text, &globalContext);
    quiz_assert(store.numberOfCalculations() == i+1);
  }
  /* Store is now {0, 2, 4, 6, 8, 5, 6, 7, 8, 9} */
  const char * result3[10] = {"1", "3", "5", "7", "9", "5", "6", "7", "8", "9"};
  assert_store_is(&store, result3);

  store.deleteAll();
}

QUIZ_CASE(calculation_ans) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  store.push("1+3/4", &globalContext);
  store.push("ans+2/3", &globalContext);
  ::Calculation::Calculation * lastCalculation = store.calculationAtIndex(1);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == false);
  quiz_assert(strcmp(lastCalculation->exactOutputText(),"29/12") == 0);

  store.push("ans+0.22", &globalContext);
  lastCalculation = store.calculationAtIndex(2);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(&globalContext) == true);
  quiz_assert(strcmp(lastCalculation->approximateOutputText(),"2.6366666666667") == 0);

  store.deleteAll();
}

void assertCalculationDisplay(const char * input, bool displayExactOutput, bool displayApproximateOutput, ::Calculation::Calculation::EqualSign sign, const char * exactOutput, const char * approximateOutput, Context * context, CalculationStore * store) {
  char buffer[500];
  strlcpy(buffer, input, sizeof(buffer));
  translate_in_special_chars(buffer);
  store->push(buffer, context);
  ::Calculation::Calculation * lastCalculation = store->calculationAtIndex(1);
  quiz_assert(lastCalculation->shouldOnlyDisplayExactOutput() == displayExactOutput);
  quiz_assert(lastCalculation->shouldOnlyDisplayApproximateOutput(context) == displayApproximateOutput);
  if (sign != ::Calculation::Calculation::EqualSign::Unknown) {
    quiz_assert(lastCalculation->exactAndApproximateDisplayedOutputsAreEqual(context) == sign);
  }
  if (exactOutput) {
    strlcpy(buffer, exactOutput, sizeof(buffer));
    translate_in_special_chars(buffer);
    quiz_assert(strcmp(lastCalculation->exactOutputText(), buffer) == 0);
  }
  if (approximateOutput) {
    strlcpy(buffer, approximateOutput, sizeof(buffer));
    translate_in_special_chars(buffer);
    quiz_assert(strcmp(lastCalculation->approximateOutputText(),buffer) == 0);
  }
  store->deleteAll();
}

QUIZ_CASE(calculation_display_exact_approximate) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  assertCalculationDisplay("1/2", false, false, ::Calculation::Calculation::EqualSign::Equal, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("1/3", false, false, ::Calculation::Calculation::EqualSign::Approximation, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("1/0", true, false, ::Calculation::Calculation::EqualSign::Unknown, "undef", "undef", &globalContext, &store);
  assertCalculationDisplay("2x-x", true, false, ::Calculation::Calculation::EqualSign::Unknown, "x", "undef", &globalContext, &store);
  assertCalculationDisplay("[[1,2,3]]", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("[[1,x,3]]", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("28^7", false, false, ::Calculation::Calculation::EqualSign::Unknown, nullptr, nullptr, &globalContext, &store);
  assertCalculationDisplay("3+R(2)>a", false, false, ::Calculation::Calculation::EqualSign::Approximation, "R(2)+3", nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationDisplay("3+2>a", false, true, ::Calculation::Calculation::EqualSign::Equal, "5", "5", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationDisplay("3>a", false, true, ::Calculation::Calculation::EqualSign::Equal, "3", "3", &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("a.exp").destroy();
  assertCalculationDisplay("3+x>f(x)", true, false, ::Calculation::Calculation::EqualSign::Unknown, "x+3", nullptr, &globalContext, &store);
  Ion::Storage::sharedStorage()->recordNamed("f.func").destroy();
}

QUIZ_CASE(calculation_complex_format) {
  Shared::GlobalContext globalContext;
  CalculationStore store;

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Real);
  assertCalculationDisplay("1+I", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "1+I", &globalContext, &store);
  assertCalculationDisplay("R(-1)", false, true, ::Calculation::Calculation::EqualSign::Unknown, "unreal", nullptr, &globalContext, &store);
  assertCalculationDisplay("ln(-2)", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", &globalContext, &store);
  assertCalculationDisplay("R(-1)*R(-1)", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", &globalContext, &store);
  assertCalculationDisplay("(-8)^(1/3)", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "-2", &globalContext, &store);
  assertCalculationDisplay("(-8)^(2/3)", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "4", &globalContext, &store);
  assertCalculationDisplay("(-2)^(1/4)", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "unreal", &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
  assertCalculationDisplay("1+I", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "1+I", &globalContext, &store);
  assertCalculationDisplay("R(-1)", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "I", &globalContext, &store);
  assertCalculationDisplay("ln(-2)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "ln(-2)", nullptr, &globalContext, &store);
  assertCalculationDisplay("R(-1)*R(-1)", false, true, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "-1", &globalContext, &store);
  assertCalculationDisplay("(-8)^(1/3)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "1+R(3)*I", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-8)^(2/3)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "-2+2*R(3)*I", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-2)^(1/4)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "root(8,4)/2+root(8,4)/2*I", nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Polar);
  assertCalculationDisplay("1+I", false, false, ::Calculation::Calculation::EqualSign::Approximation, "R(2)*X^(P/4*I)", nullptr, &globalContext, &store);
  assertCalculationDisplay("R(-1)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "X^(P/2*I)", nullptr, &globalContext, &store);
  assertCalculationDisplay("ln(-2)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "ln(-2)", nullptr, &globalContext, &store);
  assertCalculationDisplay("R(-1)*R(-1)", false, false, ::Calculation::Calculation::EqualSign::Unknown, nullptr, "X^(3.1415926535898*I)", &globalContext, &store);
  assertCalculationDisplay("(-8)^(1/3)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "2*X^(P/3*I)", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-8)^(2/3)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "4*X^((2*P)/3*I)", nullptr, &globalContext, &store);
  assertCalculationDisplay("(-2)^(1/4)", false, false, ::Calculation::Calculation::EqualSign::Approximation, "root(2,4)*X^(P/4*I)", nullptr, &globalContext, &store);

  Poincare::Preferences::sharedPreferences()->setComplexFormat(Poincare::Preferences::ComplexFormat::Cartesian);
}
