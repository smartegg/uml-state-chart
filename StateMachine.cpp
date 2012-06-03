/**
 * @file StateMachine.cpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Tue, 22 May 2012 10:29:29
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#include "StateMachine.hpp"


#include <iostream>
#include "TransitionTable.hpp"
#include "HandleError.hpp"


namespace ndsl {

namespace fsm {


StateMachine::StateMachine(Transition* transitions, size_t len , State& initStatus)
  : initStatus_(initStatus),
    status_(0) {
    for(size_t i = 0; i < len;i++) {
      table_.push_back(transitions[i]);
    }

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
  std::cout << "DEBUG:before transition: "<< status_->name() << std::endl;
  if (data.guard->run()) {
    data.action->run(&event);
    status_ = data.destState;
  }
  std::cout << "DEBUG:after transition: " << status_->name() << std::endl;
}

State& StateMachine::currentState() const {
  return *status_;
}

void StateMachine::buildMachine() {
  for (int i = 0; i < table_.size(); i++) {
    std::cout << "DEBUG: " << table_[i].sourceState->name()
      << "\tto\t" << table_[i].destState->name()<<std::endl;
    maps_[(table_[i].sourceState)][(table_[i].event)] =
      Data((table_[i].destState), (table_[i].action), (table_[i].guard));
  }

}


} //namespace fsm

} //namespace ndsl
