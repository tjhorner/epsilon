#include "graph_controller_helper.h"
#include "../app.h"
#include "../../constant.h"
#include "../../shared/poincare_helpers.h"

using namespace Shared;
using namespace Poincare;

namespace Graph {

bool GraphControllerHelper::privateMoveCursorHorizontally(Shared::CurveViewCursor * cursor, int direction, Shared::InteractiveCurveViewRange * range, int numberOfStepsInGradUnit, Ion::Storage::Record record, App * app, float cursorTopMarginRatio, float cursorRightMarginRatio, float cursorBottomMarginRatio, float cursorLeftMarginRatio) {
  ExpiringPointer<StorageCartesianFunction> function = app->functionStore()->modelForRecord(record);
  double xCursorPosition = cursor->x();
  double x = direction > 0 ? xCursorPosition + range->xGridUnit()/numberOfStepsInGradUnit : xCursorPosition -  range->xGridUnit()/numberOfStepsInGradUnit;
  double y = function->evaluateAtAbscissa(x, app->localContext());
  cursor->moveTo(x, y);
  range->panToMakePointVisible(x, y, cursorTopMarginRatio, cursorRightMarginRatio, cursorBottomMarginRatio, cursorLeftMarginRatio);
  return true;
}

void GraphControllerHelper::reloadDerivativeInBannerViewForCursorOnFunction(Shared::CurveViewCursor * cursor, Ion::Storage::Record record, App * app) {
  ExpiringPointer<StorageCartesianFunction> function = app->functionStore()->modelForRecord(record);
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  int numberOfChar = function->derivativeNameWithArgument(buffer, bufferSize, StorageCartesianFunctionStore::Symbol());
  const char * legend = "=";
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  double y = function->approximateDerivative(cursor->x(), app->localContext());
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(y, buffer + numberOfChar, bufferSize-numberOfChar, Constant::ShortNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->setLegendAtIndex(buffer, 2);
}

}
