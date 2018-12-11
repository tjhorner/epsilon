#ifndef GRAPH_INTERSECTION_GRAPH_CONTROLLER_H
#define GRAPH_INTERSECTION_GRAPH_CONTROLLER_H

#include "calculation_graph_controller.h"
#include "../storage_cartesian_function_store.h"

namespace Graph {

class IntersectionGraphController : public CalculationGraphController {
public:
  IntersectionGraphController(Responder * parentResponder, GraphView * graphView, BannerView * bannerView, Shared::InteractiveCurveViewRange * curveViewRange, Shared::CurveViewCursor * cursor);
  const char * title() override;
private:
  void reloadBannerView() override;
  Poincare::Expression::Coordinate2D computeNewPointOfInterest(double start, double step, double max, Poincare::Context * context) override;
  Ion::Storage::Record m_intersectedRecord;
};

}

#endif
