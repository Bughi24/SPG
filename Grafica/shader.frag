#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 TexCoords;  // vec3 ca in original

uniform sampler2D texture_diffuse;

// Cele 3 harti de umbre (Soare + 2 Stalpi)
uniform sampler2D shadowMapSun;
uniform sampler2D shadowMapSpot1;
uniform sampler2D shadowMapSpot2;

// Datele Soarelui
uniform vec3 lightDirSun;
uniform mat4 lightSpaceMatrixSun;

// Datele Stalpilor — adaugat spotLightDir1/2 fata de original
uniform vec3 spotLightPos1;
uniform vec3 spotLightDir1;
uniform mat4 lightSpaceMatrixSpot1;

uniform vec3 spotLightPos2;
uniform vec3 spotLightDir2;
uniform mat4 lightSpaceMatrixSpot2;

uniform vec3 viewPos;

// Functia care calculeaza daca un pixel e in umbra sau nu
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    // Daca iese din raza de actiune a umbrelor, ignoram
    if(projCoords.z > 1.0
    || projCoords.x < 0.0 || projCoords.x > 1.0
    || projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;
    float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;
    return shadow;
}

// Aceeasi semnatura ca originalul + spotDir pentru con corect
vec3 CalcSpotLight(vec3 lightPos, vec3 spotDir, mat4 lightSpaceMatrix, sampler2D shadowMap, vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 lightToFragDir = normalize(fragPos - lightPos);

    // Con: inner=30 grade (plin), outer=40 grade (fade)
    // Trebuie sa corespunda cu FOV=80 din main (FOV/2=40=outer)
    float theta = dot(lightToFragDir, normalize(spotDir));
    float epsilon = cos(radians(30.0)) - cos(radians(40.0));
    float intensity = clamp((theta - cos(radians(40.0))) / epsilon, 0.0, 1.0);

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.02 * distance + 0.001 * (distance * distance));

    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace, shadowMap, normal, lightDir);

    vec3 lightColor = vec3(1.0, 0.9, 0.4);
    return lightColor * diff * intensity * attenuation * (1.0 - shadow) * 4.0;
}

void main()
{
    // TexCoords.xy ca in original (era vec3)
    vec3 color = texture(texture_diffuse, TexCoords.xy).rgb;
    vec3 normal = normalize(Normal);

    // LUMINA SOARELUI
    vec3 lightDirectionSun = normalize(-lightDirSun);
    float diffSun = max(dot(normal, lightDirectionSun), 0.0);
    vec4 fragPosLightSpaceSun = lightSpaceMatrixSun * vec4(FragPos, 1.0);
    float shadowSun = ShadowCalculation(fragPosLightSpaceSun, shadowMapSun, normal, lightDirectionSun);
    vec3 sunLighting = vec3(0.25) + (1.0 - shadowSun) * diffSun * vec3(0.3);

    // LUMINA STALPILOR
    vec3 spot1 = CalcSpotLight(spotLightPos1, spotLightDir1, lightSpaceMatrixSpot1, shadowMapSpot1, normal, FragPos);
    vec3 spot2 = CalcSpotLight(spotLightPos2, spotLightDir2, lightSpaceMatrixSpot2, shadowMapSpot2, normal, FragPos);

    // REZULTAT FINAL
    vec3 result = color * (sunLighting + spot1 + spot2);
    FragColor = vec4(result, 1.0);
}
