#include "engine.h"
#include <iostream>

color offFill, onFill;

Engine::Engine() : keys() {

    offFill.vec = {0.5, 0.5, 0.5, 1}; // grey
    onFill.vec = {1, 1, 0, 1}; // yellow

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

    // Set uniforms that never change
    shapeShader.use();
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    int Xoffset = 150;
    int Yoffset = 150;
    // initialize 25 "off" squares
    for (int j = 0; j < 5; ++j) {
        for (int i = 0; i < 5; ++i) {
            shapes.push_back(make_unique<Rect>(shapeShader, vec2(Xoffset, Yoffset), vec2(150,150), offFill));
            Yoffset += 175; // evenly space the squares
        }
        Yoffset = 150; // reset Yoffset so next col starts in same spot
        Xoffset += 175; // increment Xoffset to add another column
    }

    // default all start as "off"
    for (const unique_ptr<Shape>& s : shapes) {
        s->isOn = false;
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

    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Mouse position is inverted because the origin of the window is in the top left corner
    MouseY = height - MouseY; // Invert y-axis of mouse position

    // Check if mouse has been pressed
    bool mousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;

    // Change color if you click a shape
    for (const unique_ptr<Shape>& s : shapes) {
        // check for mouse release
        if (!mousePressed && mousePressedLastFrame && s->isOverlapping(vec2(MouseX, MouseY - 150))) {
            // toggle and change color
            if (!s->isOn) {
                s->toggle();
                s->setColor(onFill);
                break;
            }
            else if (s->isOn) {
                s->toggle();
                s->setColor(offFill);
                break;
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


    // This function polls for events like keyboard input and mouse movement
    // It needs to be called every frame
    // Without this function, the window will freeze and become unresponsive
    glfwPollEvents();
}

void Engine::render() {
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    shapeShader.use();

    // Render shapes
    // For each shape, call it's setUniforms() function and then call it's draw() function
    for (const unique_ptr<Shape>& s : shapes) {
        s->setUniforms();
        s->draw();
    }

    // This is glfw function call is required to display the final image on the screen
    // The front buffer contains the final image that is displayed.
    // The back buffer contains the image that is currently being rendered.
    glfwSwapBuffers(window);
}

bool Engine::shouldClose() {
    return glfwWindowShouldClose(window);
}
