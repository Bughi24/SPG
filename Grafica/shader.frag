#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords; // FIX #1: era vec3, trebuie vec2

uniform sampler2D texture_diffuse;

// FIX #7 (din main.cpp): suport pentru culoare manuală (mașina)
uniform vec3 colorOverride;
uniform bool useOverrideColor;

// Cele 3 harti de umbre (Soare + 2 Stalpi)
uniform sampler2D shadowMapSun;
uniform sampler2D shadowMapSpot1;
uniform sampler2D shadowMapSpot2;

// Datele Soarelui
uniform vec3 lightDirSun;
uniform mat4 lightSpaceMatrixSun;

// Datele Stalpilor
uniform vec3 spotLightPos1;
uniform vec3 spotLightDir1;
uniform mat4 lightSpaceMatrixSpot1;

uniform vec3 spotLightPos2;
uniform vec3 spotLightDir2;
uniform mat4 lightSpaceMatrixSpot2;

uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z > 1.0 || projCoords.x < 0.0 || projCoords.x > 1.0 ||
       projCoords.y < 0.0 || projCoords.y > 1.0)
        return 0.0;

    float currentDepth = projCoords.z;

    // FIX #4: bias mai mic pentru a evita "peter panning"
    float bias = max(0.002 * (1.0 - dot(normal, lightDir)), 0.0005);

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

vec3 CalcSpotLight(vec3 lightPos, vec3 spotDir, mat4 lightSpaceMat, sampler2D shadowMap,
                   vec3 normal, vec3 fragPos)
{
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    vec3 lightToFragDir = normalize(fragPos - lightPos);
    float theta = dot(lightToFragDir, normalize(spotDir));
    float epsilon = cos(radians(30.0)) - cos(radians(50.0));
    float intensity = clamp((theta - cos(radians(50.0))) / epsilon, 0.0, 1.0);

    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.02 * distance + 0.001 * (distance * distance));

    vec4 fragPosLightSpace = lightSpaceMat * vec4(fragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace, shadowMap, normal, lightDir);

    vec3 lightColor = vec3(1.0, 0.9, 0.4);

    // FIX #3: eliminat 1.5f (supraexpunere + sintaxă invalidă GLSL)
    // FIX #5: 1.5f -> 1.2 (valoare standard GLSL, fără sufix f)
    return lightColor * diff * intensity * attenuation * (1.0 - shadow) * 1.2;
}

void main()
{
    // Selectia culorii: culoare manuală (mașină) sau textură (restul scenei)
    vec3 color;
    if(useOverrideColor) {
        color = colorOverride;
    } else {
        color = texture(texture_diffuse, TexCoords).rgb;
    }

    vec3 normal = normalize(Normal);

    // LUMINA SOARELUI
    vec3 lightDirectionSun = normalize(-lightDirSun);
    float diffSun = max(dot(normal, lightDirectionSun), 0.0);
    vec4 fragPosLightSpaceSun = lightSpaceMatrixSun * vec4(FragPos, 1.0);
    float shadowSun = ShadowCalculation(fragPosLightSpaceSun, shadowMapSun, normal, lightDirectionSun);

    // FIX #2: difuza soarelui mărită de la 0.4 la 0.85 pentru scenă mai luminoasă
    vec3 sunLighting = vec3(0.15) + (1.0 - shadowSun) * diffSun * vec3(0.85);

    // LUMINA STALPILOR
    vec3 spot1 = CalcSpotLight(spotLightPos1, spotLightDir1, lightSpaceMatrixSpot1, shadowMapSpot1, normal, FragPos);
    vec3 spot2 = CalcSpotLight(spotLightPos2, spotLightDir2, lightSpaceMatrixSpot2, shadowMapSpot2, normal, FragPos);

    // REZULTAT FINAL
    vec3 result = color * (sunLighting + spot1 + spot2);
    FragColor = vec4(result, 1.0);
}
