/**
 * @file TransitionTable.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Tue, 22 May 2012 11:05:40
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */

#include "TransitionTable.hpp"

#include <typeinfo>

namespace ndsl {

namespace fsm {


State::State() {
  onEntry();
}

State::~State() {
  onExit();
}

void State::onEntry() {

}

void State::onExit() {

}

std::string State::name()  const {
  return typeid(*this).name();
}

Event::Event() {

}

Event::~Event() {
  //empty impl for pure virtual destrctor of class Event
}

Action::~Action() {

}

Guard::~Guard() {

}



} //namespace fsm

} //namespace ndsl

