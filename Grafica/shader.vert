#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords; // FIX #1: era vec3, trebuie vec2

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords; // FIX #1: era vec3, trebuie vec2

uniform mat4 model;
uniform mat4 model_detailed; // Matricea locală a piesei (Identity pentru obiecte normale)
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Matricea finală = baza obiectului * transformarea locală a piesei
    // Pentru obiecte normale, model_detailed = mat4(1.0)
    mat4 finalModel = model * model_detailed;

    // Poziția în spațiul world
    FragPos = vec3(finalModel * vec4(aPos, 1.0));

    // FIX #3: normala transformată corect cu matricea finală combinată
    Normal = mat3(transpose(inverse(finalModel))) * aNormal;

    // FIX #1: TexCoords e acum vec2
    TexCoords = aTexCoords;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
