#ifndef LIFT_H
#define LIFT_H

#include <vector>
#include "../Header/Button.h"

// Lift podaci
extern float liftY;
extern int liftFloor;
extern std::vector<int> targetFloors;
extern bool liftMoving;

// Vrata lifta podaci
extern bool doorsOpen;
extern float doorTimer;
extern float doorOffset;
extern bool doorsFullyOpen;
extern bool doorsFullyClosed;

// Kontrolni tasteri
extern bool ventilationOn;
extern bool openButtonPressed;

// Funkcije
void updateLift(float deltaTime, std::vector<Button>& buttons);
float getLiftCenterY();

#endif