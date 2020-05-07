//
// Created by jason on 5/7/20.
//

#ifndef MYPROJECT_INPUT_HPP
#define MYPROJECT_INPUT_HPP

#include <SFML/Window/Joystick.hpp>
#include <string>
#include <array>


namespace Game
{

struct Joystick
{
  unsigned int id;
  std::string name;
  unsigned int buttonCount;
  std::array<bool, sf::Joystick::ButtonCount> buttonState;
  std::array<float, sf::Joystick::AxisCount> axisPosition;
};


Joystick loadJoystick(unsigned int id)
{
  const auto identification = sf::Joystick::getIdentification(id);
  Joystick js{ id, static_cast<std::string>(identification.name), sf::Joystick::getButtonCount(id), {}, {} };

  for (unsigned int button = 0; button < js.buttonCount; ++button) {
    js.buttonState[button] = sf::Joystick::isButtonPressed(id, button);
  }

  for (unsigned int axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
    js.axisPosition[axis] = sf::Joystick::getAxisPosition(id, static_cast<sf::Joystick::Axis>(axis));
  }

  return js;
}

constexpr std::string_view toString(const sf::Joystick::Axis axis)
{
  switch (axis) {
  case sf::Joystick::Axis::PovX:
    return "PovX";
  case sf::Joystick::Axis::PovY:
    return "PovY";
  case sf::Joystick::Axis::R:
    return "R";
  case sf::Joystick::Axis::U:
    return "U";
  case sf::Joystick::Axis::V:
    return "V";
  case sf::Joystick::Axis::X:
    return "X";
  case sf::Joystick::Axis::Y:
    return "Y";
  case sf::Joystick::Axis::Z:
    return "Z";
  }
  abort();
}


Joystick &joystickById(std::vector<Joystick> &joysticks, unsigned int id)
{
  auto joystick = std::find_if(begin(joysticks), end(joysticks), [id](const auto &j) { return j.id == id; });

  if (joystick == joysticks.end()) {
    joysticks.push_back(loadJoystick(id));
    return joysticks.back();
  } else {
    return *joystick;
  }
}
}

#endif// MYPROJECT_INPUT_HPP
