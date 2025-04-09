#include "engine.h"
#include <iostream>

enum state {start, play, over};
state screen;

color offFill, onFill, hoverOff, hoverOn;

Engine::Engine() : keys() {

    offFill.vec = {0.5, 0.5, 0.5, 1}; // grey
    onFill.vec = {1, 1, 0, 1}; // yellow
    hoverOff.vec = {0, 0, 0, 1};
    hoverOn.vec = {1, 0, 0, 1};

    this->initWindow();
    this->initShaders();
    this->initShapes();

}

Engine::~Engine() {}

unsigned int Engine::initWindow(bool debug) {
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_COCOA_RETINA_FRAMEBUFFER, GLFW_FALSE);
#endif
    glfwWindowHint(GLFW_RESIZABLE, false);

    // This creates the window using GLFW.
    // It's a C function, so we have to pass it a pointer to the window variable.
    window = glfwCreateWindow(width, height, "Lights Out", nullptr, nullptr);
    if (window == nullptr) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    // This sets the OpenGL context to the window we just created.
    glfwMakeContextCurrent(window);

    // Glad is an OpenGL function loader. It loads all the OpenGL functions that are defined by the driver.
    // This is required because OpenGL is a specification, not an implementation.
    // The driver is the implementation of OpenGL that is installed on your computer.
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // OpenGL configuration
    // This defines the size of the area OpenGL should render to.
    glViewport(0, 0, width, height);
    // This enables depth testing which prevents triangles from overlapping.
    glEnable(GL_BLEND);
    // Alpha blending allows for transparent backgrounds.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(1);

    return 0;
}

void Engine::initShaders() {
    // load shader manager
    shaderManager = make_unique<ShaderManager>();

    // Load shader into shader manager and retrieve it
    shapeShader = this->shaderManager->loadShader("../res/shaders/shape.vert",
                                                  "../res/shaders/shape.frag",
                                                  nullptr, "shape");

    textShader = shaderManager->loadShader("../res/shaders/text.vert", "../res/shaders/text.frag", nullptr, "text");

    fontRenderer = make_unique<FontRenderer>(shaderManager->getShader("text"), "../res/fonts/MxPlus_IBM_BIOS.ttf", 24);

    // Set uniforms that never change
    textShader.setVector2f("vertex", vec4(100, 100, .5, .5));
    shapeShader.use();
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    int Xoffset = 100;
    int Yoffset = 100;
    // initialize 25 "off" squares
    for (int j = 0; j < 5; ++j) {
        for (int i = 0; i < 5; ++i) {
            hoverShapes.push_back(make_unique<Rect>(shapeShader, vec2(Xoffset, Yoffset), vec2(110,110), hoverOff));
            shapes.push_back(make_unique<Rect>(shapeShader, vec2(Xoffset, Yoffset), vec2(100,100), onFill));
            Yoffset += 125; // evenly space the squares
        }
        Yoffset = 100; // reset Yoffset so next col starts in same spot
        Xoffset += 125; // increment Xoffset to add another column
    }

    // default all start as "off"
    for (const unique_ptr<Shape>& s : shapes) {
        s->isOn = true;
    }
}

void Engine::processInput() {
    glfwPollEvents();

    // Set keys to true if pressed, false if released
    for (int key = 0; key < 1024; ++key) {
        if (glfwGetKey(window, key) == GLFW_PRESS)
            keys[key] = true;
        else if (glfwGetKey(window, key) == GLFW_RELEASE)
            keys[key] = false;
    }

    // Close window if escape key is pressed
    if (keys[GLFW_KEY_ESCAPE])
        glfwSetWindowShouldClose(window, true);

    // Mouse position saved to check for collisions
    glfwGetCursorPos(window, &MouseX, &MouseY);

    // Change screen from start to play when user hits s
    if (keys[GLFW_KEY_S] && screen == start) {
        screen = play;
    }

    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Mouse position is inverted because the origin of the window is in the top left corner
    MouseY = height - MouseY; // Invert y-axis of mouse position

    // Check if mouse has been pressed
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // update squares
    for (int i = 0; i < shapes.size(); ++i){
        const auto& s = shapes[i]; // current shape
        bool buttonOverlapsMouse = s->isOverlapping(vec2(MouseX, MouseY));

        // create hover affect
        if (buttonOverlapsMouse && screen == play) {
            hoverShapes[i]->setColor(hoverOn);
        }
        // remove hover affect
        if (!buttonOverlapsMouse) {
            hoverShapes[i]->setColor(hoverOff);
        }
        // check for mouse release to toggle and change color
        if (!mousePressed && mousePressedLastFrame && s->isOverlapping(vec2(MouseX, MouseY))) {
            s->toggle(offFill, onFill);
            // toggle button above
            if (i!= 4 && i!= 9 && i!= 14 && i!= 19) {
                if (i + 1 < shapes.size()) {
                    shapes[i + 1]->toggle(offFill, onFill);
                }
            }
            // toggle button below
            if (i!= 5 && i!= 10 && i!= 15 && i!= 20) {
                if (i - 1 < shapes.size()) {
                    shapes[i - 1]->toggle(offFill, onFill);
                }
            }
            // toggle button to the left
            if (i - 5 < shapes.size()) {
                shapes[i - 5]->toggle(offFill, onFill);
            }
            // toggle button to the right
            if (i + 5 < shapes.size()) {
                shapes[i + 5]->toggle(offFill, onFill);
            }
        }
    }
    // save mousePressed for next frame
    mousePressedLastFrame = mousePressed;
}
void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    int count = 0;
    for (int i = 0; i < shapes.size(); ++i) {
        if (shapes[i]->isOn) {
            count++;
        }
    }
    if (count == 0) {
        screen = over;
    }


    // This function polls for events like keyboard input and mouse movement
    // It needs to be called every frame
    // Without this function, the window will freeze and become unresponsive
    glfwPollEvents();
}

void Engine::render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    switch (screen) {
        case start: {
            string welcome = "Welcome to Lights out!";
            string start = "Press 's' to start.";
            string instructions = "Instructions:";
            string instructions1 = "The game begins with a fully lit grid.";
            string instructions2 = "Click on a light to turn it and the four";
            string instructions3 = "adjacent lights off. You win the game when";
            string instructions4 = "all the lights have been turned off.";
            this->fontRenderer->renderText(welcome, width/2 - (14 * welcome.length()), height/1.35, projection, 1.2, vec3{1, 1, 1});
            this->fontRenderer->renderText(start, width/2 - (12 * start.length()), height/1.5, projection, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions, width/2 - (12 * instructions.length()), height/2.3, projection, 1, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions1, width/2 - (8.8 * instructions1.length()), height/2.5, projection, 0.75, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions2, width/2 - (9.3 * instructions1.length()), height/2.7, projection, 0.75, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions3, width/2 - (9.8 * instructions1.length()), height/2.9, projection, 0.75, vec3{1, 1, 1});
            this->fontRenderer->renderText(instructions4, width/2 - (8.5 * instructions1.length()), height/3.15, projection, 0.75, vec3{1, 1, 1});
            break;
        }
        case play: {
            this->shapeShader.use();
            // Render shapes
            // For each shape, call it's setUniforms() function and then call it's draw() function
            for (const unique_ptr<Shape>& s : hoverShapes) {
                s->setUniforms();
                s->draw();
            }
            for (const unique_ptr<Shape>& s : shapes) {
                s->setUniforms();
                s->draw();
            }
            break;
        }
        case over: {
            string over = "You win!";
            this->fontRenderer->renderText(over, width/2 - (12 * over.length()), height/2, projection, 1, vec3{1, 1, 1});
            break;
        }
    }

    // This is glfw function call is required to display the final image on the screen
    // The front buffer contains the final image that is displayed.
    // The back buffer contains the image that is currently being rendered.
    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}
