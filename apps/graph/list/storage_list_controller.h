#ifndef GRAPH_STORAGE_LIST_CONTROLLER_H
#define GRAPH_STORAGE_LIST_CONTROLLER_H

#include <escher.h>
#include "list_parameter_controller.h"
#include "text_field_function_title_cell.h"
#include "../storage_cartesian_function_store.h"
#include <apps/shared/function_expression_cell.h>
#include <apps/shared/storage_function_list_controller.h>
#include <apps/shared/text_field_delegate.h>

namespace Graph {

class StorageListController : public Shared::StorageFunctionListController, public Shared::TextFieldDelegate {
public:
  StorageListController(Responder * parentResponder, ButtonRowController * header, ButtonRowController * footer);
  const char * title() override;
  void renameSelectedFunction();
  // TextFieldDelegate
  bool textFieldDidFinishEditing(TextField * textField, const char * text, Ion::Events::Event event) override;
  bool textFieldDidAbortEditing(TextField * textField) override;
  bool textFieldShouldFinishEditing(TextField * textField, Ion::Events::Event event) override;
  bool textFieldDidReceiveEvent(TextField * textField, Ion::Events::Event event) override;
private:
  constexpr static int k_maxNumberOfDisplayableRows = 5;
  Shared::StorageListParameterController * parameterController() override;
  int maxNumberOfDisplayableRows() override;
  Shared::FunctionTitleCell * titleCells(int index) override;
  HighlightCell * expressionCells(int index) override;
  void willDisplayTitleCellAtIndex(HighlightCell * cell, int j) override;
  void willDisplayExpressionCellAtIndex(HighlightCell * cell, int j) override;
  Shared::TextFieldDelegateApp * textFieldDelegateApp() override {
    return static_cast<Shared::TextFieldDelegateApp *>(app());
  }
  void setFunctionNameInTextField(Shared::ExpiringPointer<Shared::StorageFunction> function, TextField * textField);
  TextFieldFunctionTitleCell m_functionTitleCells[k_maxNumberOfDisplayableRows];
  Shared::FunctionExpressionCell m_expressionCells[k_maxNumberOfDisplayableRows];
  ListParameterController m_parameterController;
};

}

#endif
