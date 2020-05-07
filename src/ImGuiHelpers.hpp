//
// Created by jason on 5/7/20.
//

#ifndef MYPROJECT_IMGUIHELPERS_HPP
#define MYPROJECT_IMGUIHELPERS_HPP

#include <imgui.h>
#include <string_view>
#include <fmt/format.h>

namespace ImGuiHelper {
template<typename... Param> static void Text(std::string_view format, Param && ... param)
{
  ImGui::TextUnformatted(fmt::format(format, std::forward<Param>(param)...).c_str());
}

}

#endif// MYPROJECT_IMGUIHELPERS_HPP
