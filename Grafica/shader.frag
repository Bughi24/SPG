#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 TexCoords;

uniform sampler2D texture_diffuse;
// Cele 3 harti de umbre (Soare + 2 Stalpi)
uniform sampler2D shadowMapSun;
uniform sampler2D shadowMapSpot1;
uniform sampler2D shadowMapSpot2;

// Datele Soarelui
uniform vec3 lightDirSun;
uniform mat4 lightSpaceMatrixSun;

// Datele Stalpilor
uniform vec3 spotLightPos1;
uniform mat4 lightSpaceMatrixSpot1;

uniform vec3 spotLightPos2;
uniform mat4 lightSpaceMatrixSpot2;

uniform vec3 viewPos;

// Functia care calculeaza daca un pixel e in umbra sau nu
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 normal, vec3 lightDir)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    
    // Daca iese din raza de actiune a umbrelor, ignoram
    if(projCoords.z > 1.0) return 0.0;
    
    float currentDepth = projCoords.z;
    // Bias pentru a preveni "shadow acne" (dungi urate pe obiecte)
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    // PCF (Filtram umbra sa fie blurata frumos pe margini)
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    return shadow;
}

// Functia care calculeaza fizica luminii pentru un Stalp (Spotlight)
vec3 CalcSpotLight(vec3 lightPos, mat4 lightSpaceMatrix, sampler2D shadowMap, vec3 normal, vec3 fragPos) 
{
    // Directia de la pixel catre bec
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);

    // Directia din bec catre pixel
    vec3 lightToFragDir = normalize(fragPos - lightPos);

    // Conul de lumina
    float theta = dot(lightToFragDir, vec3(0.0, -1.0, 0.0)); 
    float epsilon = cos(radians(20.0)) - cos(radians(40.0));
    float intensity = clamp((theta - cos(radians(40.0))) / epsilon, 0.0, 1.0);

    // Lumina scade cu cat te departezi de stalp
    float distance = length(lightPos - fragPos);
    float attenuation = 1.0 / (1.0 + 0.02 * distance + 0.001 * (distance * distance));    

    // Verificam daca ceva blocheaza lumina stalpului
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(fragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace, shadowMap, normal, lightDir);

    vec3 lightColor = vec3(1.0, 0.9, 0.4); // Culoare galbena calda (neon de strada)
    
    // Inmultim cu 4.0 la final ca sa aiba putere sa bata pana jos pe asfalt
    return lightColor * diff * intensity * attenuation * (1.0 - shadow) * 4.0; 
}

void main()
{
    // Extragem culoarea de baza a obiectului (din poza de asfalt/iarba/etc)
    vec3 color = texture(texture_diffuse, TexCoords.xy).rgb;
    vec3 normal = normalize(Normal);
    
    // LUMINA SOARELUI
    vec3 lightDirectionSun = normalize(-lightDirSun);
    float diffSun = max(dot(normal, lightDirectionSun), 0.0);
    vec4 fragPosLightSpaceSun = lightSpaceMatrixSun * vec4(FragPos, 1.0);
    float shadowSun = ShadowCalculation(fragPosLightSpaceSun, shadowMapSun, normal, lightDirectionSun);
    
    // Scadem putin din puterea soarelui ca sa para ca se insereaza (si sa iasa stalpii in evidenta)
    vec3 sunLighting = vec3(0.25) + (1.0 - shadowSun) * diffSun * vec3(0.3);

    // LUMINA STALPILOR 
    vec3 spot1 = CalcSpotLight(spotLightPos1, lightSpaceMatrixSpot1, shadowMapSpot1, normal, FragPos);
    vec3 spot2 = CalcSpotLight(spotLightPos2, lightSpaceMatrixSpot2, shadowMapSpot2, normal, FragPos);
    
    // REZULTAT FINAL = Vopsea * (Soare + Stalp1 + Stalp2)
    vec3 result = color * (sunLighting + spot1 + spot2);
    
    FragColor = vec4(result, 1.0);
}