#include <cstdio>
#include <iostream>
#include "TransitionTable.hpp"
#include "StateMachine.hpp"
using namespace ndsl::fsm;


struct StatusA : State {
  void onEntry() {
    puts("A: onEntry");
  }
  void onExit() {
    puts("A: onExit");
  }
};
struct StatusB : State {};
struct StatusC : State {};

struct EventA  : Event {};
struct ActionA   : Action  {
  void run() {
    puts("hello world\n");
  }
};


int main(int argc, char const* argv[]) {
  StatusA sa;
  StatusB sb;
  StatusC sc;
  EventA ea;
  ActionA aa;
  None none;


  Transition transitions[] = {
    {sa, sb, ea, aa, none},
    {sb, sc, ea, aa, none}
  };

  StateMachine sm(transitions, 2, sa);
  sm.start();
//  std::cout<< sm.currentState().name()<<std::endl;
  sm.processEvent(ea);
//  std::cout<< sm.currentState().name()<<std::endl;
  sm.processEvent(ea);
//  std::cout<< sm.currentState().name()<<std::endl;


  return 0;
}
