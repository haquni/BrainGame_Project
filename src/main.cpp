#include <Arduino.h>

#include "GameController.h"

namespace {

constexpr uint8_t kMasterButtonPin = 8;
constexpr uint8_t kWarningLedPin = 7;
constexpr uint8_t kTeamButtonPins[game::kTeamCount] = {9, 10, 11, 12};
constexpr uint8_t kTeamLedPins[game::kTeamCount] = {2, 3, 4, 5};

game::GameController controller;

void applyOutputs(const game::GameOutputs &outputs) {
  digitalWrite(kWarningLedPin, outputs.warningLedOn ? HIGH : LOW);

  for (size_t i = 0; i < game::kTeamCount; ++i) {
    digitalWrite(kTeamLedPins[i], outputs.teamLeds[i] ? HIGH : LOW);
  }
}

}  // namespace

void setup() {
  for (size_t i = 0; i < game::kTeamCount; ++i) {
    pinMode(kTeamLedPins[i], OUTPUT);
    pinMode(kTeamButtonPins[i], INPUT);
  }

  pinMode(kMasterButtonPin, INPUT);
  pinMode(kWarningLedPin, OUTPUT);

  controller.reset();
  applyOutputs(controller.outputs());
}

void loop() {
  game::GameInputs inputs = {};
  inputs.masterPressed = digitalRead(kMasterButtonPin) == HIGH;
  inputs.nowMs = millis();

  for (size_t i = 0; i < game::kTeamCount; ++i) {
    inputs.teamPressed[i] = digitalRead(kTeamButtonPins[i]) == HIGH;
  }

  controller.process(inputs);
  applyOutputs(controller.outputs());
}
