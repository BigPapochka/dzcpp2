#pragma once
#include <string>
#include <utility>

#include "EnergyConsumer.hpp"
#include "Inventory.hpp"
#include "Validation.hpp"

class Dock : public EnergyConsumer, public InventoryItem {
 private:
  std::string id_;
  std::string name_;
  double energy_;

 public:
  Dock(std::string id, std::string name, double energy)
      : id_{std::move(id)}, name_{std::move(name)}, energy_{energy} {
    validateId(id_);
    validateName(name_);
    validateEnergy(energy_);
  }
  std::string InventoryId() const override { return id_; }
  std::string name() const override { return name_; }
  std::string describe() const override { return id_ + " " + name_; }
  double dailyEnergy() const override { return energy_; }
};
