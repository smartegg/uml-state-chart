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
#include "HandleError.hpp"


namespace ndsl {

namespace fsm {


StateMachine::StateMachine(Transition* transitions, int len , State& initStatus)
  : table_(transitions),
    size_(len),
    initStatus_(initStatus),
    status_(0) {

}

StateMachine::~StateMachine() {

}

void StateMachine::start() {
  status_ = &initStatus_;
  buildMachine();
}

void StateMachine::processEvent(Event& event) {
  Data data = maps_[status_][&event];
  if (status_ == 0) {
    NDSL_FAIL();
  }
  if (data.guard->run()) {
    data.action->run();
    status_ = data.destState;
  }
}

State& StateMachine::currentState() const {
  return *status_;
}

void StateMachine::buildMachine() {
  for (int i = 0; i < size_; i++) {
    maps_[&(table_[i].sourceState)][&(table_[i].event)] =
      Data(&(table_[i].destState), &(table_[i].action), &(table_[i].guard));
  }

}


} //namespace fsm

} //namespace ndsl
