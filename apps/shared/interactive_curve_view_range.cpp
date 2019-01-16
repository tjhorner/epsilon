#include "interactive_curve_view_range.h"
#include <ion.h>
#include <cmath>
#include <float.h>
#include <stddef.h>
#include <assert.h>
#include <poincare/preferences.h>

using namespace Poincare;

namespace Shared {

static inline float min(float x, float y) { return (x<y ? x : y); }
static inline float max(float x, float y) { return (x>y ? x : y); }

uint32_t InteractiveCurveViewRange::rangeChecksum() {
  float data[5] = {m_xMin, m_xMax, m_yMin, m_yMax, m_yAuto ? 1.0f : 0.0f};
  size_t dataLengthInBytes = 5*sizeof(float);
  assert((dataLengthInBytes & 0x3) == 0); // Assert that dataLengthInBytes is a multiple of 4
  return Ion::crc32((uint32_t *)data, dataLengthInBytes/sizeof(uint32_t));
}

void InteractiveCurveViewRange::setYAuto(bool yAuto) {
  m_yAuto = yAuto;
  notifyRangeChange();
}

void InteractiveCurveViewRange::setXMin(float xMin) {
  float newXMin = xMin;
  MemoizedCurveViewRange::setXMin(clipped(newXMin, false));
  if (m_xMax - newXMin < k_minFloat) {
    newXMin = m_xMax - k_minFloat;
    MemoizedCurveViewRange::setXMin(clipped(newXMin, false));
  }
  notifyRangeChange();
}

void InteractiveCurveViewRange::setXMax(float xMax) {
  float newXMax = xMax;
  MemoizedCurveViewRange::setXMax(clipped(newXMax, true));
  if (newXMax - m_xMin < k_minFloat) {
    newXMax = m_xMin + k_minFloat;
    MemoizedCurveViewRange::setXMax(clipped(newXMax, true));
  }
  notifyRangeChange();
}

void InteractiveCurveViewRange::setYMin(float yMin) {
  float newYMin = yMin;
  MemoizedCurveViewRange::setYMin(clipped(newYMin, false));
  if (m_yMax - newYMin < k_minFloat) {
    newYMin = m_yMax - k_minFloat;
    MemoizedCurveViewRange::setYMin(clipped(newYMin, false));
  }
}

void InteractiveCurveViewRange::setYMax(float yMax) {
  float newYMax = yMax;
  MemoizedCurveViewRange::setYMax(clipped(newYMax, true));
  if (newYMax - m_yMin < k_minFloat) {
    newYMax = m_yMin + k_minFloat;
    MemoizedCurveViewRange::setYMax(clipped(newYMax, true));
  }
}

void InteractiveCurveViewRange::zoom(float ratio, float x, float y) {
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  if (ratio*std::fabs(xMax-xMin) < k_minFloat || ratio*std::fabs(yMax-yMin) < k_minFloat) {
    return;
  }
  float centerX = std::isnan(x) || std::isinf(x) ? (xMax+xMin)/2: x;
  float centerY = std::isnan(y) || std::isinf(y) ? (yMax+yMin)/2: y;
  float newXMin = clipped(centerX*(1.0f-ratio)+ratio*xMin, false);
  float newXMax = clipped(centerX*(1.0f-ratio)+ratio*xMax, true);
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xMax = newXMax;
    MemoizedCurveViewRange::setXMin(newXMin);
  }
  setYAuto(false);
  float newYMin = clipped(centerY*(1.0f-ratio)+ratio*yMin, false);
  float newYMax = clipped(centerY*(1.0f-ratio)+ratio*yMax, true);
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yMax = newYMax;
    MemoizedCurveViewRange::setYMin(newYMin);
  }
}

void InteractiveCurveViewRange::panWithVector(float x, float y) {
  setYAuto(false);
  if (clipped(m_xMin + x, false) != m_xMin + x || clipped(m_xMax + x, true) != m_xMax + x || clipped(m_yMin + y, false) != m_yMin + y || clipped(m_yMax + y, true) != m_yMax + y || std::isnan(clipped(m_xMin + x, false)) || std::isnan(clipped(m_xMax + x, true)) || std::isnan(clipped(m_yMin + y, false)) || std::isnan(clipped(m_yMax + y, true))) {
    return;
  }
  m_xMax = clipped(m_xMax + x, true);
  MemoizedCurveViewRange::setXMin(clipped(m_xMin + x, false));
  m_yMax = clipped(m_yMax + y, true);
  MemoizedCurveViewRange::setYMin(clipped(m_yMin + y, false));
}

void InteractiveCurveViewRange::roundAbscissa() {
  // Set x range
  float xMin = m_xMin;
  float xMax = m_xMax;
  float newXMin = clipped(std::round((xMin+xMax)/2) - (float)Ion::Display::Width/2.0f, false);
  float newXMax = clipped(std::round((xMin+xMax)/2) + (float)Ion::Display::Width/2.0f-1.0f, true);
  if (std::isnan(newXMin) || std::isnan(newXMax)) {
    return;
  }
  m_xMax = newXMax;
  MemoizedCurveViewRange::setXMin(newXMin);
  // Set y range
  notifyRangeChange();
}

void InteractiveCurveViewRange::normalize() {
  /* We center the ranges on the current range center, and put each axis so that
   * 1cm = 2 units. */
  float xMin = m_xMin;
  float xMax = m_xMax;
  float yMin = m_yMin;
  float yMax = m_yMax;
  // Set x range
  float newXMin = clipped((xMin+xMax)/2 - NormalizedXHalfRange(), false);
  float newXMax = clipped((xMin+xMax)/2 + NormalizedXHalfRange(), true);
  if (!std::isnan(newXMin) && !std::isnan(newXMax)) {
    m_xMax = newXMax;
    MemoizedCurveViewRange::setXMin(newXMin);
  }
  // Set y range
  setYAuto(false);
  float newYMin = clipped((yMin+yMax)/2 - NormalizedYHalfRange(), false);
  float newYMax = clipped((yMin+yMax)/2 + NormalizedYHalfRange(), true);
  if (!std::isnan(newYMin) && !std::isnan(newYMax)) {
    m_yMax = newYMax;
    MemoizedCurveViewRange::setYMin(newYMin);
  }
}

void InteractiveCurveViewRange::setTrigonometric() {
  // Set x range
  float x = (Preferences::sharedPreferences()->angleUnit() == Preferences::AngleUnit::Degree) ? 600.0f : 10.5f;
  m_xMax = x;
  MemoizedCurveViewRange::setXMin(-x);
  // Set y range
  setYAuto(false);
  float y = 1.6f;
  m_yMax = y;
  MemoizedCurveViewRange::setYMin(-y);
}

void InteractiveCurveViewRange::setDefault() {
  if (m_delegate == nullptr) {
    return;
  }
  m_xMax = m_delegate->interestingXHalfRange();
  MemoizedCurveViewRange::setXMin(-m_xMax);
  setYAuto(true);
}

void InteractiveCurveViewRange::centerAxisAround(Axis axis, float position) {
  if (std::isnan(position)) {
    return;
  }
  if (axis == Axis::X) {
    float range = m_xMax - m_xMin;
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = std::pow(10.0f, std::floor(std::log10(std::fabs(position)))-1.0f);
    }
    m_xMax = clipped(position + range/2.0f, true);
    MemoizedCurveViewRange::setXMin(clipped(position - range/2.0f, false));
  } else {
    setYAuto(false);
    float range = m_yMax - m_yMin;
    if (std::fabs(position/range) > k_maxRatioPositionRange) {
      range = std::pow(10.0f, std::floor(std::log10(std::fabs(position)))-1.0f);
    }
    m_yMax = clipped(position + range/2.0f, true);
    MemoizedCurveViewRange::setYMin(clipped(position - range/2.0f, false));
  }
}

void InteractiveCurveViewRange::panToMakePointVisible(float x, float y, float topMarginRatio, float rightMarginRatio, float bottomMarginRation, float leftMarginRation) {
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  if (x < m_xMin + leftMarginRation*xRange - FLT_EPSILON && !std::isinf(x) && !std::isnan(x)) {
    float newXMin = clipped(x - leftMarginRation*xRange, false);
    m_xMax = clipped(newXMin + xRange, true);
    MemoizedCurveViewRange::setXMin(newXMin);
    setYAuto(false);
  }
  if (x > m_xMax - rightMarginRatio*xRange + FLT_EPSILON && !std::isinf(x) && !std::isnan(x)) {
    m_xMax = clipped(x + rightMarginRatio*xRange, true);
    MemoizedCurveViewRange::setXMin(clipped(m_xMax - xRange, false));
    setYAuto(false);
  }
  if (y < m_yMin + bottomMarginRation*yRange - FLT_EPSILON && !std::isinf(y) && !std::isnan(y)) {
    float newYMin = clipped(y - bottomMarginRation*yRange, false);
    m_yMax = clipped(newYMin + yRange, true);
    MemoizedCurveViewRange::setYMin(newYMin);
    setYAuto(false);
  }
  if (y > m_yMax - topMarginRatio*yRange + FLT_EPSILON && !std::isinf(y) && !std::isnan(y)) {
    m_yMax = clipped(y + topMarginRatio*yRange, true);
    MemoizedCurveViewRange::setYMin(clipped(m_yMax - yRange, false));
    setYAuto(false);
  }
}

bool InteractiveCurveViewRange::isCursorVisible(float topMarginRatio, float rightMarginRatio, float bottomMarginRation, float leftMarginRation) {
  float xRange = m_xMax - m_xMin;
  float yRange = m_yMax - m_yMin;
  return m_cursor->x() >= m_xMin + leftMarginRation*xRange && m_cursor->x() <= m_xMax - rightMarginRatio*xRange && m_cursor->y() >= m_yMin + bottomMarginRation*yRange && m_cursor->y() <= m_yMax - topMarginRatio*yRange;
}

float InteractiveCurveViewRange::clipped(float x, bool isMax) {
  float maxF = isMax ? k_upperMaxFloat : k_lowerMaxFloat;
  float minF = isMax ? -k_lowerMaxFloat : -k_upperMaxFloat;
  return max(minF, min(x, maxF));
}

void InteractiveCurveViewRange::notifyRangeChange() {
  if (m_delegate) {
    m_delegate->didChangeRange(this);
  }
}

}
