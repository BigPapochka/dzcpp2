#pragma once

class EnergyConsumer {
 public:
  virtual ~EnergyConsumer() = default;

  virtual double dailyEnergy() const = 0;
};