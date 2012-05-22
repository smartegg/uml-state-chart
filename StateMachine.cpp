/**
 * @file StateMachine.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Tue, 22 May 2012 10:29:29
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#include "StateMachine.hpp"

#include "TransitionTable.hpp"

namespace ndsl {

namespace fsm {


StateMachine::StateMachine(Transition* transitions, int len , State& initStatus)
  :table_(transitions),
   size_(len),
   initStatus_(initStatus),
   status_(0) {

}

StateMachine::~StateMachine() {

}

void StateMachine::start() {
  status_ = &initStatus_;
}

void StateMachine::processEvent(Event& event) {

}

State& StateMachine::currentState() const{
  return *status_;
}

void StateMachine::buildMachine() {

}


} //namespace fsm

} //namespace ndsl
