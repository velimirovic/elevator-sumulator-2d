#ifndef BUTTON_H
#define BUTTON_H

#include <vector>
#include <string>
#include <GLFW/glfw3.h>

//Dugmad
struct Button {
    float x, y, width, height;
    std::string label;
    int floorNumber;  // -1 za kontrolne tastere
    bool pressed;
};

extern std::vector<Button> buttons;

// Funkcije
void initButtons();
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

#endif