//
// Created by jason on 5/7/20.
//

#ifndef MYPROJECT_INPUT_HPP
#define MYPROJECT_INPUT_HPP

#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <string>
#include <thread>
#include <variant>

#include "Utility.hpp"

namespace Game {

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

struct GameState
{

  using clock = std::chrono::steady_clock;
  clock::time_point lastTick{ clock::now() };

  template<typename Source> struct Pressed
  {
    constexpr static std::string_view name{ "Pressed" };
    constexpr static std::array       elements{ std::string_view{ "source" } };
    Source                            source;
  };

  template<typename Source> struct Released
  {
    constexpr static std::string_view name{ "Released" };
    constexpr static std::array       elements{ std::string_view{ "source" } };
    Source                            source;
  };

  template<typename Source> struct Moved
  {
    constexpr static std::string_view name{ "Moved" };
    constexpr static std::array       elements{ std::string_view{ "source" } };
    Source                            source;
  };

  struct JoystickButton
  {
    constexpr static std::string_view name{ "JoystickButton" };
    constexpr static auto             elements = std::to_array<std::string_view>({ "id", "button" });
    unsigned int                      id;
    unsigned int                      button;
  };

  struct JoystickAxis
  {
    constexpr static std::string_view name{ "JoystickAxis" };
    constexpr static auto             elements = std::to_array<std::string_view>({ "id", "axis", "position" });
    unsigned int                      id;
    unsigned int                      axis;
    float                             position;
  };

  struct Mouse
  {
    constexpr static std::string_view name{ "Mouse" };
    constexpr static auto             elements = std::to_array<std::string_view>({ "x", "y" });
    int                               x;
    int                               y;
  };

  struct MouseButton
  {
    constexpr static std::string_view name{ "MouseButton" };
    constexpr static auto             elements = std::to_array<std::string_view>({ "button", "mouse" });
    int                               button;
    Mouse                             mouse;
  };

  struct Key
  {
    constexpr static std::string_view name{ "Key" };
    constexpr static auto elements = std::to_array<std::string_view>({ "alt", "control", "system", "shift", "key" });
    bool                  alt;
    bool                  control;
    bool                  system;
    bool                  shift;
    sf::Keyboard::Key     key;
  };

  struct CloseWindow
  {
    constexpr static std::string_view                name{ "CloseWindow" };
    constexpr static std::array<std::string_view, 0> elements{};
  };

  struct TimeElapsed
  {
    constexpr static std::string_view name{ "TimeElapsed" };
    constexpr static auto             elements = std::to_array<std::string_view>({ "elapsed" });
    clock::duration                   elapsed;

    [[nodiscard]] sf::Time toSFMLTime() const
    {
      return sf::microseconds(duration_cast<std::chrono::microseconds>(elapsed).count());
    }
  };


  struct Joystick
  {
    unsigned int                                id;
    unsigned int                                buttonCount;
    std::array<bool, sf::Joystick::ButtonCount> buttonState;
    std::array<float, sf::Joystick::AxisCount>  axisPosition;
  };

  void update(const Pressed<JoystickButton> &button)
  {
    auto &js                             = joystickById(joySticks, button.source.id);
    js.buttonState[button.source.button] = true;
  }

  void update(const Released<JoystickButton> &button)
  {
    auto &js                             = joystickById(joySticks, button.source.id);
    js.buttonState[button.source.button] = false;
  }

  void update(const Moved<JoystickAxis> &button)
  {
    auto &js                            = joystickById(joySticks, button.source.id);
    js.axisPosition[button.source.axis] = button.source.position;
  }

  std::vector<Joystick> joySticks;

  static void refreshJoystick(Joystick &js)
  {
    sf::Joystick::update();

    for (unsigned int button = 0; button < js.buttonCount; ++button) {
      js.buttonState[button] = sf::Joystick::isButtonPressed(js.id, button);
    }

    for (unsigned int axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
      js.axisPosition[axis] = sf::Joystick::getAxisPosition(js.id, static_cast<sf::Joystick::Axis>(axis));
    }
  }

  static Joystick loadJoystick(unsigned int id)
  {
    const auto identification = sf::Joystick::getIdentification(id);
    Joystick   js{ id, sf::Joystick::getButtonCount(id), {}, {} };
    refreshJoystick(js);

    return js;
  }


  static Joystick &joystickById(std::vector<Joystick> &joysticks, unsigned int id)
  {
    auto joystick = std::find_if(begin(joysticks), end(joysticks), [id](const auto &j) { return j.id == id; });

    if (joystick == joysticks.end()) [[unlikely]] {
      joysticks.push_back(loadJoystick(id));
      return joysticks.back();
    } else [[likely]] {
      return *joystick;
    }
  }


  using Event = std::variant<std::monostate,
                             Pressed<Key>,
                             Released<Key>,
                             Pressed<JoystickButton>,
                             Released<JoystickButton>,
                             Moved<JoystickAxis>,
                             Moved<Mouse>,
                             Pressed<MouseButton>,
                             Released<MouseButton>,
                             CloseWindow,
                             TimeElapsed>;


  static sf::Event::KeyEvent toSFMLEventInternal(const Key &key)
  {
    return { .code = key.key, .alt = key.alt, .control = key.control, .shift = key.shift, .system = key.system };
  }

  static sf::Event::JoystickButtonEvent toSFMLEventInternal(const JoystickButton &joy)
  {
    return { .joystickId = joy.id, .button = joy.button };
  }

  static sf::Event::MouseMoveEvent toSFMLEventInternal(const Mouse &mouse) { return { .x = mouse.x, .y = mouse.y }; }

  static sf::Event::MouseButtonEvent toSFMLEventInternal(const MouseButton &mouse)
  {
    return { .button = static_cast<sf::Mouse::Button>(mouse.button), .x = mouse.mouse.x, .y = mouse.mouse.y };
  }

  static sf::Event::JoystickMoveEvent toSFMLEventInternal(const JoystickAxis &joy)
  {
    return { .joystickId = joy.id, .axis = static_cast<sf::Joystick::Axis>(joy.axis), .position = joy.position };
  }

  static sf::Event toSFMLEventInternal(const Pressed<Key> &value)
  {
    return { .type = sf::Event::KeyPressed, .key = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const Released<Key> &value)
  {
    return { .type = sf::Event::KeyReleased, .key = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const Pressed<JoystickButton> &value)
  {
    return sf::Event{ .type = sf::Event::JoystickButtonPressed, .joystickButton = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const Released<JoystickButton> &value)
  {
    return sf::Event{ .type = sf::Event::JoystickButtonReleased, .joystickButton = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const Moved<JoystickAxis> &value)
  {
    return sf::Event{ .type = sf::Event::JoystickMoved, .joystickMove = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const Moved<Mouse> &value)
  {
    return sf::Event{ .type = sf::Event::MouseMoved, .mouseMove = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const Pressed<MouseButton> &value)
  {
    return sf::Event{ .type = sf::Event::MouseButtonPressed, .mouseButton = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const Released<MouseButton> &value)
  {
    return sf::Event{ .type = sf::Event::MouseButtonReleased, .mouseButton = toSFMLEventInternal(value.source) };
  }

  static sf::Event toSFMLEventInternal(const CloseWindow & /*value*/)
  {
    return sf::Event{ .type = sf::Event::Closed, .size = {} };
  }

  static std::optional<sf::Event> toSFMLEvent(const Event &event)
  {
    return std::visit(
      overloaded{ [&](const auto &value) -> std::optional<sf::Event> { return toSFMLEventInternal(value); },
                  [&](const TimeElapsed &) -> std::optional<sf::Event> { return {}; },
                  [&](const std::monostate &) -> std::optional<sf::Event> { return {}; } },
      event);
  }

  std::vector<Event> pendingEvents;

  void setEvents(std::vector<Event> events) {
    pendingEvents = std::move(events);
  }

  Event nextEvent(sf::RenderWindow &window)
  {
    if (!pendingEvents.empty()) {
      auto event = pendingEvents.front();
      pendingEvents.erase(pendingEvents.begin());

      std::visit(overloaded{
                   [](const TimeElapsed &te) {
                     std::this_thread::sleep_for(te.elapsed);
                   },
                   [&](const Moved<Mouse> &me) {
                     sf::Mouse::setPosition({me.source.x, me.source.y}, window);
                   },
                   [](const auto &) { }
                 },
                 event);
      return event;
    }

    sf::Event event{};
    if (window.pollEvent(event)) { return toEvent(event); }

    const auto nextTick    = clock::now();
    const auto timeElapsed = nextTick - lastTick;
    lastTick               = nextTick;

    return TimeElapsed{ timeElapsed };
  }


  Event toEvent(const sf::Event &event)
  {
    switch (event.type) {
    case sf::Event::Closed:
      return CloseWindow{};
    case sf::Event::JoystickButtonPressed:
      return Pressed<JoystickButton>{ event.joystickButton.joystickId, event.joystickButton.button };
    case sf::Event::JoystickButtonReleased:
      return Released<JoystickButton>{ event.joystickButton.joystickId, event.joystickButton.button };
    case sf::Event::JoystickMoved:
      return Moved<JoystickAxis>{ event.joystickMove.joystickId, event.joystickMove.axis, event.joystickMove.position };

    case sf::Event::MouseButtonPressed:
      return Pressed<MouseButton>{ event.mouseButton.button, { event.mouseButton.x, event.mouseButton.y } };
    case sf::Event::MouseButtonReleased:
      return Released<MouseButton>{ event.mouseButton.button, { event.mouseButton.x, event.mouseButton.y } };
    // case sf::Event::MouseEntered:
    // case sf::Event::MouseLeft:
    case sf::Event::MouseMoved:
      return Moved<Mouse>{ event.mouseMove.x, event.mouseMove.y };
    // case sf::Event::MouseWheelScrolled:
    case sf::Event::KeyPressed:
      return Pressed<Key>{ event.key.alt, event.key.control, event.key.system, event.key.shift, event.key.code };
    case sf::Event::KeyReleased:
      return Released<Key>{ event.key.alt, event.key.control, event.key.system, event.key.shift, event.key.code };
    default:
      return std::monostate{};
    }
  }
};

template<typename T>
concept JoystickEvent =
  std::is_same_v<
    T,
    GameState::Pressed<
      GameState::
        JoystickButton>> || std::is_same_v<T, GameState::Released<GameState::JoystickButton>> || std::is_same_v<T, GameState::Moved<GameState::JoystickAxis>>;
}// namespace Game

namespace nlohmann {
template<> struct adl_serializer<Game::GameState::clock::duration>
{
  static void to_json(nlohmann::json &j, const Game::GameState::clock::duration &duration)
  {
    j = nlohmann::json{ { "nanoseconds", std::chrono::nanoseconds{duration}.count() } };
  }

  static void from_json(const nlohmann::json &j, Game::GameState::clock::duration &duration)
  {
    std::uint64_t value = j.at("nanoseconds");
    duration = std::chrono::nanoseconds{value};
  }
};

template<> struct adl_serializer<sf::Keyboard::Key>
{
  static void to_json(nlohmann::json &j, const sf::Keyboard::Key k)
  {
    j = nlohmann::json{ { "keycode", static_cast<int>(k) } };
  }

  static void from_json(const nlohmann::json &j, sf::Keyboard::Key &k)
  {
    k = static_cast<sf::Keyboard::Key>(j.at("keycode").get<int>());
  }
};


}// namespace nlohmann

namespace Game {
template<typename EventType, typename... Param> void serialize(nlohmann::json &j, const Param &... param)
{
  auto make_inner = [&]() {
    nlohmann::json innerObj;
    std::size_t    index = 0;

    (innerObj.emplace(EventType::elements[index++], param), ...);

    return innerObj;
  };

  nlohmann::json outerObj;
  outerObj.emplace(EventType::name, make_inner());
  j = outerObj;
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType & /*event*/) requires(EventType::elements.empty())
{
  serialize<EventType>(j);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event) requires(EventType::elements.size() == 1)
{
  const auto &[elem0] = event;
  serialize<EventType>(j, elem0);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event) requires(EventType::elements.size() == 2)
{
  const auto &[elem0, elem1] = event;
  serialize<EventType>(j, elem0, elem1);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event) requires(EventType::elements.size() == 3)
{
  const auto &[elem0, elem1, elem2] = event;
  serialize<EventType>(j, elem0, elem1, elem2);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event) requires(EventType::elements.size() == 4)
{
  const auto &[elem0, elem1, elem2, elem3] = event;
  serialize<EventType>(j, elem0, elem1, elem2, elem3);
}

template<typename EventType>
void to_json(nlohmann::json &j, const EventType &event) requires(EventType::elements.size() == 5)
{
  const auto &[elem0, elem1, elem2, elem3, elem4] = event;
  serialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
}

template<typename EventType, typename... Param> void deserialize(const nlohmann::json &j, Param &... param)
{
  // annoying conversion to string necessary for key lookup with .at?
  const auto &top = j.at(std::string{EventType::name});

  if (top.size() != sizeof...(Param)) {
    throw std::logic_error("Deserialization size mismatch");
  }

  std::size_t cur_elem = 0;
  (top.at(std::string{EventType::elements[cur_elem++]}).get_to(param), ...);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &/**/) requires(EventType::elements.size() == 0)
{
  deserialize<EventType>(j);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event) requires(EventType::elements.size() == 1)
{
  auto &[elem0] = event;
  deserialize<EventType>(j, elem0);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event) requires(EventType::elements.size() == 2)
{
  auto &[elem0, elem1] = event;
  deserialize<EventType>(j, elem0, elem1);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event) requires(EventType::elements.size() == 3)
{
  auto &[elem0, elem1, elem2] = event;
  deserialize<EventType>(j, elem0, elem1, elem2);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event) requires(EventType::elements.size() == 4)
{
  auto &[elem0, elem1, elem2, elem3] = event;
  deserialize<EventType>(j, elem0, elem1, elem2, elem3);
}

template<typename EventType>
void from_json(const nlohmann::json &j, EventType &event) requires(EventType::elements.size() == 5)
{
  auto &[elem0, elem1, elem2, elem3, elem4] = event;
  deserialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
}

template<typename ... T>
void choose_variant(const nlohmann::json &j, std::variant<std::monostate, T...> &variant)
{
  bool matched = false;

  auto try_variant = [&]<typename Variant>(){
    if (!matched) {
      try {
        Variant obj;
        from_json(j, obj);
        variant = obj;
        matched = true;
      } catch (const std::exception &) {
        // parse error, continue
      }
    }
  };

  (try_variant.template operator()<T>(), ...);
}

void from_json(const nlohmann::json &j, Game::GameState::Event &event)
{
  choose_variant(j, event);
}

void to_json(nlohmann::json &j, const Game::GameState::Event &event)
{
  std::visit(Game::overloaded{ [](const std::monostate &) {}, [&j](const auto &e) { to_json(j, e); } }, event);
}

}// namespace Game


#endif// MYPROJECT_INPUT_HPP
