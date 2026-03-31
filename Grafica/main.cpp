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

// Car
glm::vec3 carPos = glm::vec3(0.0f, 0.5f, 100.0f); 
float carAngle = 180.0f;
float carSpeed = 0.0f;
float maxSpeed = 40.0f;
float acceleration = 20.0f;
float steeringSpeed = 90.0f;

glm::vec3 carSize = glm::vec3(4.0f, 1.6f, 9.0f);

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

// Stâlpul extra
glm::vec3 extraObjectPos = glm::vec3(67.0f, -0.5f, -12.0f);

// Directiile spoturilor
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

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
};

bool CheckCollision(BoundingBox a, BoundingBox b) {
    return (a.min.x <= b.max.x && a.max.x >= b.min.x) &&
        (a.min.y <= b.max.y && a.max.y >= b.min.y) &&
        (a.min.z <= b.max.z && a.max.z >= b.min.z);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) carSpeed += acceleration * deltaTime;
    else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) carSpeed -= acceleration * deltaTime;
    else carSpeed *= 0.98f;

    if (carSpeed > maxSpeed) carSpeed = maxSpeed;
    if (carSpeed < -maxSpeed / 2) carSpeed = -maxSpeed / 2;

    if (abs(carSpeed) > 0.1f) {
        float steerDir = (carSpeed > 0) ? 1.0f : -1.0f;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) carAngle += steeringSpeed * deltaTime * steerDir;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) carAngle -= steeringSpeed * deltaTime * steerDir;
    }

    glm::vec3 direction = glm::vec3(sin(glm::radians(carAngle)), 0.0f, cos(glm::radians(carAngle)));
    glm::vec3 nextPos = carPos + direction * carSpeed * deltaTime;

    bool collided = false;
    BoundingBox carBB = { nextPos - carSize / 2.0f, nextPos + carSize / 2.0f };

    for (int i = 0; i < 10; i++) {
        BoundingBox buildingBB = {
            buildingPositions[i] - glm::vec3(7.5f, 0.0f, 7.5f),
            buildingPositions[i] + glm::vec3(7.5f, 40.0f, 7.5f)
        };
        if (CheckCollision(carBB, buildingBB)) {
            collided = true;
            carSpeed = -carSpeed * 0.5f;
            break;
        }
    }

    if (!collided) {
        for (int i = 0; i < 2; i++) {
            BoundingBox poleBB = {
                polePositions[i] - glm::vec3(0.5f, 0.0f, 0.5f),
                polePositions[i] + glm::vec3(0.5f, 20.0f, 0.5f)
            };
            if (CheckCollision(carBB, poleBB)) { collided = true; break; }
        }
    }

    if (!collided) {
        BoundingBox extraBB = {
            extraObjectPos - glm::vec3(0.8f, 0.0f, 0.8f),
            extraObjectPos + glm::vec3(0.8f, 3.0f, 0.8f)
        };
        if (CheckCollision(carBB, extraBB)) collided = true;
    }

    if (!collided) {
        BoundingBox hillBB;
        hillBB.min = glm::vec3(-60.0f, -0.5f, -60.0f);
        hillBB.max = glm::vec3(60.0f, 54.0f, 60.0f);
        if (CheckCollision(carBB, hillBB)) {
            collided = true;
            carSpeed = -carSpeed * 0.5f;
        }
    }

    if (!collided) {
        carPos = nextPos;
    }

    glm::vec3 cameraOffset = glm::vec3(-sin(glm::radians(carAngle)) * 15.0f, 6.0f, -cos(glm::radians(carAngle)) * 15.0f);
    cameraPos = carPos + cameraOffset;
    cameraFront = glm::normalize(carPos + glm::vec3(0, 1.5f, 0) - cameraPos);
}

void DrawDetailedCar(Shader& shader, unsigned int cubeVAO) {
    shader.setBool("useOverrideColor", true);
    glBindVertexArray(cubeVAO);

    // CAROSERIE 
    glm::mat4 mBody = glm::scale(glm::mat4(1.0f), glm::vec3(4.0f, 1.0f, 9.0f));
    shader.setMat4("model_detailed", mBody);
    shader.setVec3("colorOverride", glm::vec3(0.8f, 0.1f, 0.1f));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // CABINĂ 
    glm::mat4 mCabin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -0.8f));
    mCabin = glm::scale(mCabin, glm::vec3(3.2f, 0.9f, 4.5f));
    shader.setMat4("model_detailed", mCabin);
    shader.setVec3("colorOverride", glm::vec3(0.1f, 0.1f, 0.1f));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // ROȚI
    shader.setVec3("colorOverride", glm::vec3(0.05f, 0.05f, 0.05f));
    float wX = 2.2f;   // Distanța stânga-dreapta 
    float wY = -0.5f;  // La nivelul solului
    float wZ = 3.0f;   // Distanța față-spate
    glm::vec3 wScale = glm::vec3(0.6f, 1.2f, 1.2f);

    glm::vec3 wheelPositions[] = {
        glm::vec3(-wX, wY,  wZ),
        glm::vec3(wX, wY,  wZ),
        glm::vec3(-wX, wY, -wZ),
        glm::vec3(wX, wY, -wZ)
    };

    for (int i = 0; i < 4; i++) {
        glm::mat4 mWheel = glm::translate(glm::mat4(1.0f), wheelPositions[i]);
        mWheel = glm::scale(mWheel, wScale);
        shader.setMat4("model_detailed", mWheel);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // --- FARURI ---
    shader.setVec3("colorOverride", glm::vec3(1.0f, 1.0f, 0.5f));
    glm::mat4 mFarL = glm::translate(glm::mat4(1.0f), glm::vec3(-1.2f, 0.2f, 4.6f));
    shader.setMat4("model_detailed", glm::scale(mFarL, glm::vec3(0.7f, 0.5f, 0.15f)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glm::mat4 mFarR = glm::translate(glm::mat4(1.0f), glm::vec3(1.2f, 0.2f, 4.6f));
    shader.setMat4("model_detailed", glm::scale(mFarR, glm::vec3(0.7f, 0.5f, 0.15f)));
    glDrawArrays(GL_TRIANGLES, 0, 36);

    shader.setBool("useOverrideColor", false);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);

    unsigned int windowWidth = 1280;
    unsigned int windowHeight = 720;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Proiect Grafica 3D - Windowed", NULL, NULL);
    if (window == NULL) {
        std::cout << "Eroare la crearea ferestrei" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Eroare GLAD" << std::endl; return -1;
    }
    glEnable(GL_DEPTH_TEST);

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

    unsigned int grassTexture = loadTexture("texturi/grass.jpg");
    unsigned int mountainTexture = loadTexture("texturi/munte.jpg");
    unsigned int asphaltTexture = loadTexture("texturi/asfalt.png");
    unsigned int buildingTexture = loadTexture("texturi/cladire.jpg");
    unsigned int metalTexture = loadTexture("texturi/metal.jpg");

    // Câmpie
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

    // Clădiri / Stâlpi
    unsigned int buildingVAO, buildingVBO;
    glGenVertexArrays(1, &buildingVAO); glGenBuffers(1, &buildingVBO);
    glBindVertexArray(buildingVAO); glBindBuffer(GL_ARRAY_BUFFER, buildingVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buildingVertices), &buildingVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // VAO masina
    unsigned int carVAO, carVBO;
    glGenVertexArrays(1, &carVAO); glGenBuffers(1, &carVBO);
    glBindVertexArray(carVAO); glBindBuffer(GL_ARRAY_BUFFER, carVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buildingVertices), &buildingVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1); glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Shadow mapping
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

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glm::mat4 lightSpaceMatrices[3];

        glm::vec3 sunPos = -lightDirSun * 150.0f;
        glm::mat4 sunProj = glm::ortho(-200.0f, 200.0f, -200.0f, 200.0f, 1.0f, 400.0f);
        glm::mat4 sunView = glm::lookAt(sunPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrices[0] = sunProj * sunView;

        spotDirs[0] = glm::vec3(0.0f, -1.0f, 0.0f);
        spotDirs[1] = glm::vec3(0.0f, -1.0f, 0.0f);

        for (int i = 0; i < 2; i++) {
            glm::mat4 spotProj = glm::perspective(glm::radians(100.0f), 1.0f, 0.5f, 60.0f);
            glm::mat4 spotView = glm::lookAt(
                spotLightPositions[i],
                spotLightPositions[i] + spotDirs[i],
                glm::vec3(0.0f, 0.0f, 1.0f)
            );
            lightSpaceMatrices[i + 1] = spotProj * spotView;
        }

        shadowShader.use();
        shadowShader.setMat4("model_detailed", glm::mat4(1.0f));
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

        for (int lightIndex = 0; lightIndex < 3; lightIndex++) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[lightIndex]);
            glClear(GL_DEPTH_BUFFER_BIT);
            shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrices[lightIndex]);

            // Munte
            glm::mat4 modelHillShadow = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 2.0f));
            shadowShader.setMat4("model", modelHillShadow);
            glBindVertexArray(hillVAO); glDrawArrays(GL_TRIANGLES, 0, 12);

            // Clădiri
            glBindVertexArray(buildingVAO);
            for (unsigned int i = 0; i < 10; i++) {
                glm::mat4 modelBuildingShadow = glm::translate(glm::mat4(1.0f), buildingPositions[i]);
                modelBuildingShadow = glm::translate(modelBuildingShadow, glm::vec3(0.0f, 20.0f, 0.0f));
                modelBuildingShadow = glm::scale(modelBuildingShadow, glm::vec3(15.0f, 40.0f, 15.0f));
                shadowShader.setMat4("model", modelBuildingShadow);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            glm::mat4 mCarShadowBase = glm::translate(glm::mat4(1.0f), carPos);
            mCarShadowBase = glm::rotate(mCarShadowBase, glm::radians(carAngle), glm::vec3(0, 1, 0));
            shadowShader.setMat4("model", mCarShadowBase);
            DrawDetailedCar(shadowShader, carVAO);
            shadowShader.setMat4("model_detailed", glm::mat4(1.0f));

            // Stâlpi
            glBindVertexArray(buildingVAO);
            for (int i = 0; i < 2; i++) {
                float dirX = (polePositions[i].x > 0) ? 1.0f : -1.0f;

                glm::mat4 modelStalp = glm::translate(glm::mat4(1.0f), polePositions[i]);
                modelStalp = glm::translate(modelStalp, glm::vec3(0.0f, 9.5f, 0.0f));
                modelStalp = glm::scale(modelStalp, glm::vec3(0.4f, 20.0f, 0.4f));
                shadowShader.setMat4("model", modelStalp);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                glm::mat4 modelBrat = glm::translate(glm::mat4(1.0f), polePositions[i]);
                modelBrat = glm::translate(modelBrat, glm::vec3(dirX * 2.0f, 19.3f, 0.0f));
                modelBrat = glm::scale(modelBrat, glm::vec3(4.0f, 0.3f, 0.3f));
                shadowShader.setMat4("model", modelBrat);
                glDrawArrays(GL_TRIANGLES, 0, 36);

                glm::mat4 modelLampa = glm::translate(glm::mat4(1.0f), polePositions[i]);
                modelLampa = glm::translate(modelLampa, glm::vec3(dirX * 4.0f, 19.3f, 0.0f));
                modelLampa = glm::scale(modelLampa, glm::vec3(1.0f, 0.4f, 0.6f));
                shadowShader.setMat4("model", modelLampa);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }

            // Circuit
            shadowShader.setMat4("model", glm::mat4(1.0f));
            glBindVertexArray(trackVAO); glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(trackVertices.size() / 8));

            // Obiect extra
            glm::mat4 modelExtraShadow = glm::translate(glm::mat4(1.0f), extraObjectPos);
            modelExtraShadow = glm::translate(modelExtraShadow, glm::vec3(0.0f, 1.0f, 0.0f));
            modelExtraShadow = glm::scale(modelExtraShadow, glm::vec3(1.5f, 3.0f, 1.5f));
            shadowShader.setMat4("model", modelExtraShadow);
            glBindVertexArray(buildingVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // RANDAM SCENA VIZUALĂ
        int scrWidth, scrHeight;
        glfwGetFramebufferSize(window, &scrWidth, &scrHeight);

        glViewport(0, 0, scrWidth, scrHeight);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Cer
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

        // Shader principal
        floorShader.use();
     
        floorShader.setMat4("model_detailed", glm::mat4(1.0f));
        floorShader.setBool("useOverrideColor", false);
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

        glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, depthMap[0]);
        glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, depthMap[1]);
        glActiveTexture(GL_TEXTURE3); glBindTexture(GL_TEXTURE_2D, depthMap[2]);
        glActiveTexture(GL_TEXTURE0);

        // Câmpie
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        floorShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(floorVAO); glDrawArrays(GL_TRIANGLES, 0, 6);

        // Munte
        glBindTexture(GL_TEXTURE_2D, mountainTexture);
        glm::mat4 modelHill = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, 2.0f));
        floorShader.setMat4("model", modelHill);
        glBindVertexArray(hillVAO); glDrawArrays(GL_TRIANGLES, 0, 12);

        // Masina
        glm::mat4 mCarBase = glm::translate(glm::mat4(1.0f), carPos);
        mCarBase = glm::rotate(mCarBase, glm::radians(carAngle), glm::vec3(0, 1, 0));
        floorShader.setMat4("model", mCarBase);
        DrawDetailedCar(floorShader, carVAO);
        floorShader.setMat4("model_detailed", glm::mat4(1.0f));
        floorShader.setBool("useOverrideColor", false);

        // Asfalt
        glBindTexture(GL_TEXTURE_2D, asphaltTexture);
        floorShader.setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(trackVAO); glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(trackVertices.size() / 8));

        // Clădiri
        glBindTexture(GL_TEXTURE_2D, buildingTexture);
        glBindVertexArray(buildingVAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 modelBuilding = glm::translate(glm::mat4(1.0f), buildingPositions[i]);
            modelBuilding = glm::translate(modelBuilding, glm::vec3(0.0f, 20.0f, 0.0f));
            modelBuilding = glm::scale(modelBuilding, glm::vec3(15.0f, 40.0f, 15.0f));
            floorShader.setMat4("model", modelBuilding);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Stâlpi
        glBindTexture(GL_TEXTURE_2D, asphaltTexture);
        for (int i = 0; i < 2; i++) {
            float dirX = (polePositions[i].x > 0) ? 1.0f : -1.0f;

            glm::mat4 modelStalp = glm::translate(glm::mat4(1.0f), polePositions[i]);
            modelStalp = glm::translate(modelStalp, glm::vec3(0.0f, 9.5f, 0.0f));
            modelStalp = glm::scale(modelStalp, glm::vec3(0.4f, 20.0f, 0.4f));
            floorShader.setMat4("model", modelStalp);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 modelBrat = glm::translate(glm::mat4(1.0f), polePositions[i]);
            modelBrat = glm::translate(modelBrat, glm::vec3(dirX * 2.0f, 19.3f, 0.0f));
            modelBrat = glm::scale(modelBrat, glm::vec3(4.0f, 0.3f, 0.3f));
            floorShader.setMat4("model", modelBrat);
            glDrawArrays(GL_TRIANGLES, 0, 36);

            glm::mat4 modelLampa = glm::translate(glm::mat4(1.0f), polePositions[i]);
            modelLampa = glm::translate(modelLampa, glm::vec3(dirX * 4.0f, 19.3f, 0.0f));
            modelLampa = glm::scale(modelLampa, glm::vec3(1.0f, 0.4f, 0.6f));
            floorShader.setMat4("model", modelLampa);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Obiect extra
        glBindTexture(GL_TEXTURE_2D, metalTexture);
        glm::mat4 modelExtra = glm::translate(glm::mat4(1.0f), extraObjectPos);
        modelExtra = glm::translate(modelExtra, glm::vec3(0.0f, 1.0f, 0.0f));
        modelExtra = glm::scale(modelExtra, glm::vec3(1.5f, 3.0f, 1.5f));
        floorShader.setMat4("model", modelExtra);
        glBindVertexArray(buildingVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Curățare
    glDeleteVertexArrays(1, &skyboxVAO);  glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &floorVAO);   glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &hillVAO);    glDeleteBuffers(1, &hillVBO);
    glDeleteVertexArrays(1, &trackVAO);   glDeleteBuffers(1, &trackVBO);
    glDeleteVertexArrays(1, &buildingVAO); glDeleteBuffers(1, &buildingVBO);
    glDeleteVertexArrays(1, &carVAO);     glDeleteBuffers(1, &carVBO);
    glDeleteFramebuffers(3, depthMapFBO);
    glDeleteTextures(3, depthMap);
    glfwTerminate();
    return 0;
}