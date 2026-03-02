#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Shader.h"
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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

    while (!glfwWindowShouldClose(window))
    {
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
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 2.0f, 80.0f), glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
        floorShader.setMat4("model", modelHill);
        glBindVertexArray(hillVAO);
        glDrawArrays(GL_TRIANGLES, 0, 12);


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