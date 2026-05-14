#pragma once

#include <stddef.h>
#include <stdint.h>

namespace game {

constexpr size_t kTeamCount = 4;

enum class GameState : uint8_t {
  STOP,
  RUNNING,
  LOCKED_BY_TEAM,
};

struct GameConfig {
  uint32_t roundDurationMs = 60000UL;
  uint32_t warningAtMs = 40000UL;
  uint32_t blinkIntervalMs = 333UL;
  uint8_t blinkToggleCount = 6;
};

struct GameInputs {
  bool masterPressed = false;
  bool teamPressed[kTeamCount] = {false, false, false, false};
  uint32_t nowMs = 0;
};

struct GameOutputs {
  GameState state = GameState::STOP;
  int8_t activeTeamIndex = -1;
  bool warningLedOn = false;
  bool teamLeds[kTeamCount] = {false, false, false, false};
};

class GameController {
 public:
  explicit GameController(GameConfig config = {});

  void reset();
  void process(const GameInputs &inputs);

  const GameOutputs &outputs() const;
  const GameConfig &config() const;

 private:
  struct BlinkState {
    bool active = false;
    bool ledOn = false;
    uint8_t togglesRemaining = 0;
    uint32_t lastToggleMs = 0;
  };

  void handleMasterButton(const GameInputs &inputs);
  void handleTeamButtons(const GameInputs &inputs);
  void updateRoundTimer(const GameInputs &inputs);
  void updateStopBlinking(const GameInputs &inputs);

  void startRound(uint32_t nowMs);
  void lockTeam(size_t teamIndex);
  void resetToStop();
  void startStopBlink(size_t teamIndex, uint32_t nowMs);
  void clearBlinkStates();
  void clearTeamLeds();

  GameConfig config_;
  GameOutputs outputs_;
  uint32_t roundStartMs_ = 0;
  bool warningTriggered_ = false;
  bool previousMasterPressed_ = false;
  bool previousTeamPressed_[kTeamCount] = {false, false, false, false};
  BlinkState blinkStates_[kTeamCount] = {};
};

}  // namespace game
