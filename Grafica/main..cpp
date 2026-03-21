#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 80.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float roll = 0.0f;

float yaw = -90.0f;
float pitch = 0.0f;
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;

// Directia Soarelui
glm::vec3 lightDirSun = glm::normalize(glm::vec3(-0.5f, -0.5f, -0.3f));


float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,  -1.0f, -1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f,  1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f, -1.0f, -1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, -1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f,  1.0f, -1.0f,   1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, -1.0f,  1.0f,  -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f,  1.0f, -1.0f,   1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,  -1.0f,  1.0f,  1.0f,  -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,  -1.0f, -1.0f,  1.0f,   1.0f, -1.0f,  1.0f
};

// Px, Py, Pz,   Nx, Ny, Nz,   U, V
float floorVertices[] = {
    -200.0f, -0.5f, -200.0f,  0.0f, 1.0f, 0.0f,   0.0f, 100.0f,
     200.0f, -0.5f, -200.0f,  0.0f, 1.0f, 0.0f, 100.0f, 100.0f,
     200.0f, -0.5f,  200.0f,  0.0f, 1.0f, 0.0f, 100.0f,   0.0f,
     200.0f, -0.5f,  200.0f,  0.0f, 1.0f, 0.0f, 100.0f,   0.0f,
    -200.0f, -0.5f,  200.0f,  0.0f, 1.0f, 0.0f,   0.0f,   0.0f,
    -200.0f, -0.5f, -200.0f,  0.0f, 1.0f, 0.0f,   0.0f, 100.0f
};

float hillVertices[] = {
    -30.0f, -0.5f,  30.0f,  0.0f, 0.5145f, 0.8575f,  0.0f, 0.0f,
     30.0f, -0.5f,  30.0f,  0.0f, 0.5145f, 0.8575f, 10.0f, 0.0f,
     0.0f,  18.0f,   0.0f,  0.0f, 0.5145f, 0.8575f,  5.0f, 5.0f,
     30.0f, -0.5f,  30.0f,  0.8575f, 0.5145f, 0.0f, 10.0f, 0.0f,
     30.0f, -0.5f, -30.0f,  0.8575f, 0.5145f, 0.0f, 10.0f, 10.0f,
     0.0f,  18.0f,   0.0f,  0.8575f, 0.5145f, 0.0f,  5.0f, 5.0f,
     30.0f, -0.5f, -30.0f,  0.0f, 0.5145f, -0.8575f, 10.0f, 10.0f,
    -30.0f, -0.5f, -30.0f,  0.0f, 0.5145f, -0.8575f,  0.0f, 10.0f,
     0.0f,  18.0f,   0.0f,  0.0f, 0.5145f, -0.8575f,  5.0f, 5.0f,
    -30.0f, -0.5f, -30.0f, -0.8575f, 0.5145f, 0.0f,  0.0f, 10.0f,
    -30.0f, -0.5f,  30.0f, -0.8575f, 0.5145f, 0.0f,  0.0f, 0.0f,
     0.0f,  18.0f,   0.0f, -0.8575f, 0.5145f, 0.0f,  5.0f, 5.0f
};

float buildingVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  2.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  2.0f, 5.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  2.0f, 5.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 5.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   2.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   2.0f, 5.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   2.0f, 5.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 5.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   2.0f, 5.0f,
    -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 5.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   2.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,   2.0f, 5.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 5.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   2.0f, 5.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   2.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,   2.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,   0.0f, 5.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 2.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  2.0f, 2.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  2.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  2.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f, 0.0f,  0.0f, 2.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 2.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   2.0f, 2.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   2.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   2.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,   0.0f, 2.0f
};

// LISTE DE OBIECTE
glm::vec3 buildingPositions[] = {
    glm::vec3(0.0f, 0.0f, -160.0f), glm::vec3(100.0f, 0.0f, -150.0f), glm::vec3(-100.0f, 0.0f, -150.0f),
    glm::vec3(0.0f, 0.0f,  160.0f), glm::vec3(100.0f, 0.0f,  150.0f), glm::vec3(-100.0f, 0.0f,  150.0f),
    glm::vec3(130.0f, 0.0f,   60.0f), glm::vec3(130.0f, 0.0f,  -60.0f),
    glm::vec3(-130.0f, 0.0f,   60.0f), glm::vec3(-130.0f, 0.0f,  -60.0f)
};

// Stalpii si becurile lor
glm::vec3 polePositions[] = {
    glm::vec3(65.0f, 0.0f,  0.0f),
    glm::vec3(-65.0f, 0.0f,  0.0f)
};

glm::vec3 spotLightPositions[] = {
    glm::vec3(69.0f, 19.0f,  0.0f),
    glm::vec3(-69.0f, 19.0f,  0.0f)
};

// --- ADAUGAT: obiect extra sub lampa dreapta ---
// Pozitia e direct sub spotLightPositions[0] = (69, 19, 0)
// Il punem la (75, 0, 0) ca sa fie vizibil si sa primeasca umbra pilonului
// Obiect extra langa baza pilonului drept (polul e la x=65)
// Il punem la x=68, z=3 ca sa fie vizibil si sa primeasca umbra pilonului
glm::vec3 extraObjectPos = glm::vec3(68.0f, 0.0f, 3.0f);

// Directiile spoturilor (calculate o data, folosite si in shader)
glm::vec3 spotDirs[2];

void updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
    glm::vec3 normalUp = glm::normalize(glm::cross(cameraRight, cameraFront));

    glm::mat4 rollMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(roll), cameraFront);
    cameraUp = glm::mat3(rollMatrix) * normalUp;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse) { lastX = xpos; lastY = ypos; firstMouse = false; }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos; lastY = ypos;
    xoffset *= 0.1f; yoffset *= 0.1f;
    yaw += xoffset; pitch += yoffset;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    updateCameraVectors();
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 25.0f * deltaTime;
    float rollSpeed = 50.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cameraPos -= cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) roll -= rollSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) roll += rollSpeed;
    updateCameraVectors();
}

unsigned int loadCubemap(std::vector<std::string> faces) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return textureID;
}

unsigned int loadTexture(const char* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Eroare la incarcarea texturii: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

std::vector<float> generateTrack(float radiusX, float radiusZ, float trackWidth, int segments) {
    std::vector<float> vertices;
    float scaleLength = 40.0f;
    float scaleWidth = 1.0f;

    for (int i = 0; i < segments; i++) {
        float angle1 = ((float)i / segments) * 2.0f * 3.14159f;
        float angle2 = ((float)(i + 1) / segments) * 2.0f * 3.14159f;
        float rInX = radiusX - trackWidth; float rInZ = radiusZ - trackWidth;
        float rOutX = radiusX + trackWidth; float rOutZ = radiusZ + trackWidth;

        float p1x = rInX * cos(angle1); float p1z = rInZ * sin(angle1);
        float p2x = rOutX * cos(angle1); float p2z = rOutZ * sin(angle1);
        float p3x = rInX * cos(angle2); float p3z = rInZ * sin(angle2);
        float p4x = rOutX * cos(angle2); float p4z = rOutZ * sin(angle2);

        float v1 = ((float)i / segments) * scaleLength;
        float v2 = ((float)(i + 1) / segments) * scaleLength;

        vertices.insert(vertices.end(), { p1x, -0.4f, p1z, 0.0f, 1.0f, 0.0f, 0.0f, v1 });
        vertices.insert(vertices.end(), { p2x, -0.4f, p2z, 0.0f, 1.0f, 0.0f, scaleWidth, v1 });
        vertices.insert(vertices.end(), { p3x, -0.4f, p3z, 0.0f, 1.0f, 0.0f, 0.0f, v2 });

        vertices.insert(vertices.end(), { p3x, -0.4f, p3z, 0.0f, 1.0f, 0.0f, 0.0f, v2 });
        vertices.insert(vertices.end(), { p2x, -0.4f, p2z, 0.0f, 1.0f, 0.0f, scaleWidth, v1 });
        vertices.insert(vertices.end(), { p4x, -0.4f, p4z, 0.0f, 1.0f, 0.0f, scaleWidth, v2 });
    }
    return vertices;
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // SETUP FULL SCREEN
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Proiect Grafica 3D", primaryMonitor, NULL);
    if (window == NULL) {
        std::cout << "Eroare GLFW" << std::endl;
        glfwTerminate(); return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Eroare GLAD" << std::endl; return -1;
    }
    glEnable(GL_DEPTH_TEST);

    // SHADERE
    Shader skyboxShader("skybox.vert", "skybox.frag");
    Shader floorShader("shader.vert", "shader.frag");
    Shader shadowShader("shadow_depth.vert", "shadow_depth.frag");

    floorShader.use();
    floorShader.setInt("texture_diffuse", 0);
    floorShader.setInt("shadowMapSun", 1);
    floorShader.setInt("shadowMapSpot1", 2);
    floorShader.setInt("shadowMapSpot2", 3);

    // SKYBOX VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO); glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO); glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces = {
        "texturi/right.jpg", "texturi/left.jpg", "texturi/top.jpg",
        "texturi/bottom.jpg", "texturi/front.jpg", "texturi/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // TEXTURI
    unsigned int grassTexture = loadTexture("texturi/grass.jpg");
    unsigned int mountainTexture = loadTexture("texturi/munte.jpg");
    unsigned int asphaltTexture = loadTexture("texturi/asfalt.png");
    unsigned int buildingTexture = loadTexture("texturi/cladire.jpg");
    unsigned int metalTexture = loadTexture("texturi/metal.jpg");

    // SETUP VAO-uri

    // Campie
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO); glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO); glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Munte
    unsigned int hillVAO, hillVBO;
    glGenVertexArrays(1, &hillVAO); glGenBuffers(1, &hillVBO);
    glBindVertexArray(hillVAO); glBindBuffer(GL_ARRAY_BUFFER, hillVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hillVertices), &hillVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Asfalt
    std::vector<float> trackVertices = generateTrack(80.0f, 120.0f, 8.0f, 100);
    unsigned int trackVAO, trackVBO;
    glGenVertexArrays(1, &trackVAO); glGenBuffers(1, &trackVBO);
    glBindVertexArray(trackVAO); glBindBuffer(GL_ARRAY_BUFFER, trackVBO);
    glBufferData(GL_ARRAY_BUFFER, trackVertices.size() * sizeof(float), trackVertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Cladiri / Stalpi
    unsigned int buildingVAO, buildingVBO;
    glGenVertexArrays(1, &buildingVAO); glGenBuffers(1, &buildingVBO);
    glBindVertexArray(buildingVAO); glBindBuffer(GL_ARRAY_BUFFER, buildingVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buildingVertices), &buildingVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // SETUP SHADOW MAPPING MULTIPLU
    const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
    unsigned int depthMapFBO[3];
    unsigned int depthMap[3];

    glGenFramebuffers(3, depthMapFBO);
    glGenTextures(3, depthMap);

    for (int i = 0; i < 3; i++) {
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // BUCLA DE RANDARE
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // CALCULE MATRICI LUMINI
        glm::mat4 lightSpaceMatrices[3];

        // SOARE
        glm::vec3 sunPos = -lightDirSun * 150.0f;
        glm::mat4 sunProj = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, 1.0f, 400.0f);
        glm::mat4 sunView = glm::lookAt(sunPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrices[0] = sunProj * sunView;

        // SPOTURI: spot0 tinteste exact obiectul extra, spot1 drept in jos
        // Spot 0 : tintim centrul obiectului extra
        spotDirs[0] = glm::normalize(
            (extraObjectPos + glm::vec3(0.0f, 1.0f, 0.0f)) - spotLightPositions[0]
        );
        // Spot 1: drept in jos ca originalul
        spotDirs[1] = glm::vec3(0.0f, -1.0f, 0.0f);

        for (int i = 0; i < 2; i++) {
            // FOV = 2 * outer_angle din shader (outer=40 grade) = 80 grade
            // Asa shadow map acopera exact conul de lumina fara sa piarda umbre
            glm::mat4 spotProj = glm::perspective(glm::radians(80.0f), 1.0f, 0.5f, 60.0f);
            glm::mat4 spotView = glm::lookAt(
                spotLightPositions[i],
                spotLightPositions[i] + spotDirs[i],
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
            lightSpaceMatrices[i + 1] = spotProj * spotView;
        }

        // RANDAM CELE 3 HARTI DE UMBRE
        shadowShader.use();
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        for (int lightIndex = 0; lightIndex < 3; lightIndex++) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[lightIndex]);
            glClear(GL_DEPTH_BUFFER_BIT);
            shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrices[lightIndex]);

            // Munte
            glm::mat4 modelHillShadow = glm::mat4(1.0f);
            modelHillShadow = glm::scale(modelHillShadow, glm::vec3(2.0f, 3.0f, 2.0f));
            shadowShader.setMat4("model", modelHillShadow);
            glBindVertexArray(hillVAO); glDrawArrays(GL_TRIANGLES, 0, 12);

            // Cladiri
            glBindVertexArray(buildingVAO);
            for (unsigned int i = 0; i < 10; i++) {
                glm::mat4 modelBuildingShadow = glm::mat4(1.0f);
                modelBuildingShadow = glm::translate(modelBuildingShadow, buildingPositions[i]);
                modelBuildingShadow = glm::translate(modelBuildingShadow, glm::vec3(0.0f, 20.0f, 0.0f));
                modelBuildingShadow = glm::scale(modelBuildingShadow, glm::vec3(15.0f, 40.0f, 15.0f));
                shadowShader.setMat4("model", modelBuildingShadow);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // Stalpi
            for (int i = 0; i < 2; i++) {
                float dirX = (polePositions[i].x > 0) ? 1.0f : -1.0f;

                glm::mat4 modelStalp = glm::mat4(1.0f);
                modelStalp = glm::translate(modelStalp, polePositions[i]);
                modelStalp = glm::translate(modelStalp, glm::vec3(0.0f, 9.5f, 0.0f));
                modelStalp = glm::scale(modelStalp, glm::vec3(0.4f, 20.0f, 0.4f));
                shadowShader.setMat4("model", modelStalp);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                glm::mat4 modelBrat = glm::mat4(1.0f);
                modelBrat = glm::translate(modelBrat, polePositions[i]);
                modelBrat = glm::translate(modelBrat, glm::vec3(dirX * 2.0f, 19.3f, 0.0f));
                modelBrat = glm::scale(modelBrat, glm::vec3(4.0f, 0.3f, 0.3f));
                shadowShader.setMat4("model", modelBrat);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                glm::mat4 modelLampa = glm::mat4(1.0f);
                modelLampa = glm::translate(modelLampa, polePositions[i]);
                modelLampa = glm::translate(modelLampa, glm::vec3(dirX * 4.0f, 19.3f, 0.0f));
                modelLampa = glm::scale(modelLampa, glm::vec3(1.0f, 0.4f, 0.6f));
                shadowShader.setMat4("model", modelLampa);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // Circuit
            glm::mat4 modelTrackShadow = glm::mat4(1.0f);
            shadowShader.setMat4("model", modelTrackShadow);
            glBindVertexArray(trackVAO); glDrawArrays(GL_TRIANGLES, 0, trackVertices.size() / 8);

            glm::mat4 modelExtraShadow = glm::mat4(1.0f);
            modelExtraShadow = glm::translate(modelExtraShadow, extraObjectPos);
            modelExtraShadow = glm::translate(modelExtraShadow, glm::vec3(0.0f, 1.0f, 0.0f));
            modelExtraShadow = glm::scale(modelExtraShadow, glm::vec3(1.5f, 3.0f, 1.5f));
            shadowShader.setMat4("model", modelExtraShadow);
            glBindVertexArray(buildingVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // RANDAM SCENA VIZUALA
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);

        glViewport(0, 0, scrWidth, scrHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // CERUL
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_FALSE);
        skyboxShader.use();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)scrWidth / (float)scrHeight, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 viewSkybox = glm::mat4(glm::mat3(view));
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("view", viewSkybox);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        // SHADERUL PRINCIPAL
        floorShader.use();
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);
        floorShader.setVec3("viewPos", cameraPos);

        floorShader.setMat4("lightSpaceMatrixSun", lightSpaceMatrices[0]);
        floorShader.setVec3("lightDirSun", lightDirSun);

        floorShader.setMat4("lightSpaceMatrixSpot1", lightSpaceMatrices[1]);
        floorShader.setVec3("spotLightPos1", spotLightPositions[0]);
        floorShader.setVec3("spotLightDir1", spotDirs[0]); 

        floorShader.setMat4("lightSpaceMatrixSpot2", lightSpaceMatrices[2]);
        floorShader.setVec3("spotLightPos2", spotLightPositions[1]);
        floorShader.setVec3("spotLightDir2", spotDirs[1]);  

        // TEXTURI UMBRE
        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, depthMap[0]);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, depthMap[1]);
        glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, depthMap[2]);
        glActiveTexture(GL_TEXTURE0);

        // DESENAM OBIECTELE

        // Campie
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glm::mat4 modelFloor = glm::mat4(1.0f);
        floorShader.setMat4("model", modelFloor);
        glBindVertexArray(floorVAO); glDrawArrays(GL_TRIANGLES, 0, 6);

        // Munte
        glBindTexture(GL_TEXTURE_2D, mountainTexture);
        glm::mat4 modelHill = glm::mat4(1.0f);
        modelHill = glm::scale(modelHill, glm::vec3(2.0f, 3.0f, 2.0f));
        floorShader.setMat4("model", modelHill);
        glBindVertexArray(hillVAO); glDrawArrays(GL_TRIANGLES, 0, 12);

        // Asfalt
        glBindTexture(GL_TEXTURE_2D, asphaltTexture);
        glm::mat4 modelTrack = glm::mat4(1.0f);
        floorShader.setMat4("model", modelTrack);
        glBindVertexArray(trackVAO); glDrawArrays(GL_TRIANGLES, 0, trackVertices.size() / 8);

        // Cladiri
        glBindTexture(GL_TEXTURE_2D, buildingTexture);
        glBindVertexArray(buildingVAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 modelBuilding = glm::mat4(1.0f);
            modelBuilding = glm::translate(modelBuilding, buildingPositions[i]);
            modelBuilding = glm::translate(modelBuilding, glm::vec3(0.0f, 20.0f, 0.0f));
            modelBuilding = glm::scale(modelBuilding, glm::vec3(15.0f, 40.0f, 15.0f));
            floorShader.setMat4("model", modelBuilding);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Stalpi de iluminat
        glBindTexture(GL_TEXTURE_2D, asphaltTexture);
        for (int i = 0; i < 2; i++) {
            float dirX = (polePositions[i].x > 0) ? 1.0f : -1.0f;

            glm::mat4 modelStalp = glm::mat4(1.0f);
            modelStalp = glm::translate(modelStalp, polePositions[i]);
            modelStalp = glm::translate(modelStalp, glm::vec3(0.0f, 9.5f, 0.0f));
            modelStalp = glm::scale(modelStalp, glm::vec3(0.4f, 20.0f, 0.4f));
            floorShader.setMat4("model", modelStalp);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 modelBrat = glm::mat4(1.0f);
            modelBrat = glm::translate(modelBrat, polePositions[i]);
            modelBrat = glm::translate(modelBrat, glm::vec3(dirX * 2.0f, 19.3f, 0.0f));
            modelBrat = glm::scale(modelBrat, glm::vec3(4.0f, 0.3f, 0.3f));
            floorShader.setMat4("model", modelBrat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 modelLampa = glm::mat4(1.0f);
            modelLampa = glm::translate(modelLampa, polePositions[i]);
            modelLampa = glm::translate(modelLampa, glm::vec3(dirX * 4.0f, 19.3f, 0.0f));
            modelLampa = glm::scale(modelLampa, glm::vec3(1.0f, 0.4f, 0.6f));
            floorShader.setMat4("model", modelLampa);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glBindTexture(GL_TEXTURE_2D, metalTexture);
        glm::mat4 modelExtra = glm::mat4(1.0f);
        modelExtra = glm::translate(modelExtra, extraObjectPos);
        modelExtra = glm::translate(modelExtra, glm::vec3(0.0f, 1.0f, 0.0f));
        modelExtra = glm::scale(modelExtra, glm::vec3(1.5f, 3.0f, 1.5f));
        floorShader.setMat4("model", modelExtra);
        glBindVertexArray(buildingVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // CURATARE
    glDeleteVertexArrays(1, &skyboxVAO); glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &floorVAO);  glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &hillVAO);   glDeleteBuffers(1, &hillVBO);
    glDeleteVertexArrays(1, &trackVAO);  glDeleteBuffers(1, &trackVBO);
    glDeleteVertexArrays(1, &buildingVAO); glDeleteBuffers(1, &buildingVBO);
    glDeleteFramebuffers(3, depthMapFBO);
    glDeleteTextures(3, depthMap);
    glfwTerminate();
    return 0;
}