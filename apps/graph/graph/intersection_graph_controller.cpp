#include "intersection_graph_controller.h"
#include "../app.h"
#include "../../shared/poincare_helpers.h"

using namespace Shared;

namespace Graph {

IntersectionGraphController::IntersectionGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, CurveViewCursor * cursor) :
  CalculationGraphController(parentResponder, graphView, bannerView, curveViewRange, cursor, I18n::Message::NoIntersectionFound),
  m_intersectedRecord()
{
}

const char * IntersectionGraphController::title() {
  return I18n::translate(I18n::Message::Intersection);
}

void IntersectionGraphController::reloadBannerView() {
  m_bannerView->setNumberOfSubviews(2);
  reloadBannerViewForCursorOnFunction(m_cursor, m_record, functionStore(), StorageCartesianFunctionStore::Symbol());
  constexpr size_t bufferSize = FunctionBannerDelegate::k_maxNumberOfCharacters+Poincare::PrintFloat::bufferSizeForFloatsWithPrecision(Constant::LargeNumberOfSignificantDigits);
  char buffer[bufferSize];
  const char * space = " ";
  const char * legend = "=";
  // 'f(x)=g(x)=', keep 2 chars for '='
  ExpiringPointer<StorageCartesianFunction> f = functionStore()->modelForRecord(m_record);
  int numberOfChar = f->nameWithArgument(buffer, bufferSize-2, StorageCartesianFunctionStore::Symbol());
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  // keep 1 char for '=';
  ExpiringPointer<StorageCartesianFunction> g = functionStore()->modelForRecord(m_intersectedRecord);
  numberOfChar += g->nameWithArgument(buffer+numberOfChar, bufferSize-numberOfChar-1, StorageCartesianFunctionStore::Symbol());
  numberOfChar += strlcpy(buffer+numberOfChar, legend, bufferSize-numberOfChar);
  numberOfChar += PoincareHelpers::ConvertFloatToText<double>(m_cursor->y(), buffer+numberOfChar, bufferSize-numberOfChar, Constant::MediumNumberOfSignificantDigits);
  strlcpy(buffer+numberOfChar, space, bufferSize-numberOfChar);
  bannerView()->setLegendAtIndex(buffer, 1);
}

Poincare::Expression::Coordinate2D IntersectionGraphController::computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) {
  Poincare::Expression::Coordinate2D result = {.abscissa = NAN, .value = NAN};
  for (int i = 0; i < functionStore()->numberOfActiveFunctions(); i++) {
    Ion::Storage::Record record = functionStore()->activeRecordAtIndex(i);
    if (record != m_record) {
      Poincare::Expression e = functionStore()->modelForRecord(record)->expressionReduced(context);
      Poincare::Expression::Coordinate2D intersection = functionStore()->modelForRecord(m_record)->nextIntersectionFrom(start, step, max, context, e);
      if ((std::isnan(result.abscissa) || std::fabs(intersection.abscissa-start) < std::fabs(result.abscissa-start)) && !std::isnan(intersection.abscissa)) {
        m_intersectedRecord = record;
        result = (std::isnan(result.abscissa) || std::fabs(intersection.abscissa-start) < std::fabs(result.abscissa-start)) ? intersection : result;
      }
    }
  }
  return result;
}

}
