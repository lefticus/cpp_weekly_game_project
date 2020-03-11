#include <iostream>

#include <spdlog/spdlog.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>


#include <docopt/docopt.h>

/*
static constexpr auto USAGE =
  R"(Naval Fate.

    Usage:
          naval_fate ship new <name>...
          naval_fate ship <name> move <x> <y> [--speed=<kn>]
          naval_fate ship shoot <x> <y>
          naval_fate mine (set|remove) <x> <y> [--moored | --drifting]
          naval_fate (-h | --help)
          naval_fate --version
 Options:
          -h --help     Show this screen.
          --version     Show version.
          --speed=<kn>  Speed in knots [default: 10].
          --moored      Moored (anchored) mine.
          --drifting    Drifting mine.
)";
*/

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
  //  std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
  //    { std::next(argv), std::next(argv, argc) },
  //    true,// show help if requested
  //    "Naval Fate 2.0");// version string

  //  for (auto const &arg : args) {
  //    std::cout << arg.first << arg.second << std::endl;
  //  }


  //Use the default logger (stdout, multi-threaded, colored)
  spdlog::info("Hello, {}!", "World");


  sf::RenderWindow window(sf::VideoMode(1024, 768), "ImGui + SFML = <3");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);

  constexpr auto scale_factor = 2.0;
  ImGui::GetStyle().ScaleAllSizes(scale_factor);
  ImGui::GetIO().FontGlobalScale = scale_factor;

  std::array<bool, 13> states{};

  sf::Clock deltaClock;
  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);

      if (event.type == sf::Event::Closed) {
        window.close();
      }
    }

    ImGui::SFML::Update(window, deltaClock.restart());


    ImGui::Begin("The Plan");

    int index = 0;
    for (const auto &step : { "The Plan", "Getting Started", "Finding Errors As Soon As Possible", "Handling Command Line Parameters", "C++ 20 So Far", "Reading SFML Input States", "Managing Game State", "Making Our Game Testable", "Making Game State Allocator Aware", "Add Logging To Game Engine", "Draw A Game Map", "Dialog Trees", "Porting From SFML To SDL" }) {
      ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), std::next(begin(states), index));
      ++index;
    }


    ImGui::End();

    window.clear();
    ImGui::SFML::Render(window);
    window.display();
  }

  ImGui::SFML::Shutdown();

  return 0;
}
