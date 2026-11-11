#pragma once
#include <memory>
#include <string>

#include "EnergyConsumer.hpp"
#include "Transport.hpp"
#include "Validation.hpp"
class ElectricBike : public Transport, public EnergyConsumer {
 private:
  double energy_;

 public:
  ElectricBike(std::string id, std::string name, double energy,
               int beginnerScore, std::shared_ptr<const ModelSpec> model)
      : Transport(id, name, beginnerScore, model), energy_{energy} {
    validateEnergy(energy);
  }
  double dailyEnergy() const override { return energy_; }
  std::string describe() const override { return InventoryId() + " " + name(); }
};
