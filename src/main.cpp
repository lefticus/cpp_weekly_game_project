#include <iostream>
#include <array>

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include <docopt/docopt.h>

#include "Input.hpp"
#include "ImGuiHelpers.hpp"
#include "Utility.hpp"


static constexpr auto USAGE =
  R"(C++ Weekly Game.
    Usage:
          game [options]

  Options:
          -h --help         Show this screen.
          --width=WIDTH     Screen width in pixels [default: 1024].
          --height=HEIGHT   Screen height in pixels [default: 768].
          --scale=SCALE     Scaling factor [default: 2].
)";


int main(int argc, const char **argv)
{
  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
    { std::next(argv), std::next(argv, argc) },
    true,// show help if requested
    "Game 0.0");// version string

  const auto width = args["--width"].asLong();
  const auto height = args["--height"].asLong();
  const auto scale = args["--scale"].asLong();

  if (width < 0 || height < 0 || scale < 1 || scale > 5) {
    spdlog::error("Command line options are out of reasonable range.");
    for (auto const &arg : args) {
      if (arg.second.isString()) { spdlog::info("Parameter set: {}='{}'", arg.first, arg.second.asString()); }
    }
    abort();
  }


  spdlog::set_level(spdlog::level::debug);
  // Use the default logger (stdout, multi-threaded, colored)
  spdlog::info("Hello, {}!", "World");


  sf::RenderWindow window(
    sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)), "ImGui + SFML = <3");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);

  const auto scale_factor = static_cast<float>(scale);
  ImGui::GetStyle().ScaleAllSizes(scale_factor);
  ImGui::GetIO().FontGlobalScale = scale_factor;

  constexpr std::array steps = { "The Plan",
    "Getting Started",
    "Finding Errors As Soon As Possible",
    "Handling Command Line Parameters",
    "Reading SFML Joystick States",
    "Displaying Joystick States",
    "Dealing With Game Events",
    "Reading SFML Keyboard States",
    "Reading SFML Mouse States",
    "Reading SFML Touchscreen States",
    "C++ 20 So Far",
    "Managing Game State",
    "Making Our Game Testable",
    "Making Game State Allocator Aware",
    "Add Logging To Game Engine",
    "Draw A Game Map",
    "Dialog Trees",
    "Porting From SFML To SDL" };

  std::array<bool, steps.size()> states{};

  Game::GameState gs;

  while (window.isOpen()) {

    const auto event = gs.nextEvent(window);

    if (const auto sfmlEvent = Game::GameState::toSFMLEvent(event); sfmlEvent) {
      ImGui::SFML::ProcessEvent(*sfmlEvent);
    }

    bool joystickEvent = false;

    bool timeElapsed = false;

    std::visit(Game::overloaded{
                 [&](const Game::JoystickEvent auto &jsEvent)
                 {
                   gs.update(jsEvent);
                   joystickEvent = true;
                 },
                 [&](const Game::GameState::CloseWindow & /*unused*/) { window.close(); },
                 [&](const Game::GameState::TimeElapsed &te) {
                   ImGui::SFML::Update(window, te.toSFMLTime());
                   timeElapsed = true;
                 },
                 [&](const std::monostate & /*unused*/) {

                 },
                 [&](const auto & /*do nothing*/) {}

               },
      event);


    if (!timeElapsed) {
      // todo: something more with a linear flow here
      // right now this is just saying "no reason to update the render yet"
      continue;
    }


    ImGui::Begin("The Plan");

    for (std::size_t index = 0; const auto &step : steps) {
      ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), &states.at(index));
      ++index;
    }

    ImGui::End();

    ImGui::Begin("Joystick");

    if (!gs.joySticks.empty()) {
      ImGuiHelper::Text("Joystick Event: {}", joystickEvent);
      for (std::size_t button = 0; button < gs.joySticks[0].buttonCount; ++button) {
        ImGuiHelper::Text("{}: {}", button, gs.joySticks[0].buttonState[button]);
      }

      for (std::size_t axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
        ImGuiHelper::Text(
          "{}: {}", Game::toString(static_cast<sf::Joystick::Axis>(axis)), gs.joySticks[0].axisPosition[axis]);
      }
    }

    ImGui::End();

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();

  return 0;
}
