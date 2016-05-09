#include "animal.h"
#include "terminal.h"
#include <math.h>
#include <stdio.h>
#include "distance.h"
#include "motion.h"
#include "function.h"
#include "leds.h"
#include "stateMachine.h"

const float DEFAULT_BREATH_FREQ = 0.04;

float elapsedTime = 0;
Function look_goDown;
Function look_leftRight;

bool breathingStop = false;

TERMINAL_PARAMETER_FLOAT(breathAmp, "Breathing amp", 5);
TERMINAL_PARAMETER_FLOAT(breathFreq, "Breathing freq", DEFAULT_BREATH_FREQ);
float breath_t = 0.0;

float current_stamina = 100.0;

TERMINAL_PARAMETER_INT(stamina, "Activate/deactivate stamina control", 0);
TERMINAL_PARAMETER_FLOAT(staminaMax, "Endurence of the robot", 100);
TERMINAL_PARAMETER_FLOAT(staminaRegeneration, "Rate at which the robot regains stamina", 0.02);
TERMINAL_PARAMETER_FLOAT(staminaConsumptionRatio, "Rate at which the robot regains stamina", 0.001);



TERMINAL_PARAMETER_INT(autonomous, "Activate/deactivate autonomous behaviour", 0);
float lastTime;
float farthest = 0;

enum directions {left, straight, right};

directions dir;
StateMachine generalStates;
StateMachine idleStates;
StateMachine movingStates;

void setupAnimalBehaviour(){
  
  look_goDown.clear();
  look_leftRight.clear();

  look_goDown.addPoint(0.0, 30.0);
  look_goDown.addPoint(10.0, 20.0);
  look_goDown.addPoint(20.0, 30.0);

  look_leftRight.addPoint(0.0, 0.0);
  look_leftRight.addPoint(2.0, 50.0);
  look_leftRight.addPoint(10.0, 50.0);
  look_leftRight.addPoint(12.0, -50.0);
  look_leftRight.addPoint(18.0, -50.0);
  look_leftRight.addPoint(20.0, 0.0);

  generalStates = StateMachine(idle);
  idleStates = StateMachine(bre);
  movingStates = StateMachine(walking);

}

void handleAnimalBehaviour(float f){
  tick(f);  

  handleStamina();
  
  switch(generalStates.getCurrentState()){
    case(idle):
      handleIdle();
      break;
      
    case(mov):
      handleAutonomousBehaviour();
      break;
  }
}

void handleStamina(){
  
  int speed = (abs(motion_get_dx())>abs(motion_get_dy()) ? abs(motion_get_dx()) : abs(motion_get_dy()));

  if(motion_is_moving() && speed > 40){ //decrease stamina
    current_stamina -= staminaConsumptionRatio*(speed/5);    
  }
  else{ //regeneration
    current_stamina+=staminaRegeneration*(speed/10);
  }
  if(current_stamina>60){
    led_set_all(LED_R|LED_G|LED_B);
    motion_set_extra_dx(0);
    motion_set_extra_dy(0);
  }
  else if(current_stamina<60 && current_stamina>40){
    led_set_all(LED_R|LED_B);
    if(motion_get_dx() != 0)
      motion_set_extra_dx( (motion_get_dx()>0)? -20 : 20 );
    else
      motion_set_extra_dx(0);

    if(motion_get_dy() != 0)
      motion_set_extra_dy( (motion_get_dy()>0)? -20 : 20 );
    else
      motion_set_extra_dy(0);
  }
  else if(current_stamina<40 && current_stamina>20){
    led_set_all(LED_R|LED_G);
    if(motion_get_dx() != 0)
      motion_set_extra_dx( (motion_get_dx()>0)? -40 : 40 );
    else
      motion_set_extra_dx(0);
    if(motion_get_dy() != 0)
      motion_set_extra_dy( (motion_get_dy()>0)? -40 : 40 );
    else
      motion_set_extra_dy(0);
  }
 else if(current_stamina<20){
    led_set_all(LED_R);
    if(motion_get_dx() != 0)
      motion_set_extra_dx( (motion_get_dx()>0)? -60 : 60 );
    else
      motion_set_extra_dx(0);

    if(motion_get_dy() != 0)
      motion_set_extra_dy( (motion_get_dy()>0)? -60 : 60 );
    else
      motion_set_extra_dy(0);
  }
 else if(current_stamina<=0){
   led_set_all(LED_R);
   if(motion_get_dx() != 0)
      motion_set_extra_dx( (motion_get_dx()>0)? -motion_get_dx() : motion_get_dx() );
    else
      motion_set_extra_dx(0);

    if(motion_get_dy() != 0)
      motion_set_extra_dy( (motion_get_dy()>0)? -motion_get_dy() : motion_get_dy() );
    else
      motion_set_extra_dy(0);
 }
}

void fear(){
  for(int i = 0 ; i < 4 ; i++){
    motion_set_extra_x(i, getExtra_x(motion_get_extra_x(i), motion_is_moving()));
    motion_set_extra_y(i, getExtra_y(motion_get_extra_y(i), motion_is_moving())); 
  }
}

void periodicMovements(){
  int elapsed = static_cast<int>(idleStates.getTimeSpentInState());
  if(elapsed>30 && elapsed <50)
    stretching();
  if(elapsed > 60 && elapsed < 80){
    if(elapsed > 60 && elapsed < 61){
      resetPosition();
      breathingStop = true;
    }
    lookAround();
  }
  if(elapsed >=  80 && elapsed < 81){
    resetPosition();
    breathingStop = false;
    idleStates.resetTimeSpentInState();
  } 
}

void lookAround(){
  
  //float h = look_goDown.getMod(breath_t);
  float y = look_leftRight.getMod(breath_t);
  //motion_extra_z(3, h);
  //motion_extra_z(2, h);
  motion_set_extra_y(0, y);
  motion_set_extra_y(1, y);
}

void stretching(){
  for(int i = 0 ; i < 4 ; i++){
    if(static_cast<int>(elapsedTime)%30 == i)
      motion_extra_z(i, 20);
    else
      motion_extra_z(i, 0);
  }
}

void resetPosition(){
  for(int i = 0 ; i < 4 ; i++){
    motion_set_extra_x(i, 0);
    motion_set_extra_y(i, 0);
    motion_extra_z(i, 0);
    motion_set_h(0);
  }
}

void breathing(){
  if(!breathingStop)
    motion_set_h(breathAmp*sin(breath_t*2*M_PI*breathFreq));
}

void handleIdle(){
  handleStamina();
  //behaviour if not moving
  if(!motion_is_moving()){
    breathing();
    fear();	
    periodicMovements();
  }
  else
    resetPosition();
}

void handleAutonomousBehaviour(){  
    
  switch(movingStates.getCurrentState()){

    case(walking):
      motion_set_dx(40.0);
      if(distance_get()<20)//obstacle
	movingStates.setState(scanning);
      break;
  
    case(scanning):
      if(movingStates.getTimeSpentInState() < 20){
	float last = scanSurroundings();
	if(last > farthest){
	  farthest = last;
	  if(motion_get_extra_y(0) > 0 && motion_get_extra_y(0) < 50)
	    dir = left;
	  else if(motion_get_extra_y(0) > -50 && motion_get_extra_y(0) < 0)
	    dir = right;
	  else
	    dir = right;//arbitraire, trouver une idée intelligente
	}
      }
      else
	movingStates.setState(turning);
      break;

    case(turning):
      if(movingStates.getTimeSpentInState() < 10){
	
	switch(dir){
	  case left:motion_set_turn(-20);
	    break;
	  case straight:movingStates.setState(walking);
	    break;
	  case right:motion_set_turn(20);
	    break;
	}
      
      }
      else
	movingStates.setState(walking);
      break;

  }
  
}

float scanSurroundings(){
  float y = look_leftRight.getMod(breath_t);  
  motion_set_extra_y(0, y);
  motion_set_extra_y(1, y);  
  return distance_get();
}

float getExtra_x(float currentX, bool moving){
  if(moving)
    return currentX;
  else{
    if(distance_get()>80)
      return 0;
    else
      return currentX + 0.05*((80 - distance_get()) - currentX);
  }
}

float getExtra_y(float currentY, bool moving){
  if(moving)
    return currentY;
  else{
    return currentY;
  }
}

//A CORRIGER
float getExtra_dx(float currentDx){
  if (distance_get()<10){
    //return -70;
    return 0;
    }
  else
    return 0;
}

void tick(float f){
  
  elapsedTime+= f*0.02;

  generalStates.tick(f);
  idleStates.tick(f);
  movingStates.tick(f);

  breath_t += f*0.02;
  if(breath_t>20.0)
    breath_t-=20.0;
}
