/**
 * @file StateMachine.hpp
 * @brief
 * @author smartegg<lazysmartegg@gmail.com>
 * @version 1.0
 * @date Tue, 22 May 2012 10:29:27
 * @copyright Copyright (C) 2012 smartegg<lazysmartegg@gmail.com>
 */
#ifndef _NDSL_STATEMACHINE_HPP_
#define _NDSL_STATEMACHINE_HPP_

#include <map>
#include <vector>
#include "TransitionTable.hpp"

namespace ndsl {

namespace fsm {


/**
 * @brief StateMachine a lightweight uml-based fsm (just event-table-based)
 */
class StateMachine {
  public:
    StateMachine(Transition* transitions, size_t len , State& initStatus);
    virtual ~StateMachine();

    void start();
    void processEvent(Event& event);
    State&  currentState() const;

  private:
    /**
     * @brief buildMachine
     * a helper class to build info to lookup quickly.(create adj-list)
     * @param table
     */
    void buildMachine();


    struct Data {
      Data(State* d=0, Action* a=0, Guard* g=0)
        : destState(d), action(a), guard(g) {

      }

      State* destState;
      Action* action;
      Guard* guard;
    };

    State& initStatus_;
    State* status_;

    std::map<State*, std::map<Event*, Data>  >  maps_;
    std::vector<Transition> table_;
};

} //namespace fsm

} //namespace ndsl


#endif // _NDSL_STATEMACHINE_HPP_

