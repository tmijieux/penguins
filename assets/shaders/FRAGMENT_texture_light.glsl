#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 LightPos;

uniform sampler2D ourTexture;

out vec4 FragColor;

void main()
{
    vec3 lightColor = vec3(1.0, 1.0, 1.0);

    // ambient
    vec3 ambient = 0.2 * lightColor;

    // specular
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    vec3 viewDir = normalize(- FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);


    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.7 * diff * lightColor;

    // int shininess = 100;
    // float spec = pow(diff, shininess);
    // vec3 specular = 0.3 * spec * lightColor;

    vec3 texColor = texture(ourTexture, TexCoord).xyz;
    FragColor = vec4(texColor * (ambient + diffuse), 1.0);
}
