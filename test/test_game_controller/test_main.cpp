#include <unity.h>

#include "GameController.h"

namespace {

using game::GameConfig;
using game::GameController;
using game::GameInputs;
using game::GameState;

GameController makeController() {
  GameConfig config;
  config.roundDurationMs = 60000UL;
  config.warningAtMs = 40000UL;
  config.blinkIntervalMs = 100UL;
  config.blinkToggleCount = 6;
  return GameController(config);
}

GameInputs makeInputs(uint32_t nowMs) {
  GameInputs inputs;
  inputs.nowMs = nowMs;
  return inputs;
}

void releaseAll(GameController &controller, uint32_t nowMs) {
  controller.process(makeInputs(nowMs));
}

}  // namespace

void test_starts_round_on_master_press() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(10);
  inputs.masterPressed = true;
  controller.process(inputs);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::RUNNING),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_FALSE(controller.outputs().warningLedOn);
  TEST_ASSERT_EQUAL_INT(-1, controller.outputs().activeTeamIndex);
}

void test_manual_stop_resets_outputs() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(10);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 20);

  inputs = makeInputs(30);
  inputs.masterPressed = true;
  controller.process(inputs);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::STOP),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_FALSE(controller.outputs().warningLedOn);
  TEST_ASSERT_EQUAL_INT(-1, controller.outputs().activeTeamIndex);
  for (bool led : controller.outputs().teamLeds) {
    TEST_ASSERT_FALSE(led);
  }
}

void test_first_team_locks_round() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(10);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 20);

  inputs = makeInputs(30);
  inputs.teamPressed[1] = true;
  controller.process(inputs);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::LOCKED_BY_TEAM),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_EQUAL_INT(1, controller.outputs().activeTeamIndex);
  TEST_ASSERT_TRUE(controller.outputs().teamLeds[1]);
}

void test_second_team_is_ignored_after_lock() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(10);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 20);

  inputs = makeInputs(30);
  inputs.teamPressed[0] = true;
  controller.process(inputs);
  releaseAll(controller, 40);

  inputs = makeInputs(50);
  inputs.teamPressed[2] = true;
  controller.process(inputs);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::LOCKED_BY_TEAM),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_EQUAL_INT(0, controller.outputs().activeTeamIndex);
  TEST_ASSERT_TRUE(controller.outputs().teamLeds[0]);
  TEST_ASSERT_FALSE(controller.outputs().teamLeds[2]);
}

void test_warning_turns_on_at_configured_time() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(0);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 1);

  controller.process(makeInputs(39999));
  TEST_ASSERT_FALSE(controller.outputs().warningLedOn);

  controller.process(makeInputs(40000));
  TEST_ASSERT_TRUE(controller.outputs().warningLedOn);
}

void test_round_auto_stops_at_timeout() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(0);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 1);

  controller.process(makeInputs(60000));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::STOP),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_FALSE(controller.outputs().warningLedOn);
}

void test_stop_button_press_only_blinks() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(5);
  inputs.teamPressed[2] = true;
  controller.process(inputs);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::STOP),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_TRUE(controller.outputs().teamLeds[2]);
  TEST_ASSERT_EQUAL_INT(-1, controller.outputs().activeTeamIndex);
}

void test_sequential_rounds_reset_cleanly() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(0);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 1);

  inputs = makeInputs(10);
  inputs.teamPressed[3] = true;
  controller.process(inputs);
  releaseAll(controller, 11);

  inputs = makeInputs(20);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 21);

  inputs = makeInputs(30);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 31);

  inputs = makeInputs(40);
  inputs.teamPressed[1] = true;
  controller.process(inputs);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::LOCKED_BY_TEAM),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_EQUAL_INT(1, controller.outputs().activeTeamIndex);
  TEST_ASSERT_TRUE(controller.outputs().teamLeds[1]);
  TEST_ASSERT_FALSE(controller.outputs().teamLeds[3]);
}

void test_holding_button_does_not_retrigger_lock() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(0);
  inputs.masterPressed = true;
  controller.process(inputs);
  releaseAll(controller, 1);

  inputs = makeInputs(10);
  inputs.teamPressed[0] = true;
  controller.process(inputs);

  inputs = makeInputs(20);
  inputs.teamPressed[0] = true;
  controller.process(inputs);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::LOCKED_BY_TEAM),
                        static_cast<int>(controller.outputs().state));
  TEST_ASSERT_EQUAL_INT(0, controller.outputs().activeTeamIndex);
  TEST_ASSERT_TRUE(controller.outputs().teamLeds[0]);
}

void test_blink_finishes_after_configured_toggles() {
  GameController controller = makeController();

  GameInputs inputs = makeInputs(0);
  inputs.teamPressed[1] = true;
  controller.process(inputs);

  for (uint32_t nowMs = 100; nowMs <= 600; nowMs += 100) {
    controller.process(makeInputs(nowMs));
  }

  TEST_ASSERT_FALSE(controller.outputs().teamLeds[1]);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(GameState::STOP),
                        static_cast<int>(controller.outputs().state));
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_starts_round_on_master_press);
  RUN_TEST(test_manual_stop_resets_outputs);
  RUN_TEST(test_first_team_locks_round);
  RUN_TEST(test_second_team_is_ignored_after_lock);
  RUN_TEST(test_warning_turns_on_at_configured_time);
  RUN_TEST(test_round_auto_stops_at_timeout);
  RUN_TEST(test_stop_button_press_only_blinks);
  RUN_TEST(test_sequential_rounds_reset_cleanly);
  RUN_TEST(test_holding_button_does_not_retrigger_lock);
  RUN_TEST(test_blink_finishes_after_configured_toggles);
  return UNITY_END();
}
