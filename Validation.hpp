#pragma once
#include <cmath>
#include <string>

#include "Errors.hpp"

inline void validateEnergy(double energy) {
  if (!std::isfinite(energy) || energy < 0.0) {
    throw ArgumentError("energy must be finite and non-negative");
  }
}
inline void validateId(const std::string& id) {
  if (id.empty()) {
    throw ArgumentError("id must not be empty");
  }
}
inline void validateName(const std::string& name) {
  if (name.empty()) {
    throw ArgumentError("name must not be empty");
  }
}
inline void validateScore(int score) {
  if (score < 1 || score > 10) {
    throw ArgumentError("score must be between 1 and 10");
  }
}
