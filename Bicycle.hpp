#pragma once
#include <memory>

#include "Transport.hpp"

class Bicycle : public Transport {
 public:
  Bicycle(std::string id, std::string name, int beginnerScore,
          std::shared_ptr<const ModelSpec> model)
      : Transport(id, name, beginnerScore, model) {}
  std::string describe() const override { return InventoryId() + " " + name(); }
};