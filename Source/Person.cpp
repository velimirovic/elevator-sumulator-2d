#include "../Header/Person.h"
#include "../Header/Lift.h"
#include <iostream>

// Covek podaci
float personX = 0.04f;
float personY = -0.7f;  // Prizemlje
bool personInLift = false;
int walkFrame = 0;
float walkAnimTimer = 0.0f;
bool facingRight = true;
bool isWalking = false;
int personFloor = 1;
int personEnteredFloor = 1;
bool shouldExit = false;
bool canEnterLift = true;
const float PERSON_SPEED = 0.5f;

//Kretanje osobe
void movePersonLeftRight(GLFWwindow* window, float deltaTime) {
    if (personInLift) {
        isWalking = false;
        return;
    }

    isWalking = false;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        personX -= PERSON_SPEED * deltaTime;
        if (personX < 0.015f)
            personX = 0.015f;
        facingRight = false;  // Gleda levo
        isWalking = true;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        personX += PERSON_SPEED * deltaTime;
        if (personX > 0.83f)
            personX = 0.83f;
        facingRight = true;  // Gleda desno
        isWalking = true;
    }

    // Animacija hodanja
    if (isWalking) {
        walkAnimTimer += deltaTime;
        if (walkAnimTimer >= 0.1f) {  // Menja frejm svakih 0.1s
            walkAnimTimer = 0.0f;
            walkFrame = (walkFrame + 1) % 6;  // 0-5
        }
    }
    else {
        walkFrame = 0;  // Reset na standing
    }
}

// Ulazak/izlazak iz lifta
void handlePersonEnterExit() {

    // Ulazak - samo ako je lift na istom spratu kao osoba i su vrata otvorena
    if (doorsFullyOpen && liftFloor == personFloor && canEnterLift && !liftMoving) {
        if (!personInLift && personX > 0.75f) {
            personInLift = true;
            personEnteredFloor = personFloor;
            shouldExit = false;
            canEnterLift = false;
            std::cout << "Osoba ulazi u lift na spratu " << personFloor << std::endl;
        }
    }

    // Izlazak
    if (doorsFullyOpen && personInLift && shouldExit) {
        personInLift = false;
        personFloor = liftFloor;
        personX = 0.8f;
        shouldExit = false;
        canEnterLift = false;
        std::cout << "Osoba izlazi iz lifta na spratu " << personFloor << std::endl;
    }
}