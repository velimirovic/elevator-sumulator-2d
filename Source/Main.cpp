#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../Header/Util.h"
#include "../Header/Lift.h"
#include "../Header/Person.h"
#include "../Header/Button.h"
#include <map>
#include <iostream>

// - Srećan rad! 
// - Hvala!

const float TARGET_FPS = 75.0f;
const float FRAME_TIME = 1.0f / TARGET_FPS;

//Cursor
GLFWcursor* cursorNormal = NULL;
GLFWcursor* cursorPropeller = NULL;

//Mapiranje karaktera
std::map<char, std::pair<int, int>> charMap = {
    {'0', {0, 0}}, {'1', {1, 0}}, {'2', {2, 0}}, {'3', {3, 0}}, {'4', {4, 0}}, {'5', {5, 0}}, {'6', {6, 0}}, {'7', {7, 0}},
    {'C', {0, 1}}, {'E', {1, 1}}, {'L', {2, 1}}, {'N', {3, 1}}, {'O', {4, 1}}, {'P', {5, 1}}, {'R', {6, 1}}, {'S', {7, 1}},
    {'T', {0, 2}}, {'U', {1, 2}}, {'V', {2, 2}}
};

unsigned int fontTexture;


//Callback za tastere (ESC, C, L)
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // ESC - izlazak iz programa
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // C - pozivanje lifta
    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        if (!personInLift) {
            if (personX < 0.75f) {
                std::cout << "Osoba mora biti blize liftu" << std::endl;
                return;
            }

            // Proveri da li vec postoji u listi targetFloors
            bool alreadyInQueue = false;
            for (int f : targetFloors) {
                if (f == personFloor) {
                    alreadyInQueue = true;
                    break;
                }
            }

            // Ako je lift bas na istom spratu (ne gleda da li ima ciljeve), otvori vrata
            if (personFloor == liftFloor && !liftMoving) {
                if (!doorsOpen) {
                    doorsOpen = true;
                    doorTimer = 5.0f;
                    canEnterLift = true;
                    std::cout << "Lift je ovde - otvaraju se vrata" << std::endl;
                }
                else {
                    std::cout << "Lift je vec ovde sa otvorenim vratima" << std::endl;
                }
            }
            // Inace, ako lift nije na ovom spratu i nije vec pozvan, dodaj u listu
            else if (!alreadyInQueue) {
                targetFloors.push_back(personFloor);
                liftMoving = true;
                std::cout << "Osoba poziva lift na sprat " << personFloor << std::endl;
            }
            // Ako je vec pozvan, ne radi nista
            else {
                std::cout << "Lift je vec pozvan na ovaj sprat" << std::endl;
            }
        }
    }

    // L - izlazak iz lifta
    if (key == GLFW_KEY_L && action == GLFW_PRESS) {
        // Osoba moze da izadje samo ako je u liftu i vrata su potpuno otvorena
        if (personInLift && doorsFullyOpen && !doorsFullyClosed) {
            shouldExit = true;
            std::cout << "Osoba zeli da izađe na spratu " << liftFloor << std::endl;
        }
        else if (!personInLift) {
            std::cout << "Osoba nije u liftu" << std::endl;
        }
        else if (!doorsFullyOpen || doorsFullyClosed) {
            std::cout << "Vrata nisu potpuno otvorena" << std::endl;
        }
    }
}

//Crtanje pravougaonika
void drawRect(unsigned int shader, unsigned int VAO,
    float x, float y, float width, float height,
    float r, float g, float b, float a) {
    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader, "uScale"), width, height);
    glUniform4f(glGetUniformLocation(shader, "uColor"), r, g, b, a);
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

//Teksture
void drawTexturedRect(unsigned int shader, unsigned int VAO, unsigned int texture,
    float x, float y, float width, float height) {
    glUseProgram(shader);
    glUniform2f(glGetUniformLocation(shader, "uPos"), x, y);
    glUniform2f(glGetUniformLocation(shader, "uScale"), width, height);
    glUniform4f(glGetUniformLocation(shader, "uColor"), 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform1i(glGetUniformLocation(shader, "uUseTexture"), 1);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(glGetUniformLocation(shader, "uTex"), 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

//Font
void drawText(const std::string& text, float x, float y, float size,
    float r, float g, float b, unsigned int shader, unsigned int VAO) {
    glUseProgram(shader);

    float charSpacing = size * 0.6f;  // Razmak izmedju slova
    float currentX = x;

    for (char c : text) {
        auto pos = charMap[c];
        int col = pos.first;
        int row = pos.second;

        // Teksturne koordinate (8 kolona, 3 reda)
        float texOffsetX = col / 8.0f;
        float texOffsetY = (2 - row) / 3.0f;
        float texSizeX = 1.0f / 8.0f;
        float texSizeY = 1.0f / 3.0f;

        glUniform2f(glGetUniformLocation(shader, "uPos"), currentX, y);
        glUniform2f(glGetUniformLocation(shader, "uScale"), size, size);
        glUniform2f(glGetUniformLocation(shader, "uTexOffset"), texOffsetX, texOffsetY);
        glUniform2f(glGetUniformLocation(shader, "uTexSize"), texSizeX, texSizeY);
        glUniform3f(glGetUniformLocation(shader, "uColor"), r, g, b);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fontTexture);
        glUniform1i(glGetUniformLocation(shader, "uTex"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        currentX += charSpacing;
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //Fullscreen 
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Lift - RA63/2022", monitor, NULL);
    if (window == NULL) return endProgram("Prozor nije uspeo da se kreira.");

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyboard_callback); // Poziv za ESC, C, L
    glfwSetMouseButtonCallback(window, mouse_button_callback); // Poziv za klikove

    if (glewInit() != GLEW_OK) return endProgram("GLEW nije uspeo da se inicijalizuje.");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    unsigned int shader = createShader("basic.vert", "basic.frag");
    unsigned int personShader = createShader("person.vert", "person.frag");
    unsigned int fontShader = createShader("font.vert", "font.frag");

    unsigned int leftTexture = loadImageToTexture("Resources/left_side.jpg");
    glBindTexture(GL_TEXTURE_2D, leftTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int rightTexture = loadImageToTexture("Resources/right_side.jpg");
    glBindTexture(GL_TEXTURE_2D, rightTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int personStanding = loadImageToTexture("Resources/standing.png");
    glBindTexture(GL_TEXTURE_2D, personStanding);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    unsigned int personWalk[6];
    for (int i = 0; i < 6; i++) {
        std::string filename = "Resources/walk_00" + std::to_string(i) + ".png";
        personWalk[i] = loadImageToTexture(filename.c_str());
        glBindTexture(GL_TEXTURE_2D, personWalk[i]);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    fontTexture = loadImageToTexture("Resources/font.png");
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    cursorPropeller = loadImageToCursor("Resources/propeller_on.png");
    cursorNormal = loadImageToCursor("Resources/propeller_off.png");

    unsigned int indexTexture = loadImageToTexture("Resources/index.png");
    glBindTexture(GL_TEXTURE_2D, indexTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int keybindsTexture = loadImageToTexture("Resources/keybinds.png");
    glBindTexture(GL_TEXTURE_2D, keybindsTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    float vertices[] = {
        -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  1.0f, 1.0f
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    initButtons();

    double lastFrameTime = glfwGetTime();
    liftY = -1.0f + (liftFloor * 2.0f / 8.0f);

    while (!glfwWindowShouldClose(window))
    {
        //FPS Podesavanja
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - lastFrameTime;

        if (deltaTime < FRAME_TIME) {
            continue;
        }

        lastFrameTime = currentTime;
        movePersonLeftRight(window, deltaTime);
        updateLift(deltaTime, buttons);
        handlePersonEnterExit();

        // Pozicija osobe dok je u liftu
        if (personInLift) {
            personY = liftY + 0.04f;  // Malo iznad dna lifta
            personX = 0.925f;  // U centru lifta
        }

        glClear(GL_COLOR_BUFFER_BIT);

        // Leva polovina - metal tekstura
        drawTexturedRect(shader, VAO, leftTexture, -0.5f, 0.0f, 1.0f, 2.0f);

        // Centralna linija
        drawRect(shader, VAO, 0.0f, 0.0f, 0.03f, 2.0f, 0.3f, 0.3f, 0.3f, 1.0f);

        // Desna polovina
        drawTexturedRect(shader, VAO, rightTexture, 0.5f, 0.0f, 1.0f, 2.0f);

        // Info tekstura
        drawTexturedRect(shader, VAO, indexTexture, -0.83f, 0.9f, 0.3f, 0.15f);

        // Keybinds tekstura (donji levi ugao)
        drawTexturedRect(shader, VAO, keybindsTexture, -0.77f, -0.94f, 0.45f, 0.085f);

        // Rupa gde ide lift
        drawRect(shader, VAO, 0.925f, 0.0f, 0.15f, 2.0f, 0.15f, 0.15f, 0.18f, 1.0f);

        // Lanac 
        drawRect(shader, VAO, 0.925f, 0.0f, 0.008f, 2.0f, 0.6f, 0.6f, 0.7f, 1.0f);

        // Donja ivica
        drawRect(shader, VAO, 0.425f, -1.0f, 0.85f, 0.03f, 0.3f, 0.3f, 0.3f, 1.0f);

        // 7 linija izmedju
        for (int i = 1; i <= 7; i++) {
            float y = -1.0f + (i * 2.0f / 8.0f);
            drawRect(shader, VAO, 0.425f, y, 0.85f, 0.03f, 0.3f, 0.3f, 0.3f, 1.0f);
        }

        // Gornja ivica
        drawRect(shader, VAO, 0.425f, 1.0f, 0.85f, 0.03f, 0.3f, 0.3f, 0.3f, 1.0f);

        // Lift kabina
        float liftCenterY = getLiftCenterY();

        // Okvir kabine
        drawRect(shader, VAO, 0.925f, liftCenterY, 0.16f, 0.26f, 0.4f, 0.4f, 0.45f, 1.0f);

        // Unutrasnjost kabine (svetlija)
        drawRect(shader, VAO, 0.925f, liftCenterY, 0.15f, 0.25f, 0.7f, 0.7f, 0.75f, 1.0f);

        //Covek pomeranje
        unsigned int currentPersonTexture = isWalking ? personWalk[walkFrame] : personStanding;

        glUseProgram(personShader);
        glUniform2f(glGetUniformLocation(personShader, "uPos"), personX, personY + 0.01f);
        glUniform2f(glGetUniformLocation(personShader, "uScale"), 0.04f, 0.1f);
        glUniform1i(glGetUniformLocation(personShader, "uFlipX"), !facingRight);

        //Textura coveka
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, currentPersonTexture);
        glUniform1i(glGetUniformLocation(personShader, "uTex"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //Zid do lifta
        drawRect(shader, VAO, 0.85f, 0.0f, 0.01f, 2.0f, 0.3f, 0.3f, 0.3f, 1.0f);

        // Vrata lifta (idu u desno)
        float doorX = 0.85f + doorOffset;
        drawRect(shader, VAO, doorX + 0.075f, liftCenterY, 0.15f, 0.26f, 0.5f, 0.5f, 0.55f, 1.0f);

        // Crtanje tastera:
        for (auto& btn : buttons) {
            if (btn.floorNumber == -4) {  // VENT
                btn.pressed = ventilationOn;
            }
        }

        for (const auto& btn : buttons) {

            // Pozadina tastera
            drawRect(shader, VAO, btn.x, btn.y, btn.width, btn.height,
                0.3f, 0.3f, 0.3f, 1.0f);

            // Beli okvir ako je pritisnut
            if (btn.pressed) {
                drawRect(shader, VAO, btn.x, btn.y, btn.width + 0.008f, btn.height + 0.008f,
                    1.0f, 1.0f, 1.0f, 1.0f);
                // Ponovo nacrtaj pozadinu iznad okvira
                drawRect(shader, VAO, btn.x, btn.y, btn.width, btn.height,
                    0.3f, 0.3f, 0.3f, 1.0f);
            }

            //Tekst na tasteru
            float textWidth = btn.label.length() * 0.03f * 0.6f;
            float textX = btn.x - textWidth / 2.0f + 0.01f;
            float textY = btn.y;

            drawText(btn.label, textX, textY, 0.03f, 1.0f, 1.0f, 1.0f, fontShader, VAO);
        }

        // Ugasi pritisak kontrolnih tastera posle kratkog vremena (osim venta)
        static float controlButtonTimer = 0.0f;
        static int buttonBeingReset = -99;  // Trenutni tastr koji resetujem

        for (auto& btn : buttons) {
            // OPEN, CLOSE, STOP
            if (btn.floorNumber >= -3 && btn.floorNumber <= -1 && btn.pressed) {
                if (buttonBeingReset == -99) {
                    buttonBeingReset = btn.floorNumber;
                    controlButtonTimer = 0.0f;
                }

                if (buttonBeingReset == btn.floorNumber) {
                    controlButtonTimer += deltaTime;
                    if (controlButtonTimer > 0.2f) {
                        btn.pressed = false;
                        buttonBeingReset = -99;
                        controlButtonTimer = 0.0f;
                    }
                }
            }
        }

        // Postavi kursor prema stanju ventilacije
        if (ventilationOn && cursorPropeller) {
            glfwSetCursor(window, cursorPropeller);
        }
        else if (cursorNormal) {
            glfwSetCursor(window, cursorNormal);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    if (cursorPropeller) glfwDestroyCursor(cursorPropeller);
    if (cursorNormal) glfwDestroyCursor(cursorNormal);
    glfwTerminate();
    return 0;
}