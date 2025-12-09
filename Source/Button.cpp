#include "../Header/Button.h"
#include "../Header/Lift.h"
#include "../Header/Person.h"
#include <iostream>

std::vector<Button> buttons;

// Inicijalizacija dugmica
void initButtons() {
    buttons.clear();

    // 8 spratova - 2 kolone, 4 reda
    std::string floorLabels[] = { "5", "6", "3", "4", "1", "2", "PR", "SU" };
    int floorNumbers[] = { 6, 7, 4, 5, 2, 3, 1, 0 };  // Realni spratovi (0 = SU, 1 = PR/0, 2-7 = 1-6)

    for (int i = 0; i < 8; i++) {
        Button btn;
        int row = i / 2;  // 0-3
        int col = i % 2;  // 0-1

        btn.x = -0.65f + col * 0.3f;  // Leva ili desna kolona
        btn.y = 0.6f - row * 0.2f;    // Od vrha prema dole
        btn.width = 0.1f;
        btn.height = 0.08f;
        btn.label = floorLabels[i];
        btn.floorNumber = floorNumbers[i];
        btn.pressed = false;
        buttons.push_back(btn);
    }

    // 4 kontrolna tastera - 2 kolone, 2 reda
    std::string controlLabels[] = { "OPEN", "CLOSE", "STOP", "VENT" };
    for (int i = 0; i < 4; i++) {
        Button btn;
        int row = i / 2;
        int col = i % 2;

        btn.x = -0.65f + col * 0.3f;
        btn.y = -0.4f - row * 0.15f;
        btn.width = 0.12f;
        btn.height = 0.08f;
        btn.label = controlLabels[i];
        btn.floorNumber = -1 - i;
        btn.pressed = false;
        buttons.push_back(btn);
    }
}

// Kliktanje dugmica
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // Uzmi poziciju misa
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // Konvertuj iz pixel koordinata u OpenGL koordinate
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        float glX = (mouseX / windowWidth) * 2.0f - 1.0f;
        float glY = 1.0f - (mouseY / windowHeight) * 2.0f;

        // Proveri da li je kliknuto na neki taster
        for (auto& btn : buttons) {
            float halfWidth = btn.width / 2.0f;
            float halfHeight = btn.height / 2.0f;

            // Provera kolizije
            if (glX >= btn.x - halfWidth && glX <= btn.x + halfWidth &&
                glY >= btn.y - halfHeight && glY <= btn.y + halfHeight) {

                // Tasteri se mogu kliktati samo ako je osoba u liftu!
                if (!personInLift) {
                    std::cout << "Osoba mora biti u liftu da bi koristila tastere!" << std::endl;
                    return;  // Ignorisi klik
                }

                // Pritisnut taster
                btn.pressed = true;

                // Ako je taster za sprat
                if (btn.floorNumber >= 0 && btn.floorNumber != liftFloor) {
                    // Proveri da li vec postoji u listi kliknutih
                    bool already = false;
                    for (int f : targetFloors) {
                        if (f == btn.floorNumber) {
                            already = true;
                            break;
                        }
                    }

                    if (!already) {
                        targetFloors.push_back(btn.floorNumber);
                        liftMoving = true;
                        std::cout << "Dodat sprat " << btn.floorNumber << " u listu" << std::endl;
                    }
                }

                else if (btn.floorNumber == -1) {  // OPEN
                    if (doorOffset > 0.0f && !openButtonPressed) {
                        doorsOpen = true;
                        doorTimer = 5.0f; // DOOR_OPEN_TIME
                        openButtonPressed = true;
                        doorsFullyOpen = false;
                        std::cout << "OPEN - vrata produzena za 5s" << std::endl;
                    }
                }

                else if (btn.floorNumber == -2) {  // CLOSE
                    if (doorsOpen) {
                        doorTimer = 0.0f;
                        doorsFullyOpen = false;
                        std::cout << "CLOSE - vrata se zatvaraju" << std::endl;
                    }
                }

                else if (btn.floorNumber == -3) {  // STOP
                    targetFloors.clear();
                    liftMoving = false;

                    // Ugasi sve pritisnute tastere za spratove
                    for (auto& b : buttons) {
                        if (b.floorNumber >= 0) {  // Samo spratovi (ne kontrolni tasteri)
                            b.pressed = false;
                        }
                    }

                    std::cout << "STOP - lift zaustavljen" << std::endl;
                }

                else if (btn.floorNumber == -4) {  // VENT
                    ventilationOn = !ventilationOn;
                    std::cout << "VENT - ventilacija " << (ventilationOn ? "ON" : "OFF") << std::endl;
                }

                std::cout << "Kliknuto na taster: " << btn.label << std::endl;
                break;
            }
        }
    }
}