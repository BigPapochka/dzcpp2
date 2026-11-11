#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include "Errors.hpp"
#include "Inventory.hpp"

template <typename T>
struct RegistryPrinter {
  static void print(std::ostream& out, const T& value) { out << value; }
};

template <>
struct RegistryPrinter<const InventoryItem*> {
  static void print(std::ostream& out, const InventoryItem* value) {
    if (!value) throw ArgumentError("cannot print a null item");
    out << *value;
  }
};

template <>
struct RegistryPrinter<std::unique_ptr<InventoryItem>> {
  static void print(std::ostream& out,
                    const std::unique_ptr<InventoryItem>& value) {
    if (!value) throw ArgumentError("cannot print a null item");
    out << *value;
  }
};

template <typename T>
class Registry {
 private:
  std::vector<T> items_;

 public:
  void add(T value) { items_.push_back(std::move(value)); }
  std::size_t size() const { return items_.size(); }
  const T& operator[](std::size_t index) const { return items_.at(index); }
  void erase(std::size_t index) {
    if (index >= items_.size()) {
      throw std::out_of_range("registry index out of range");
    }
    using Difference = typename std::vector<T>::difference_type;
    items_.erase(items_.begin() + static_cast<Difference>(index));
  }
  template <typename Compare>
  void sort(Compare compare) {
    std::sort(items_.begin(), items_.end(), compare);
  }
  friend std::ostream& operator<<(std::ostream& out, const Registry& registry) {
    for (const auto& value : registry.items_) {
      RegistryPrinter<T>::print(out, value);
      out << '\n';
    }
    return out;
  }
};
