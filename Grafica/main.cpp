#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 80.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

//Actualizare rezolutie
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Coordonatele pentru Skybox
float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,   -1.0f, -1.0f, -1.0f,    1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,    1.0f,  1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,   -1.0f, -1.0f, -1.0f,   -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,    1.0f, -1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,    1.0f,  1.0f, -1.0f,    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,    1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,    1.0f, -1.0f,  1.0f,   -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,    1.0f,  1.0f, -1.0f,    1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   -1.0f,  1.0f,  1.0f,   -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   -1.0f, -1.0f,  1.0f,    1.0f, -1.0f,  1.0f
};

//Coordonatele pentru Campie
float floorVertices[] = {
   
    -200.0f, -0.5f, -200.0f,      0.0f, 100.0f,
     200.0f, -0.5f, -200.0f,    100.0f, 100.0f,
     200.0f, -0.5f,  200.0f,    100.0f,   0.0f,

     200.0f, -0.5f,  200.0f,    100.0f,   0.0f,
    -200.0f, -0.5f,  200.0f,      0.0f,   0.0f,
    -200.0f, -0.5f, -200.0f,      0.0f, 100.0f
};

//Coordonatele pentru Munte
float hillVertices[] = {
    // Panta Fata
    -30.0f, -0.5f,  30.0f,       0.0f,  0.0f,
     30.0f, -0.5f,  30.0f,      10.0f,  0.0f,
      0.0f,  18.0f,  0.0f,       5.0f,  5.0f, // Varful muntelui

     // Panta Dreapta
     30.0f, -0.5f,  30.0f,      10.0f,  0.0f,
     30.0f, -0.5f, -30.0f,      10.0f, 10.0f,
     0.0f,  18.0f,  0.0f,       5.0f,  5.0f,

     // Panta Spate
     30.0f, -0.5f, -30.0f,      10.0f, 10.0f,
    -30.0f, -0.5f, -30.0f,       0.0f, 10.0f,
     0.0f,  18.0f,  0.0f,       5.0f,  5.0f,

     // Panta Stanga
    -30.0f, -0.5f, -30.0f,       0.0f, 10.0f,
    -30.0f, -0.5f,  30.0f,       0.0f,  0.0f,
     0.0f,  18.0f,  0.0f,       5.0f,  5.0f
};

void processInput(GLFWwindow* window)
{
    // Dacă apeși ESC, fereastra se închide
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Viteza camerei depinde de timpul real (deltaTime), nu de cât de rapid e calculatorul
    float cameraSpeed = 25.0f * deltaTime;

    // W și S: Mergem În Față și În Spate
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;

    // A și D: Mergem la Stânga și la Dreapta (Strafe)
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);

    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // claritate
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // claritate
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // colt
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // colt
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // colt

    return textureID;
}

unsigned int loadTexture(const char* path)
{
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
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Eroare la incarcarea texturii: " << path << std::endl;
    }
    stbi_image_free(data);
    return textureID;
}

std::vector<float> generateTrack(float radiusX, float radiusZ, float trackWidth, int segments)
{
    std::vector<float> vertices;

    float scaleLength = 40.0f;

    float scaleWidth = 1.0f;

    for (int i = 0; i < segments; i++)
    {
        float angle1 = ((float)i / segments) * 2.0f * 3.14159f;
        float angle2 = ((float)(i + 1) / segments) * 2.0f * 3.14159f;

        float rInX = radiusX - trackWidth;
        float rInZ = radiusZ - trackWidth;
        float rOutX = radiusX + trackWidth;
        float rOutZ = radiusZ + trackWidth;

        float p1x = rInX * cos(angle1);
        float p1z = rInZ * sin(angle1);
        
        float p2x = rOutX * cos(angle1);
        float p2z = rOutZ * sin(angle1);

        float p3x = rInX * cos(angle2);
        float p3z = rInZ * sin(angle2);

        float p4x = rOutX * cos(angle2);
        float p4z = rOutZ * sin(angle2);

        // Coordonate pentru textură (U, V) - repetam textura pe lungime
        float v1 = ((float)i / segments) * scaleLength;
        float v2 = ((float)(i + 1) / segments) * scaleLength;

        // Construim 2 triunghiuri (6 vertecsi) pentru aceasta felie de circuit
        // Format: X, Y, Z, U, V  (Y este -0.4f ca sa stea o idee deasupra ierbii)

        // --- MODIFICARE: Swapp-ul coordonatelor U si V ---
        // Triunghiul 1
        vertices.insert(vertices.end(), { p1x, -0.4f, p1z, 0.0f, v1 });
        vertices.insert(vertices.end(), { p2x, -0.4f, p2z, scaleWidth, v1 });
        vertices.insert(vertices.end(), { p3x, -0.4f, p3z, 0.0f, v2 });

        // Triunghiul 2
        vertices.insert(vertices.end(), { p3x, -0.4f, p3z, 0.0f, v2 });
        vertices.insert(vertices.end(), { p2x, -0.4f, p2z, scaleWidth, v1 });
        vertices.insert(vertices.end(), { p4x, -0.4f, p4z, scaleWidth, v2 });
        // --------------------------------------------------
    }

    return vertices;
}

// Coordonatele pentru o Cladire (UV-uri reparate pentru ferestre)
float buildingVertices[] = {
    // Fata spate
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  2.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  2.0f, 5.0f,
     0.5f,  0.5f, -0.5f,  2.0f, 5.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 5.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    // Fata fata
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  2.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  2.0f, 5.0f,
     0.5f,  0.5f,  0.5f,  2.0f, 5.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 5.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    // Fata stanga
    -0.5f,  0.5f,  0.5f,  2.0f, 5.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 5.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  2.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  2.0f, 5.0f,
    // Fata dreapta
     0.5f,  0.5f,  0.5f,  0.0f, 5.0f,
     0.5f,  0.5f, -0.5f,  2.0f, 5.0f,
     0.5f, -0.5f, -0.5f,  2.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  2.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 5.0f,
     // Fata jos (podea)
     -0.5f, -0.5f, -0.5f,  0.0f, 2.0f,
      0.5f, -0.5f, -0.5f,  2.0f, 2.0f,
      0.5f, -0.5f,  0.5f,  2.0f, 0.0f,
      0.5f, -0.5f,  0.5f,  2.0f, 0.0f,
     -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f,  0.0f, 2.0f,
     // Fata sus (acoperis)
     -0.5f,  0.5f, -0.5f,  0.0f, 2.0f,
      0.5f,  0.5f, -0.5f,  2.0f, 2.0f,
      0.5f,  0.5f,  0.5f,  2.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  2.0f, 0.0f,
     -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f,  0.0f, 2.0f
};

glm::vec3 buildingPositions[] = {
    // Latura de Nord (in spatele circuitului)
    glm::vec3(0.0f, 0.0f, -160.0f),
    glm::vec3(100.0f, 0.0f, -150.0f),
    glm::vec3(-100.0f, 0.0f, -150.0f),

    // Latura de Sud (in fata circuitului)
    glm::vec3(0.0f, 0.0f,  160.0f),
    glm::vec3(100.0f, 0.0f,  150.0f),
    glm::vec3(-100.0f, 0.0f,  150.0f),

    // Latura de Est (in dreapta)
    glm::vec3(130.0f, 0.0f,   60.0f),
    glm::vec3(130.0f, 0.0f,  -60.0f),

    // Latura de Vest (in stanga)
    glm::vec3(-130.0f, 0.0f,   60.0f),
    glm::vec3(-130.0f, 0.0f,  -60.0f)
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Skybox", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Eroare GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Eroare GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);

    //SKYBOX 
    Shader skyboxShader("skybox.vert", "skybox.frag");
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    std::vector<std::string> faces = {
        "texturi/right.jpg", "texturi/left.jpg", "texturi/top.jpg",
        "texturi/bottom.jpg", "texturi/front.jpg", "texturi/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    //SHADER PENTRU OBIECTE NORMALE
    Shader floorShader("shader.vert", "shader.frag");
    unsigned int grassTexture = loadTexture("texturi/grass.jpg");
    unsigned int mountainTexture = loadTexture("texturi/munte.jpg");

    // VAO PENTRU CAMPIE
    unsigned int floorVAO, floorVBO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &floorVBO);
    glBindVertexArray(floorVAO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), &floorVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // VAO PENTRU MUNTE
    unsigned int hillVAO, hillVBO;
    glGenVertexArrays(1, &hillVAO);
    glGenBuffers(1, &hillVBO);
    glBindVertexArray(hillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, hillVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(hillVertices), &hillVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    std::vector<float> trackVertices = generateTrack(80.0f, 120.0f, 8.0f, 100);
    unsigned int asphaltTexture = loadTexture("texturi/asfalt.png");

    unsigned int trackVAO, trackVBO;
    glGenVertexArrays(1, &trackVAO);
    glGenBuffers(1, &trackVBO);

    glBindVertexArray(trackVAO);
    glBindBuffer(GL_ARRAY_BUFFER, trackVBO);

    glBufferData(GL_ARRAY_BUFFER, trackVertices.size() * sizeof(float), trackVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int buildingTexture = loadTexture("texturi/cladire.jpg");

    unsigned int buildingVAO, buildingVBO;
    glGenVertexArrays(1, &buildingVAO);
    glGenBuffers(1, &buildingVBO);

    glBindVertexArray(buildingVAO);
    glBindBuffer(GL_ARRAY_BUFFER, buildingVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(buildingVertices), &buildingVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    while (!glfwWindowShouldClose(window))
    {
        // --- 1. CALCULAM DELTA TIME ---
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- 2. CITIM TASTATURA ---
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // DESENAM CERUL
        glDepthMask(GL_FALSE);
        skyboxShader.use();
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);

        // MATRICILE PENTRU OBIECTE 3D 
        floorShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);

        // Setarea camerei
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        floorShader.setMat4("projection", projection);
        floorShader.setMat4("view", view);

        // Campia
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, grassTexture);

        glm::mat4 modelFloor = glm::mat4(1.0f); 
        floorShader.setMat4("model", modelFloor);
        glBindVertexArray(floorVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Muntele
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mountainTexture);

        glm::mat4 modelHill = glm::mat4(1.0f); 
        modelHill = glm::scale(modelHill, glm::vec3(2.0f, 3.0f, 2.0f));
        floorShader.setMat4("model", modelHill);
        glBindVertexArray(hillVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, asphaltTexture);

        glm::mat4 modelTrack = glm::mat4(1.0f);
        floorShader.setMat4("model", modelTrack);

        glBindVertexArray(trackVAO);
        glDrawArrays(GL_TRIANGLES, 0, trackVertices.size() / 5);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buildingTexture);
        glBindVertexArray(buildingVAO);

        for (unsigned int i = 0; i < 10; i++)
        {
            glm::mat4 modelBuilding = glm::mat4(1.0f);

            modelBuilding = glm::translate(modelBuilding, buildingPositions[i]);

            modelBuilding = glm::translate(modelBuilding, glm::vec3(0.0f, 20.0f, 0.0f));
            
            modelBuilding = glm::scale(modelBuilding, glm::vec3(15.0f, 40.0f, 15.0f));

            floorShader.setMat4("model", modelBuilding);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteVertexArrays(1, &floorVAO);
    glDeleteBuffers(1, &floorVBO);
    glDeleteVertexArrays(1, &hillVAO);
    glDeleteBuffers(1, &hillVBO);
    glfwTerminate();

    return 0;
}