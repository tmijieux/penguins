#version 330 core
layout (location = 0) in vec3 iPos;
layout (location = 1) in vec3 iNormal;
layout (location = 2) in vec2 iTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 normal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 LightPos;

void main()
{
    mat4 viewmodel = view * model;
    vec4 viewPos = viewmodel * vec4(iPos, 1.0);
    gl_Position = proj * viewPos;

    TexCoord = iTexCoord;
    Normal = mat3(transpose(inverse(viewmodel))) * iNormal;
    // Normal = vec3(viewmodel * vec4(iNormal, 0.0));
    // Normal = iNormal;
    FragPos = viewPos.xyz;
    LightPos = vec3(view * vec4(0.0, 40.0, 0.0, 0.0));
}
