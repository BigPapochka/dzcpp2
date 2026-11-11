#pragma once
#include <cmath>
#include <cstddef>

#include "EnergyConsumer.hpp"
#include "Errors.hpp"

template <typename Container>
double sumDailyEnergy(const Container& items) {
  double total = 0.0;
  for (std::size_t i = 0; i < items.size(); ++i) {
    const auto* consumer = dynamic_cast<const EnergyConsumer*>(items[i].get());
    if (consumer) {
      const double next = total + consumer->dailyEnergy();
      if (!std::isfinite(next)) {
        throw ArgumentError("total energy overflow");
      }
      total = next;
    }
  }
  return total;
}
