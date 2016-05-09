#include <stdlib.h>
#include <stdio.h>
#include "stateMachine.h"
#include "motion.h"
#include "animal.h"

StateMachine::StateMachine(){

}

StateMachine::StateMachine(states initial){
  currentState = initial;
}

void StateMachine::setState(states newState){
  exitCurrentState();
  enterState(newState);
}

void StateMachine::exitCurrentState(){
  switch(currentState){
   
    case(walking):
      motion_set_dx(0.0);
      resetPosition();
      break;
      
    case(scanning):
      resetPosition();
      break;
      
    case(turning):
      motion_set_turn(0.0);
      resetPosition();
      break;
  }
}

void StateMachine::enterState(states newState){
  resetTimeSpentInState();
  currentState = newState;
  
}

states StateMachine::getCurrentState(){
  return currentState;
}

float StateMachine::getTimeSpentInState(){
  return timeSpentInState;
}

void StateMachine::resetTimeSpentInState(){
  timeSpentInState = 0;
}

void StateMachine::tick(float f){
  timeSpentInState += f*0.02;
}

