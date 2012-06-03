#include <cstdio>
#include <iostream>
#include "TransitionTable.hpp"
#include "StateMachine.hpp"
using namespace ndsl::fsm;


struct StatusA : State {};
struct StatusB : State {};
struct StatusC : State {};

struct EventA  : Event {
  int b;
};
struct ActionA   : Action  {
  void run(Event* ev) {
    EventA* a = static_cast<EventA*>(ev);
    printf("%d\n", a->b);
  }
};

struct None : Guard {
  bool run() {
    return true;
  }
};


int main(int argc, char const* argv[]) {
  StatusA* sa = new StatusA();
  StatusB* sb = new StatusB();
  StatusC* sc = new StatusC();
  EventA* ea = new EventA();
  ActionA* aa = new ActionA();
  None* none = new None();


  Transition transitions[] = {
    {sa, sb, ea, aa, none},
    {sb, sc, ea, aa, none}
  };

  StateMachine sm(transitions, 2, *sa);
  sm.start();
//  std::cout<< sm.currentState().name()<<std::endl;
  ea->b = 1;
  sm.processEvent(*ea);
//  std::cout<< sm.currentState().name()<<std::endl;
  ea->b = 2;
  sm.processEvent(*ea);
//  std::cout<< sm.currentState().name()<<std::endl;


  return 0;
}
