#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in vec3 aColor;

uniform mat4 proj;
uniform mat4 view;

out vec3 Color;

void main()
{
    gl_Position = proj * view * vec4(aPos, 1.0);
    Color = aColor;
}
