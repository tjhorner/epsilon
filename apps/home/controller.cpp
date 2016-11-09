#include "controller.h"
#include "../apps_container.h"
extern "C" {
#include <assert.h>
}

namespace Home {

Controller::Controller(Responder * parentResponder, ::AppsContainer * container) :
  ViewController(parentResponder),
  m_container(container),
  m_selectableTableView(SelectableTableView(this, this))
{
}

bool Controller::handleEvent(Ion::Events::Event event) {
  if (event == Ion::Events::OK) {
    m_container->switchTo(m_container->appAtIndex(m_selectableTableView.selectedRow()*k_numberOfColumns+m_selectableTableView.selectedColumn()+1));
    return true;
  }
  return false;
}

void Controller::didBecomeFirstResponder() {
  if (m_selectableTableView.selectedRow() == -1) {
    m_selectableTableView.selectCellAtLocation(0, 0);
  } else {
    m_selectableTableView.selectCellAtLocation(m_selectableTableView.selectedColumn(), m_selectableTableView.selectedRow());
  }
  app()->setFirstResponder(&m_selectableTableView);
}

View * Controller::view() {
  return &m_selectableTableView;
}

int Controller::numberOfRows() {
  return ((numberOfIcons()-1)/k_numberOfColumns)+1;
}

int Controller::numberOfColumns() {
  return k_numberOfColumns;
}

KDCoordinate Controller::cellHeight() {
  return 50;
}

KDCoordinate Controller::cellWidth() {
  return 50;
}

TableViewCell * Controller::reusableCell(int index) {
  return &m_cells[index];
}

int Controller::reusableCellCount() {
  return k_maxNumberOfCells;
}

void Controller::willDisplayCellAtLocation(TableViewCell * cell, int i, int j) {
  AppCell * appCell = (AppCell *)cell;
  int appIndex = (j*k_numberOfColumns+i)+1;
  if (appIndex >= m_container->numberOfApps()) {
    appCell->setVisible(false);
  } else {
    appCell->setVisible(true);
    ::App * app = m_container->appAtIndex((j*k_numberOfColumns+i)+1);
    appCell->setApp(app);
  }
}

int Controller::numberOfIcons() {
  assert(m_container->numberOfApps() > 0);
  return m_container->numberOfApps() - 1;
}

}
