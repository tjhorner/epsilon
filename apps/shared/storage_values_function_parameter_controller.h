#ifndef SHARED_STORAGE_VALUES_FUNCTION_PARAM_CONTROLLER_H
#define SHARED_STORAGE_VALUES_FUNCTION_PARAM_CONTROLLER_H

#include <escher.h>
#include "storage_function.h"
#include "../i18n.h"

namespace Shared {

class StorageValuesFunctionParameterController : public ViewController, public SimpleListViewDataSource, public SelectableTableViewDataSource {
public:
  StorageValuesFunctionParameterController(char symbol) :
    ViewController(nullptr),
    m_copyColumn(I18n::Message::CopyColumnInList),
    m_selectableTableView(this, this, this),
    m_record(),
    m_symbol(symbol)
  {}

  View * view() override { return &m_selectableTableView; }
  const char * title() override;
  void viewWillAppear() override;
  void didBecomeFirstResponder() override;
  virtual int numberOfRows() override { return 1; }
  KDCoordinate cellHeight() override { return Metric::ParameterCellHeight; }
  virtual HighlightCell * reusableCell(int index) override {
    assert(index == 0);
    return &m_copyColumn;
  }
  virtual int reusableCellCount() override { return 1; }
  void setRecord(Ion::Storage::Record record) { m_record = record; }
protected:
  MessageTableCellWithChevron m_copyColumn;
  SelectableTableView m_selectableTableView;
  Ion::Storage::Record m_record;
private:
  char m_pageTitle[StorageFunction::k_maxNameWithArgumentSize];
  char m_symbol;
};

}

#endif

