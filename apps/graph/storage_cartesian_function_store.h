#ifndef GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H
#define GRAPH_STORAGE_CARTESIAN_FUNCTION_STORE_H

#include "../shared/storage_cartesian_function.h"
#include "../shared/storage_function_store.h"
#include <stdint.h>
#include <escher.h>

namespace Graph {

class StorageCartesianFunctionStore : public Shared::StorageFunctionStore {
public:
  Shared::ExpiringPointer<Shared::StorageCartesianFunction> modelForRecord(Ion::Storage::Record record) const { return Shared::ExpiringPointer<Shared::StorageCartesianFunction>(static_cast<Shared::StorageCartesianFunction *>(privateModelForRecord(record))); }
  static char Symbol() { return 'x'; }
  char symbol() const override { return Symbol(); }
private:
  Ion::Storage::Record::ErrorStatus addEmptyModel() override;
  const char * modelExtension() const override { return Shared::GlobalContext::funcExtension; }
  void setMemoizedModelAtIndex(int cacheIndex, Ion::Storage::Record record) const override;
  Shared::StorageExpressionModel * memoizedModelAtIndex(int cacheIndex) const override;
  mutable Shared::StorageCartesianFunction m_functions[k_maxNumberOfMemoizedModels];
};

}

#endif
