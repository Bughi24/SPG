#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec3 TexCoords;

uniform mat4 model;
uniform mat4 model_detailed; // Nou: Matricea locală a piesei
uniform mat4 view;
uniform mat4 projection;

void main() {
    // Calculăm matricea finală de model prin combinare
    // Pentru obiecte normale, model_detailed trebuie trimisă ca Identity Matrix (mat4(1.0))
    mat4 finalModel = model * model_detailed;

    FragPos = vec3(finalModel * vec4(aPos, 1.0));
    
    // Corecție pentru normale (folosim matricea combinată)
    Normal = mat3(transpose(inverse(finalModel))) * aNormal;  
    
    TexCoords = aTexCoords;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}