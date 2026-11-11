#pragma once
#include <cstddef>
#include <iomanip>
#include <memory>
#include <ostream>
#include <string>
#include <utility>

#include "Errors.hpp"
#include "FocusCard.hpp"
#include "Inventory.hpp"
#include "Registry.hpp"
#include "Reports.hpp"
#include "ServiceCenter.hpp"
#include "Transport.hpp"
#include "Validation.hpp"

class Fleet {
 private:
  Registry<std::unique_ptr<InventoryItem>> items_;
  ServiceCenter service_;
  std::weak_ptr<const FocusCard> focus_;

  InventoryItem& find(const std::string& id) {
    validateId(id);
    for (std::size_t i = 0; i < items_.size(); ++i) {
      if (items_[i]->InventoryId() == id) return *items_[i];
    }
    throw ArgumentError("item not found");
  }

  const InventoryItem& find(const std::string& id) const {
    validateId(id);
    for (std::size_t i = 0; i < items_.size(); ++i) {
      if (items_[i]->InventoryId() == id) return *items_[i];
    }
    throw ArgumentError("item not found");
  }

  Transport& findTransport(const std::string& id) {
    auto* transport = dynamic_cast<Transport*>(&find(id));
    if (!transport) throw ArgumentError("item is not transport");
    return *transport;
  }

  const Transport& findTransport(const std::string& id) const {
    const auto* transport = dynamic_cast<const Transport*>(&find(id));
    if (!transport) throw ArgumentError("item is not transport");
    return *transport;
  }

 public:
  Fleet() = default;
  Fleet(const Fleet&) = delete;
  Fleet& operator=(const Fleet&) = delete;

  void add(std::unique_ptr<InventoryItem> item) {
    if (!item) throw ArgumentError("item is empty");
    validateId(item->InventoryId());
    for (std::size_t i = 0; i < items_.size(); ++i) {
      if (*items_[i] == *item) {
        throw ArgumentError("duplicate inventory id");
      }
    }
    const auto* transport = dynamic_cast<const Transport*>(item.get());
    if (transport) service_.inspect(*transport);
    items_.add(std::move(item));
  }

  std::size_t size() const { return items_.size(); }

  void list(std::ostream& out) const {
    Registry<const InventoryItem*> sorted;
    for (std::size_t i = 0; i < items_.size(); ++i) {
      sorted.add(items_[i].get());
    }
    sorted.sort([](const InventoryItem* left, const InventoryItem* right) {
      return *left < *right;
    });
    out << sorted;
  }

  double totalEnergy() const { return sumDailyEnergy(items_); }

  void novice(std::ostream& out, int minimumScore = 6) const {
    validateScore(minimumScore);
    for (std::size_t i = 0; i < items_.size(); ++i) {
      const auto* transport = dynamic_cast<const Transport*>(items_[i].get());
      if (transport && transport->isAvailable() &&
          transport->beginnerScore() >= minimumScore) {
        out << *transport << '\n';
      }
    }
  }

  std::size_t countNovice(int minimumScore = 6) const {
    validateScore(minimumScore);
    std::size_t count = 0;
    for (std::size_t i = 0; i < items_.size(); ++i) {
      const auto* transport = dynamic_cast<const Transport*>(items_[i].get());
      if (transport && transport->isAvailable() &&
          transport->beginnerScore() >= minimumScore)
        ++count;
    }
    return count;
  }

  void remove(const std::string& id) {
    validateId(id);
    for (std::size_t i = 0; i < items_.size(); ++i) {
      if (items_[i]->InventoryId() == id) {
        items_.erase(i);
        return;
      }
    }
    throw ArgumentError("item not found");
  }

  void focus(const std::string& id) {
    validateId(id);
    for (std::size_t i = 0; i < items_.size(); ++i) {
      if (items_[i]->InventoryId() == id) {
        focus_ = items_[i]->focusCard();
        return;
      }
    }
    throw ArgumentError("item not found");
  }

  void showFocus(std::ostream& out) const {
    const auto card = focus_.lock();
    if (!card || !card->alive)
      out << "removed / expired\n";
    else
      out << card->description << '\n';
  }

  void showModel(const std::string& id, std::ostream& out) const {
    const auto* transport = dynamic_cast<const Transport*>(&find(id));
    if (!transport) throw ArgumentError("item has no model specification");
    const auto& model = transport->modelSpec();
    out << std::quoted(model.modelname) << ' '
        << std::quoted(model.manufacturer) << '\n';
  }

  void repair(const std::string& id, std::string reason = "maintenance") {
    findTransport(id).startRepair(std::move(reason));
  }

  void restore(const std::string& id) {
    Transport& transport = findTransport(id);
    if (transport.isAvailable()) {
      throw ArgumentError("transport is not in repair");
    }
    service_.inspect(transport);
    transport.finishRepair();
  }

  void status(const std::string& id, std::ostream& out) const {
    out << (findTransport(id).isAvailable() ? "available" : "in_repair")
        << '\n';
  }

  void repairs(std::ostream& out) const {
    Registry<const Transport*> sorted;
    for (std::size_t i = 0; i < items_.size(); ++i) {
      const auto* transport = dynamic_cast<const Transport*>(items_[i].get());
      if (transport && !transport->isAvailable()) sorted.add(transport);
    }
    sorted.sort([](const Transport* left, const Transport* right) {
      return *left < *right;
    });
    for (std::size_t i = 0; i < sorted.size(); ++i) {
      out << *sorted[i] << " | " << sorted[i]->repairReason() << '\n';
    }
  }
};
