#pragma once
#include <memory>
#include <ostream>
#include <string>

#include "FocusCard.hpp"

class InventoryItem {
 private:
  std::shared_ptr<FocusCard> focusCard_ = std::make_shared<FocusCard>();

 public:
  InventoryItem() = default;
  InventoryItem(const InventoryItem&) = delete;
  InventoryItem& operator=(const InventoryItem&) = delete;
  virtual ~InventoryItem() { focusCard_->alive = false; }
  virtual std::string InventoryId() const = 0;
  virtual std::string name() const = 0;
  virtual std::string describe() const = 0;
  std::weak_ptr<const FocusCard> focusCard() {
    focusCard_->description = describe();
    return focusCard_;
  }
};

inline std::ostream& operator<<(std::ostream& out, const InventoryItem& item) {
  return out << item.describe();
}
inline bool operator<(const InventoryItem& left, const InventoryItem& right) {
  return left.InventoryId() < right.InventoryId();
}
inline bool operator==(const InventoryItem& left, const InventoryItem& right) {
  return left.InventoryId() == right.InventoryId();
}
