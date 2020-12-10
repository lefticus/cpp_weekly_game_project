//
// Created by jason on 7/30/20.
//

#ifndef MYPROJECT_UTILITY_HPP
#define MYPROJECT_UTILITY_HPP

namespace Game {
template<class... Ts> struct overloaded : Ts...
{
  using Ts::operator()...;
};
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}


#endif// MYPROJECT_UTILITY_HPP
