#ifndef _NDSL_TRANSITIONTABLE_HPP_
#define _NDSL_TRANSITIONTABLE_HPP_

#include <string>

namespace ndsl {

namespace fsm {

/**
 * @brief State represent the State in UML statechart, support entry/exit semantic
 *   FIXME:  every state must be  identify  as a unique number ?
 *
 */
class State {
  public:
    State();
    virtual ~State() = 0;
    void onEntry();
    void onExit();
    virtual std::string name() const;
};


/**
 * @brief Event  represent the Event in UML statechart
 */
class Event {
  public:
    Event();
    virtual ~Event() = 0;
};

/**
 * @brief Action represent the actions in UML statechart
 * just inheritance it and write a functor class
 */
class Action {
  public:
    virtual ~Action() = 0;
};

/**
 * @brief Guard  represent the guard condition in UML statechart
 * just inheritance it and write a functor class
 * add  somethinkg like bool ()(var) {}
 */
class Guard {
  public:
    virtual ~Guard() = 0;
};

struct none : Guard {
  bool operator()() {
    return true;
  }
};

struct Transition {
  State& sourceStatus;
  State& destStatus;
  Event& event;
  Action& action;
  Guard& guard;
};


} //namespace fsm

} //namespace ndsl

#endif // _NDSL_TRANSITIONTABLE_HPP_

