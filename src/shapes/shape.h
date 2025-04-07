#ifndef GRAPHICS_SHAPE_H
#define GRAPHICS_SHAPE_H

#include "glm/glm.hpp"
#include <vector>
#include "../shader/shader.h"
#include "../util/color.h"

using std::vector, glm::vec2, glm::vec3, glm::vec4, glm::mat4, glm::translate, glm::scale, glm::rotate, glm::radians;

class Shape {
    public:
        /// @brief Construct a new Shape object
        /// @param shader The shader to use for rendering
        /// @param pos The position of the shape
        /// @param size The size of the shape
        /// @param color The color of the shape
        Shape(Shader& shader, vec2 pos, glm::vec2 size, struct color color);

        mutable bool isOn;

        /// @brief Copy constructor for Shape
        Shape(Shape const& other);

        /// @brief Destroy the Shape object
        virtual ~Shape() = default;

        // --------------------------------------------------------
        // Initialization functions
        // --------------------------------------------------------

        /// @brief Initializes the VAO.
        /// @details This function is called in the derived classes' constructor.
        unsigned int initVAO();

        /// @brief Initializes the VBO.
        /// @details This function is called in the derived classes' constructor.
        void initVBO();

        /// @brief Initializes the EBO.
        /// @details This function is called in the derived classes' constructor.
        /// @details If function has no indices, pass nullptr for indices and 0 for indexCount.
        void initEBO();

        // --------------------------------------------------------
        // Getters
        // --------------------------------------------------------
        // Position/Movement Functions
        float getPosX() const;
        float getPosY() const;
        vec2 getPos() const;


        virtual float getLeft() const = 0;
        virtual float getRight() const = 0;
        virtual float getTop() const = 0;
        virtual float getBottom() const = 0;

        // Color Functions
        vec4 getColor4() const;
        vec3 getColor3() const;
        float getRed() const;
        float getGreen() const;
        float getBlue() const;
        float getOpacity() const;

        // Size Functions
        vec2 getSize() const;

        // --------------------------------------------------------
        // Setters
        // --------------------------------------------------------

        // Position
        void setPos(vec2 pos);
        void setPosX(float x);
        void setPosY(float y);

        // Movement Setters (add/sub to current value)
        void move(vec2 offset);
        void moveX(float x);
        void moveY(float y);

        // Size
        void setSize(vec2 size);
        void setSizeX(float x);
        void setSizeY(float y);

        // Color
        void setColor(color color);
        void setColor(vec4 color);
        void setColor(vec3 color);
        void setRed(float r);
        void setGreen(float g);
        void setBlue(float b);
        void setOpacity(float a);

        // --------------------------------------------------------
        // Collision functions
        // --------------------------------------------------------
        virtual bool isOverlapping(const vec2& point) const;


        // Toggle isOn
        void toggle() const;

        //virtual void toggle();

        // --------------------------------------------------------
        // Drawing functions
        // --------------------------------------------------------

        /// @brief Sets the uniform variables from members, and calls the virtual draw function
        void setUniforms() const;

        /// @brief Pure virtual function to draw the shape.
        virtual void draw() const = 0;

protected:
        /// @brief Shader used to draw all abstract shapes.
        Shader & shader;

        /// @brief The position of the shape
        vec2 pos;

        // @brief A 2D vector representing the size of the shape
        vec2 size;

        /// @brief The rotation of the shape in degrees
        float rotation = 0.0f;

        /// @brief The VAO of the shape
        struct color color;

        /// @brief The Vertex Array Object, Vertex Buffer Object, and Element Buffer Object of the shape.
        unsigned int VAO, VBO, EBO;

        /// @brief The vertices of the shape
        vector<float> vertices;

        /// @brief The indices of the shape
        vector<unsigned int> indices;
};

#endif //GRAPHICS_SHAPE_H
