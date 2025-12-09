#ifndef PERSON_H
#define PERSON_H

#include <GLFW/glfw3.h>

// Covek podaci
extern float personX;
extern float personY;
extern bool personInLift;
extern int walkFrame;
extern float walkAnimTimer;
extern bool facingRight;
extern bool isWalking;
extern int personFloor;
extern int personEnteredFloor;
extern bool shouldExit;
extern bool canEnterLift;

// Funkcije
void movePersonLeftRight(GLFWwindow* window, float deltaTime);
void handlePersonEnterExit();

#endif