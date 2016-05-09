#ifndef STM_H
#define STM_H

enum states {bre, str, loo, mov, idle, walking, scanning, turning};

class StateMachine{

 public :

  StateMachine();
  StateMachine(states initial);

  void setState(states newState);
  void exitCurrentState();
  void enterState(states newState);
  
  states getCurrentState();
  float getTimeSpentInState();
  
  void resetTimeSpentInState();
  void tick(float f);


 private :
  states currentState;
  float timeSpentInState;
};


#endif
