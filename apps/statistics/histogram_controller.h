#ifndef STATISTICS_HISTOGRAM_CONTROLLER_H
#define STATISTICS_HISTOGRAM_CONTROLLER_H

#include <escher.h>
#include "store.h"
#include "histogram_view.h"
#include "histogram_banner_view.h"
#include "histogram_parameter_controller.h"

namespace Statistics {

class HistogramController : public ViewController, public ButtonRowDelegate, public AlternateEmptyViewDelegate {

public:
  HistogramController(Responder * parentResponder, ButtonRowController * header, Store * store, int series, uint32_t * m_storeVersion, uint32_t * m_barVersion, uint32_t * m_rangeVersion, int * m_selectedBarIndex);
  StackViewController * stackController();
  HistogramParameterController * histogramParameterController() { return &m_histogramParameterController; }
  void setCurrentDrawnSeries(int series);

  // AlternateEmptyViewDelegate
  bool isEmpty() const override;
  I18n::Message emptyMessage() override;
  Responder * defaultController() override;

  // ViewController
  const char * title() override;
  View * view() override { return &m_view; }
  void viewWillAppear() override;

  // Responder
  bool handleEvent(Ion::Events::Event event) override;
  void didBecomeFirstResponder() override;
  void willExitResponderChain(Responder * nextFirstResponder) override;
private:
  constexpr static int k_maxNumberOfBarsPerWindow = 100;
  constexpr static int k_maxIntervalLegendLength = 33;
  constexpr static int k_maxLegendLength = 13;
  constexpr static int k_maxNumberOfCharacters = 30;
  class ContentView : public View {
  public:
    ContentView(HistogramController * controller, Store * store);
    void reload();
    HistogramView * histogramViewAtIndex(int index);
    int seriesOfSubviewAtIndex(int index);
    int indexOfSubviewAtSeries(int series);
    HistogramBannerView * bannerView() { return &m_bannerView; }
    void setDisplayBanner(bool display) { m_displayBanner = display; }
    void selectHistogram(int index);
    void deselectHistogram(int index);
    // View
    void drawRect(KDContext * ctx, KDRect rect) const override;
    int numberOfSubviews() const override;
  private:
    KDRect bannerFrame() const;
    View * subviewAtIndex(int index) override;
    void layoutSubviews() override;
    void changeHistogramSelection(int index, bool select);
    HistogramView m_histogramView1;
    HistogramView m_histogramView2;
    HistogramView m_histogramView3;
    HistogramBannerView m_bannerView;
    bool m_displayBanner;
    Store * m_store; // TODO Do not duplicate this pointer
  };
  Responder * tabController() const;
  void reloadBannerView();
  void initRangeParameters();
  void initYRangeParameters(int series);
  void initBarParameters();
  void initBarSelection();
  // return true if the window has scrolled
  bool moveSelection(int deltaIndex);
  Store * m_store;
  ContentView m_view;
  uint32_t * m_storeVersion;
  uint32_t * m_barVersion;
  uint32_t * m_rangeVersion;
  int m_selectedSeries;
  int * m_selectedBarIndex;
  HistogramParameterController m_histogramParameterController;
};

}


#endif
