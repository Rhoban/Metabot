#ifndef _METABOT_ANIMAL_H
#define _METABOT_ANIMAL_H


void setupAnimalBehaviour();
void handleAnimalBehaviour(float f);
void breathing();

float getExtra_x(float currentX, bool moving);
float getExtra_y(float currentY, bool moving);
float getExtra_dx(float currentDx);

void tick(float f);
void stretching();
void resetPosition();
void lookAround();
void periodicMovements();
void fear();
void handleStamina();
void handleIdle();
void handleAutonomousBehaviour();
float scanSurroundings();

#endif
