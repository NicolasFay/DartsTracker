#version 330 core

out vec4 FragColor;

uniform vec4 shapeColor;

void main()
{
    // How does the fragment shader know about the shapeColor variable?
    // It is passed as a uniform variable from the shape's setUniform method.
    // FragColor is a built-in variable that holds the color of the fragment.
    FragColor = shapeColor;
}