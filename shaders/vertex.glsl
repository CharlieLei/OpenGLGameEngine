#version 410 core

in vec3 inPosition;
in vec3 inColor;
out vec3 passColor;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    gl_Position = viewMatrix * vec4(inPosition, 1.0);
    passColor = inColor;
}