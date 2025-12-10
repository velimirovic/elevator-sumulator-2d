#include "../Header/Lift.h"
#include <iostream>
#include "../Header/Person.h"

// Konstante
const float LIFT_SPEED = 0.3f;
const float DOOR_OPEN_TIME = 5.0f;
const float DOOR_SPEED = 0.1f;

// Lift podaci
float liftY = 0.0f;
int liftFloor = 2; // Trenutni sprat
std::vector<int> targetFloors; // Kliknuti spratovi
bool liftMoving = false;  // Da li se lift krece

// Vrata lifta podaci
bool doorsOpen = false;
float doorTimer = 0.0f;
float doorOffset = 0.0f;  // Koliko su vrata otvorena (0.0 = zatvorena, 0.12 = otvorena)
bool doorsFullyOpen = false; // Da li su vrata potpuno otvorena
bool doorsFullyClosed = true; // Da li su vrata potpuno zatvorena

// Kontrolni tasteri
bool ventilationOn = false;
bool openButtonPressed = false; // Samo jednom moze da se pritisne

// Kretanje lifta
void updateLift(float deltaTime, std::vector<Button>& buttons) {

    // Animacija vrata
    if (doorsOpen && doorOffset < 0.12f) {
        doorOffset += DOOR_SPEED * deltaTime;
        if (doorOffset >= 0.12f) {
            doorOffset = 0.12f;
            doorsFullyOpen = true;
            doorsFullyClosed = false;
        }
    }
    else if (!doorsOpen && doorOffset > 0.0f) {
        if (doorOffset < 0.1f) {
            doorsFullyOpen = false;
        }

        doorOffset -= DOOR_SPEED * deltaTime;
        if (doorOffset <= 0.0f) {
            doorOffset = 0.0f;
            doorsFullyClosed = true;
            doorsFullyOpen = false;
            canEnterLift = true;
            openButtonPressed = false;
        }
    }

    // Ako su vrata otvorena cekaj
    if (doorsOpen) {
        doorTimer -= deltaTime;
        if (doorTimer <= 0.0f) {
            doorsOpen = false;
            std::cout << "Vrata se zatvaraju" << std::endl;
        }
        return;
    }

    if (!doorsFullyClosed) {
        return;
    }

    // Ako nema ciljeva stani
    if (targetFloors.empty()) {
        liftMoving = false;
        return;
    }

    liftMoving = true;
    int currentTarget = targetFloors[0];
    float targetY = -1.0f + (currentTarget * 2.0f / 8.0f);

    // Kreci se ka cilju
    if (liftY < targetY) {
        liftY += LIFT_SPEED * deltaTime;
        if (liftY >= targetY) {
            liftY = targetY;
            liftFloor = currentTarget;

            // Stigao na sprat - otvori vrata
            doorsOpen = true;
            doorTimer = DOOR_OPEN_TIME;
            canEnterLift = true;
            liftMoving = false;

            if (ventilationOn) {
                ventilationOn = false;
                std::cout << "Ventilacija iskljucena - lift stigao na ciljani sprat" << std::endl;
            }

            // Ugasi taster
            for (auto& btn : buttons) {
                if (btn.floorNumber == currentTarget) {
                    btn.pressed = false;
                    break;
                }
            }

            // Ukloni iz liste
            targetFloors.erase(targetFloors.begin());

            std::cout << "Lift stigao na sprat " << liftFloor << " - vrata otvorena" << std::endl;
        }
    }
    else if (liftY > targetY) {
        liftY -= LIFT_SPEED * deltaTime;
        if (liftY <= targetY) {
            liftY = targetY;
            liftFloor = currentTarget;

            if (ventilationOn) {
                ventilationOn = false;
                std::cout << "Ventilacija iskljucena - lift stigao na ciljani sprat" << std::endl;
            }

            // Stigao na sprat - otvori vrata
            doorsOpen = true;
            doorTimer = DOOR_OPEN_TIME;
            canEnterLift = true;
            liftMoving = false;

            // Ugasi taster
            for (auto& btn : buttons) {
                if (btn.floorNumber == currentTarget) {
                    btn.pressed = false;
                    break;
                }
            }

            // Ukloni iz liste
            targetFloors.erase(targetFloors.begin());

            std::cout << "Lift stigao na sprat " << liftFloor << " - vrata otvorena" << std::endl;
        }
    }
}

float getLiftCenterY() {
    float floorHeight = 2.0f / 8.0f;
    return liftY + floorHeight / 2.0f;
}