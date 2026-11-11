#pragma once
#include <string>

#include "Transport.hpp"

class Skateboard final : public Transport {
 public:
  using Transport::Transport;
  std::string describe() const override { return InventoryId() + " " + name(); }
};
