#pragma once
#include <memory>
#include <string>
#include <utility>

#include "Inventory.hpp"
#include "ModelSpec.hpp"
#include "Validation.hpp"

enum class MaintenanceState { Available, InRepair };

class Transport : public InventoryItem {
  friend class Fleet;

 private:
  std::string id_;
  std::string name_;
  int beginnerScore_;
  std::shared_ptr<const ModelSpec> model_;
  MaintenanceState maintenanceState_{MaintenanceState::Available};
  std::string repairReason_;

  void startRepair(std::string reason) {
    if (maintenanceState_ != MaintenanceState::Available) {
      throw ArgumentError("transport is already in repair");
    }
    if (reason.empty()) {
      throw ArgumentError("repair reason must not be empty");
    }
    repairReason_ = std::move(reason);
    maintenanceState_ = MaintenanceState::InRepair;
  }

  void finishRepair() {
    if (maintenanceState_ != MaintenanceState::InRepair) {
      throw ArgumentError("transport is not in repair");
    }
    repairReason_.clear();
    maintenanceState_ = MaintenanceState::Available;
  }

 public:
  Transport(std::string id, std::string name, int beginnerScore,
            std::shared_ptr<const ModelSpec> model)
      : id_{std::move(id)},
        name_{std::move(name)},
        beginnerScore_{beginnerScore},
        model_{std::move(model)} {
    validateId(id_);
    validateName(name_);
    validateScore(beginnerScore_);
    if (!model_) throw ArgumentError("model is empty");
  }
  std::string InventoryId() const override { return id_; }
  std::string name() const override { return name_; }
  virtual int beginnerScore() const { return beginnerScore_; }
  const ModelSpec& modelSpec() const { return *model_; }
  bool isAvailable() const {
    return maintenanceState_ == MaintenanceState::Available;
  }
  MaintenanceState maintenanceState() const { return maintenanceState_; }
  const std::string& repairReason() const { return repairReason_; }
};
