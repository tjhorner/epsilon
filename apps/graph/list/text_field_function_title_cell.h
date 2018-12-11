#ifndef GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H
#define GRAPH_LIST_TEXT_FIELD_FUNCTION_TITLE_CELL_H

#include <apps/shared/function_title_cell.h>
#include <apps/shared/storage_function.h>
#include <apps/shared/text_field_with_extension.h>

namespace Graph {

class StorageListController;

class TextFieldFunctionTitleCell : public Shared::FunctionTitleCell, public Responder {
public:
  TextFieldFunctionTitleCell(StorageListController * listController, Orientation orientation = Orientation::VerticalIndicator, const KDFont * font = KDFont::LargeFont);
  TextField * textField() { return &m_textField; }
  void setEditing(bool editing);
  bool isEditing() const;
  void setText(const char * textContent);

  // FunctionTitleCell
  void setColor(KDColor color) override;
  const KDFont * font() const override { return m_textField.font(); }
  // EvenOddCell
  void setEven(bool even) override;
  // HighlightCell
  void setHighlighted(bool highlight) override;
  Responder * responder() override;
  const char * text() const override {
    return m_textField.text();
  }
  // View
  int numberOfSubviews() const override { return 1; }
  View * subviewAtIndex(int index) override {
    assert(index == 0);
    return &m_textField;
  }
  void layoutSubviews() override;

  // Responder
  void didBecomeFirstResponder() override;
protected:
  KDRect textFieldFrame() const;
private:
  constexpr static int k_textFieldBufferSize = Shared::StorageFunction::k_maxNameWithArgumentSize;
  Shared::TextFieldWithExtension m_textField;
  char m_textFieldBuffer[k_textFieldBufferSize];
};

}

#endif
