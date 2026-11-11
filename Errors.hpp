#pragma once
#include <stdexcept>
#include <string>

class FleetError : public std::runtime_error {
 public:
  explicit FleetError(const std::string& message)
      : std::runtime_error(message) {}
};
class ArgumentError : public FleetError {
 public:
  explicit ArgumentError(const std::string& message) : FleetError(message) {}
};
class InspectionError : public FleetError {
 public:
  InspectionError() : FleetError("inspection rejected") {}
};
