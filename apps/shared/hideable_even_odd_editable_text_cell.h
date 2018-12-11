#ifndef APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H
#define APPS_SHARED_HIDEABLE_EVEN_ODD_EDITABLE_TEXT_CELL_H

#include <escher/even_odd_editable_text_cell.h>
#include <escher/palette.h>
#include "hideable.h"

namespace Shared {

class HideableEvenOddEditableTextCell : public EvenOddEditableTextCell, public Hideable {
public:
  HideableEvenOddEditableTextCell(Responder * parentResponder = nullptr, InputEventHandlerDelegate * inputEventHandlerDelegate = nullptr, TextFieldDelegate * delegate = nullptr, char * draftTextBuffer = nullptr) :
    EvenOddEditableTextCell(parentResponder, inputEventHandlerDelegate, delegate, draftTextBuffer, KDFont::SmallFont),
    Hideable()
  {}
  KDColor backgroundColor() const override;
  void setHide(bool hide) override;
};

}

#endif
