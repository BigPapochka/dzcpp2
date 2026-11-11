#include <iomanip>
#include <iostream>
#include <locale>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "Bicycle.hpp"
#include "Dock.hpp"
#include "ElectricBike.hpp"
#include "ElectricScooter.hpp"
#include "Fleet.hpp"
#include "Helmet.hpp"
#include "ModelSpec.hpp"
#include "Skateboard.hpp"

namespace {
using ModelKey = std::pair<std::string, std::string>;
using ModelMap = std::map<ModelKey, std::shared_ptr<const ModelSpec>>;

std::string readText(std::istringstream& input, const std::string& field) {
  std::string value;
  if (!(input >> std::quoted(value))) {
    throw ArgumentError("missing or invalid " + field);
  }
  return value;
}

template <typename T>
T readNumber(std::istringstream& input, const std::string& field) {
  std::string token;
  if (!(input >> token)) throw ArgumentError("missing " + field);
  std::istringstream number(token);
  number.imbue(std::locale::classic());
  T value{};
  if (!(number >> value) || number.peek() != std::char_traits<char>::eof()) {
    throw ArgumentError("invalid " + field);
  }
  return value;
}

void requireEnd(std::istringstream& input) {
  std::string extra;
  if (input >> extra) throw ArgumentError("unexpected arguments");
}

int readMinimumScore(std::istringstream& input) {
  input >> std::ws;
  if (input.peek() == std::char_traits<char>::eof()) return 6;
  const int score = readNumber<int>(input, "minimum score");
  validateScore(score);
  requireEnd(input);
  return score;
}

std::shared_ptr<const ModelSpec> getModel(ModelMap& models,
                                          const std::string& type,
                                          const std::string& name) {
  const ModelKey key{type, name};
  const auto found = models.find(key);
  if (found != models.end()) return found->second;
  auto model = std::make_shared<const ModelSpec>(ModelSpec{name, "Unknown"});
  models.emplace(key, model);
  return model;
}

void addItem(Fleet& fleet, ModelMap& models, std::istringstream& input) {
  const std::string type = readText(input, "item type");
  const bool isTransport = type == "bike" || type == "scooter" ||
                           type == "ebike" || type == "skateboard";
  if (!isTransport && type != "helmet" && type != "dock") {
    throw ArgumentError("unknown item type");
  }
  const std::string id = readText(input, "id");
  const std::string name = readText(input, "name");
  double energy = 0.0;
  int score = 1;
  if (type == "scooter" || type == "ebike" || type == "dock") {
    energy = readNumber<double>(input, "energy");
  }
  if (isTransport) score = readNumber<int>(input, "score");
  requireEnd(input);

  if (type == "helmet") {
    fleet.add(std::make_unique<Helmet>(id, name));
  } else if (type == "dock") {
    fleet.add(std::make_unique<Dock>(id, name, energy));
  } else {
    const auto model = getModel(models, type, name);
    if (type == "bike") {
      fleet.add(std::make_unique<Bicycle>(id, name, score, model));
    } else if (type == "scooter") {
      fleet.add(
          std::make_unique<ElectricScooter>(id, name, energy, score, model));
    } else if (type == "ebike") {
      fleet.add(std::make_unique<ElectricBike>(id, name, energy, score, model));
    } else {
      fleet.add(std::make_unique<Skateboard>(id, name, score, model));
    }
  }
}

bool execute(Fleet& fleet, ModelMap& models, std::istringstream& input,
             const std::string& command, std::ostream& out) {
  if (command == "add") {
    addItem(fleet, models, input);
  } else if (command == "list") {
    requireEnd(input);
    fleet.list(out);
  } else if (command == "energy") {
    requireEnd(input);
    const double energy = fleet.totalEnergy();
    out << std::fixed << std::setprecision(1) << energy << '\n';
  } else if (command == "novice") {
    fleet.novice(out, readMinimumScore(input));
  } else if (command == "count_novice") {
    out << fleet.countNovice(readMinimumScore(input)) << '\n';
  } else if (command == "count") {
    requireEnd(input);
    out << fleet.size() << '\n';
  } else if (command == "report") {
    requireEnd(input);
    const double energy = fleet.totalEnergy();
    out << "count: " << fleet.size() << '\n'
        << "energy: " << std::fixed << std::setprecision(1) << energy << '\n';
  } else if (command == "repair") {
    const std::string id = readText(input, "id");
    input >> std::ws;
    std::string reason = "maintenance";
    if (input.peek() != std::char_traits<char>::eof()) {
      reason = readText(input, "repair reason");
    }
    requireEnd(input);
    fleet.repair(id, std::move(reason));
  } else if (command == "repairs") {
    requireEnd(input);
    fleet.repairs(out);
  } else if (command == "remove" || command == "focus" || command == "model" ||
             command == "restore" || command == "status") {
    const std::string id = readText(input, "id");
    requireEnd(input);
    if (command == "remove")
      fleet.remove(id);
    else if (command == "focus")
      fleet.focus(id);
    else if (command == "restore")
      fleet.restore(id);
    else if (command == "status")
      fleet.status(id, out);
    else
      fleet.showModel(id, out);
  } else if (command == "show_focus") {
    requireEnd(input);
    fleet.showFocus(out);
  } else if (command == "quit") {
    requireEnd(input);
    return false;
  } else {
    throw ArgumentError("unknown command");
  }
  return true;
}
}

int main() {
  Fleet fleet;
  ModelMap models;
  std::cout.imbue(std::locale::classic());
  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream input(line);
    input.imbue(std::locale::classic());
    std::string command;
    if (!(input >> command)) continue;
    try {
      if (!execute(fleet, models, input, command, std::cout)) break;
    } catch (const FleetError& error) {
      std::cout << "error: " << error.what() << '\n';
    } catch (const std::exception& error) {
      std::cout << "error: " << error.what() << '\n';
    }
  }
  return 0;
}
