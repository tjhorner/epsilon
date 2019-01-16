#include "storage_function_graph_view.h"
#include <assert.h>
#include <cmath>
#include <float.h>
using namespace Poincare;

namespace Shared {

StorageFunctionGraphView::StorageFunctionGraphView(InteractiveCurveViewRange * graphRange,
  CurveViewCursor * cursor, BannerView * bannerView, View * cursorView) :
  CurveView(graphRange, cursor, bannerView, cursorView),
  m_selectedRecord(),
  m_highlightedStart(NAN),
  m_highlightedEnd(NAN),
  m_shouldColorHighlighted(false),
  m_xLabels{},
  m_yLabels{},
  m_context(nullptr)
{
}

void StorageFunctionGraphView::drawRect(KDContext * ctx, KDRect rect) const {
  ctx->fillRect(rect, KDColorWhite);
  drawGrid(ctx, rect);
  drawAxes(ctx, rect);
  simpleDrawBothAxesLabels(ctx, rect);
}

void StorageFunctionGraphView::setContext(Context * context) {
  m_context = context;
}

Context * StorageFunctionGraphView::context() const {
  return m_context;
}

void StorageFunctionGraphView::selectRecord(Ion::Storage::Record record) {
  if (m_selectedRecord != record) {
    m_selectedRecord = record;
    reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
  }
}

void StorageFunctionGraphView::setAreaHighlight(float start, float end) {
  if (m_highlightedStart != start || m_highlightedEnd != end) {
    float dirtyStart = start > m_highlightedStart ? m_highlightedStart : start;
    float dirtyEnd = start > m_highlightedStart ? start : m_highlightedStart;
    reloadBetweenBounds(dirtyStart, dirtyEnd);
    dirtyStart = end > m_highlightedEnd ? m_highlightedEnd : end;
    dirtyEnd = end > m_highlightedEnd ? end : m_highlightedEnd;
    reloadBetweenBounds(dirtyStart, dirtyEnd);
    if (std::isnan(start) || std::isnan(end)) {
      reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
    }
    if (std::isnan(m_highlightedStart) || std::isnan(m_highlightedEnd)) {
      reloadBetweenBounds(start, end);
    }
    m_highlightedStart = start;
    m_highlightedEnd = end;
  }
}

void StorageFunctionGraphView::setAreaHighlightColor(bool highlightColor) {
  if (m_shouldColorHighlighted != highlightColor) {
    reloadBetweenBounds(m_highlightedStart, m_highlightedEnd);
    m_shouldColorHighlighted = highlightColor;
  }
}

char * StorageFunctionGraphView::label(Axis axis, int index) const {
  return (axis == Axis::Horizontal ? (char *)m_xLabels[index] : (char *)m_yLabels[index]);
}

void StorageFunctionGraphView::reloadBetweenBounds(float start, float end) {
  if (start == end) {
    return;
  }
  float pixelLowerBound = floatToPixel(Axis::Horizontal, start)-2.0;
  float pixelUpperBound = floatToPixel(Axis::Horizontal, end)+4.0;
  /* We exclude the banner frame from the dirty zone to avoid unnecessary
   * redrawing */
  KDRect dirtyZone(KDRect(pixelLowerBound, 0, pixelUpperBound-pixelLowerBound,
        bounds().height()-m_bannerView->bounds().height()));
  markRectAsDirty(dirtyZone);
}


}
