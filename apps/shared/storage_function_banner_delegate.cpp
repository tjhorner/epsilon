#include "storage_function_banner_delegate.h"
#include "poincare_helpers.h"
#include "../constant.h"

using namespace Poincare;

namespace Shared {

void StorageFunctionBannerDelegate::reloadBannerViewForCursorOnFunction(CurveViewCursor * cursor, Ion::Storage::Record record, StorageFunctionStore * functionStore, char symbol) {
  ExpiringPointer<StorageFunction> function = functionStore->modelForRecord(record);
  constexpr int bufferSize = k_maxNumberOfCharacters+PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  const char * legend = "0=";
  int legendLength = strlen(legend);
  int numberOfChar = 0;
  strlcpy(buffer, legend, bufferSize);
  numberOfChar += legendLength;
  buffer[0] = symbol;
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->x(), buffer+numberOfChar, PrintFloat::bufferSizeForFloatsWithPrecision(Constant::MediumNumberOfSignificantDigits), Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize - numberOfChar);
  bannerView()->setLegendAtIndex(buffer, 0);

  numberOfChar = 0;
  numberOfChar += function->nameWithArgument(buffer, bufferSize, symbol);
  legend = "=";
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->setLegendAtIndex(buffer, 1);
}

}
