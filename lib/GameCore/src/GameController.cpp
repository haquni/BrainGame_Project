#include "GameController.h"

namespace game {

GameController::GameController(GameConfig config) : config_(config) {
  reset();
}

void GameController::reset() {
  previousMasterPressed_ = false;
  for (size_t i = 0; i < kTeamCount; ++i) {
    previousTeamPressed_[i] = false;
  }
  resetToStop();
}

void GameController::process(const GameInputs &inputs) {
  handleMasterButton(inputs);
  handleTeamButtons(inputs);
  updateRoundTimer(inputs);
  updateStopBlinking(inputs);
}

const GameOutputs &GameController::outputs() const {
  return outputs_;
}

const GameConfig &GameController::config() const {
  return config_;
}

void GameController::handleMasterButton(const GameInputs &inputs) {
  const bool risingEdge = inputs.masterPressed && !previousMasterPressed_;
  previousMasterPressed_ = inputs.masterPressed;

  if (!risingEdge) {
    return;
  }

  if (outputs_.state == GameState::STOP) {
    startRound(inputs.nowMs);
    return;
  }

  resetToStop();
}

void GameController::handleTeamButtons(const GameInputs &inputs) {
  for (size_t i = 0; i < kTeamCount; ++i) {
    const bool risingEdge = inputs.teamPressed[i] && !previousTeamPressed_[i];
    previousTeamPressed_[i] = inputs.teamPressed[i];

    if (!risingEdge) {
      continue;
    }

    if (outputs_.state == GameState::STOP) {
      startStopBlink(i, inputs.nowMs);
      continue;
    }

    if (outputs_.state == GameState::RUNNING && outputs_.activeTeamIndex < 0) {
      lockTeam(i);
    }
  }
}

void GameController::updateRoundTimer(const GameInputs &inputs) {
  if (outputs_.state != GameState::RUNNING) {
    return;
  }

  const uint32_t elapsedMs = inputs.nowMs - roundStartMs_;

  if (!warningTriggered_ && elapsedMs >= config_.warningAtMs) {
    warningTriggered_ = true;
    outputs_.warningLedOn = true;
  }

  if (elapsedMs >= config_.roundDurationMs) {
    resetToStop();
  }
}

void GameController::updateStopBlinking(const GameInputs &inputs) {
  if (outputs_.state != GameState::STOP) {
    return;
  }

  for (size_t i = 0; i < kTeamCount; ++i) {
    BlinkState &blink = blinkStates_[i];
    if (!blink.active || (inputs.nowMs - blink.lastToggleMs) < config_.blinkIntervalMs) {
      continue;
    }

    blink.lastToggleMs = inputs.nowMs;
    blink.ledOn = !blink.ledOn;
    outputs_.teamLeds[i] = blink.ledOn;

    if (blink.togglesRemaining > 0) {
      --blink.togglesRemaining;
    }

    if (blink.togglesRemaining == 0) {
      blink.active = false;
      blink.ledOn = false;
      outputs_.teamLeds[i] = false;
    }
  }
}

void GameController::startRound(uint32_t nowMs) {
  outputs_.state = GameState::RUNNING;
  outputs_.activeTeamIndex = -1;
  outputs_.warningLedOn = false;
  roundStartMs_ = nowMs;
  warningTriggered_ = false;
  clearBlinkStates();
  clearTeamLeds();
}

void GameController::lockTeam(size_t teamIndex) {
  outputs_.state = GameState::LOCKED_BY_TEAM;
  outputs_.activeTeamIndex = static_cast<int8_t>(teamIndex);
  outputs_.warningLedOn = false;
  clearTeamLeds();
  outputs_.teamLeds[teamIndex] = true;
}

void GameController::resetToStop() {
  outputs_.state = GameState::STOP;
  outputs_.activeTeamIndex = -1;
  outputs_.warningLedOn = false;
  roundStartMs_ = 0;
  warningTriggered_ = false;
  clearBlinkStates();
  clearTeamLeds();
}

void GameController::startStopBlink(size_t teamIndex, uint32_t nowMs) {
  BlinkState &blink = blinkStates_[teamIndex];
  blink.active = true;
  blink.ledOn = true;
  blink.togglesRemaining = config_.blinkToggleCount;
  blink.lastToggleMs = nowMs;
  outputs_.teamLeds[teamIndex] = true;
}

void GameController::clearBlinkStates() {
  for (size_t i = 0; i < kTeamCount; ++i) {
    blinkStates_[i] = {};
  }
}

void GameController::clearTeamLeds() {
  for (size_t i = 0; i < kTeamCount; ++i) {
    outputs_.teamLeds[i] = false;
  }
}

}  // namespace game
