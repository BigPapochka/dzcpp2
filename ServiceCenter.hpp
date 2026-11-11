#pragma once
#include <cctype>
#include <string>

#include "Errors.hpp"
#include "Transport.hpp"

class ServiceCenter {
 public:
  void inspect(const Transport& transport) const {
    std::string lowered = transport.name();
    for (char& symbol : lowered) {
      symbol =
          static_cast<char>(std::tolower(static_cast<unsigned char>(symbol)));
    }
    if (lowered.find("broken") != std::string::npos) {
      throw InspectionError{};
    }
  }
};
