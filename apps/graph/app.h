#ifndef GRAPH_APP_H
#define GRAPH_APP_H

#include <escher.h>
#include "storage_cartesian_function_store.h"
#include "graph/graph_controller.h"
#include "list/storage_list_controller.h"
#include "values/storage_values_controller.h"
#include "../shared/storage_function_app.h"

namespace Graph {

class App : public Shared::StorageFunctionApp {
public:
  class Descriptor : public ::App::Descriptor {
  public:
    I18n::Message name() override;
    I18n::Message upperName() override;
    const Image * icon() override;
  };
  class Snapshot : public Shared::StorageFunctionApp::Snapshot {
  public:
    Snapshot();
    App * unpack(Container * container) override;
    void reset() override;
    void storageDidChangeForRecord(const Ion::Storage::Record record) override;
    Descriptor * descriptor() override;
    StorageCartesianFunctionStore * functionStore();
    Shared::InteractiveCurveViewRange * graphRange();
  private:
    void tidy() override;
    StorageCartesianFunctionStore m_functionStore;
    Shared::InteractiveCurveViewRange m_graphRange;
  };
  InputViewController * inputViewController() override;
  char XNT() override;
  NestedMenuController * variableBoxForInputEventHandler(InputEventHandler * textInput) override;
  StorageCartesianFunctionStore * functionStore() override { return static_cast<Snapshot *>(snapshot())->functionStore(); }
private:
  App(Container * container, Snapshot * snapshot);
  StorageListController m_listController;
  ButtonRowController m_listFooter;
  ButtonRowController m_listHeader;
  StackViewController m_listStackViewController;
  GraphController m_graphController;
  AlternateEmptyViewController m_graphAlternateEmptyViewController;
  ButtonRowController m_graphHeader;
  StackViewController m_graphStackViewController;
  StorageValuesController m_valuesController;
  AlternateEmptyViewController m_valuesAlternateEmptyViewController;
  ButtonRowController m_valuesHeader;
  StackViewController m_valuesStackViewController;
  TabViewController m_tabViewController;
  InputViewController m_inputViewController;
};

}

#endif
