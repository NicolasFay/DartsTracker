#include "engine.h"
#include <iostream>


Engine::Engine() : keys() {
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
    window = glfwCreateWindow(width, height, "spinning square!", nullptr, nullptr);
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
    shapeShader.use().setMatrix4("projection", this->PROJECTION);
}

void Engine::initShapes() {
    shapes.push_back(make_unique<Rect>(shapeShader, vec2(width / 2, height / 2), vec2(300,300), color(1,1,1,1)));
}

void Engine::processInput() {
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

    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float speed = 200.0f * deltaTime;
    if (keys[GLFW_KEY_UP]) shapes[0]->moveY(speed);
    if (keys[GLFW_KEY_DOWN]) shapes[0]->moveY(-speed);
    if (keys[GLFW_KEY_LEFT]) shapes[0]->moveX(-speed);
    if (keys[GLFW_KEY_RIGHT]) shapes[0]->moveX(speed);
}

void Engine::update() {
    // Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    float scale = (sin(currentFrame) + 1.0) / 2.0;  // Varies between 0 and 1
    shapes[0]->setSize(vec2(300 * scale, 300 * scale));

    shapes[0]->rotateShape(90.0f, 0.01);

    float phaseAngle = currentFrame * 1.5f; // You can adjust this value to make the colors change faster or slower
    float red   = sin(phaseAngle + 0) * 0.5f + 0.5f; // 0 degrees out of phase
    float green = sin(phaseAngle + 2.0f * M_PI / 3.0f) * 0.5f + 0.5f;  // 120 degrees out of phase
    float blue  = sin(phaseAngle + 4.0f * M_PI / 3.0f) * 0.5f + 0.5f;  // 240 degrees out of phase
    shapes[0]->setColor(vec4(red, green, blue, 1.0f));

    // This function polls for events like keyboard input and mouse movement
    // It needs to be called every frame
    // Without this function, the window will freeze and become unresponsive
    glfwPollEvents();
}

void Engine::render() {
    glClearColor(.2, .2, .2, 1);
    glClear(GL_COLOR_BUFFER_BIT);


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
