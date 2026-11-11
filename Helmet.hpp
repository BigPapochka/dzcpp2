#pragma once
#include <string>
#include <utility>

#include "Inventory.hpp"
#include "Validation.hpp"

class Helmet : public InventoryItem {
 private:
  std::string id_;
  std::string name_;

 public:
  Helmet(std::string id, std::string name)
      : id_{std::move(id)}, name_{std::move(name)} {
    validateId(id_);
    validateName(name_);
  }
  std::string InventoryId() const override { return id_; }
  std::string name() const override { return name_; }
  std::string describe() const override { return id_ + " " + name_; }
};
