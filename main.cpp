#include <iostream>
#include <cmath>
#include <vector>
#include <iterator>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "data.h"
#include "util.h"
#include "record.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

glm::vec3 cameraPos = glm::vec3(5.0f, 5.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(-1.0f, -1.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

namespace settings
{
    static bool spinning = false;
    static bool replay = false;
    static int rotType = -1;
    static int waiting = 0;
    static double progress = 0;

    static int moodyBlues = 0;

    static CubeState state;
    static CubeRotation animation = {
            glm::mat4(1.0f),
            glm::mat4(1.0f),
            glm::mat4(1.0f),
            glm::mat4(1.0f),
            glm::mat4(1.0f),
            glm::mat4(1.0f),
            glm::mat4(1.0f),
            glm::mat4(1.0f),
    };

    static float zoom = 45.0;
    static glm::vec3 view = glm::vec3(5.0f * sqrt(3), glm::radians(35.3f), glm::radians(45.0f));
}

void updateRecord(int rotType)
{
    double time = glfwGetTime();
    _pushRecord(time, rotType, settings::state);
}

void updateView()
{
    cameraPos = viewFrom(settings::view);
    cameraFront = -glm::normalize(cameraPos);
}

void goBack()
{
    if (recordPointer <= 0)
        return;

    _goBack();

    settings::spinning = true;
    settings::rotType = inverseRotation(records[recordPointer].rotType);
    settings::progress = 0;
    settings::waiting = 512;
}

void goForward()
{
    if (recordPointer >= records.size())
        return;

    _goForward();

    settings::spinning = true;
    settings::rotType = records[recordPointer - 1].rotType;
    settings::progress = 0;
    settings::waiting = 512;
}

void replay()
{
    if (!settings::replay)
        return;

    if (settings::moodyBlues == 0)
    {
        initState(settings::state);
    }

    if (!settings::spinning && settings::waiting <= 0)
    {
        settings::spinning = true;
        settings::rotType = records[settings::moodyBlues].rotType;
        settings::progress = 0;
        settings::waiting = 1024;

        settings::moodyBlues++;

        if (settings::moodyBlues == records.size())
        {
            settings::moodyBlues = 0;
            settings::replay = false;
        }
    }
}

void processInput(GLFWwindow* window)
{
    bool shift = false;
    bool ctrl = false;
    bool goldExperience = false; // replayKey = 'R'
    int rotKey = 0;
    int directionKey = 0;
    int zoom = 0;
    int where = 0;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    SET_PRESS_FLAG(GLFW_KEY_Q, rotKey, 'Q')
    SET_PRESS_FLAG(GLFW_KEY_W, rotKey, 'W')
    SET_PRESS_FLAG(GLFW_KEY_E, rotKey, 'E')
    SET_PRESS_FLAG(GLFW_KEY_A, rotKey, 'A')
    SET_PRESS_FLAG(GLFW_KEY_S, rotKey, 'S')
    SET_PRESS_FLAG(GLFW_KEY_D, rotKey, 'D')

    SET_PRESS_FLAG(GLFW_KEY_LEFT_SHIFT, shift, true)
    SET_PRESS_FLAG(GLFW_KEY_RIGHT_SHIFT, shift, true)
    SET_PRESS_FLAG(GLFW_KEY_LEFT_CONTROL, ctrl, true)
    SET_PRESS_FLAG(GLFW_KEY_RIGHT_CONTROL, ctrl, true)

    SET_PRESS_FLAG(GLFW_KEY_MINUS, zoom, -1);
    SET_PRESS_FLAG(GLFW_KEY_EQUAL, zoom, +1);

    SET_PRESS_FLAG(GLFW_KEY_UP, directionKey, 1)
    SET_PRESS_FLAG(GLFW_KEY_DOWN, directionKey, 2)
    SET_PRESS_FLAG(GLFW_KEY_LEFT, directionKey, 3)
    SET_PRESS_FLAG(GLFW_KEY_RIGHT, directionKey, 4)

    SET_PRESS_FLAG(GLFW_KEY_R, goldExperience, true)

    SET_PRESS_FLAG(GLFW_KEY_LEFT_BRACKET, where, -1)
    SET_PRESS_FLAG(GLFW_KEY_RIGHT_BRACKET, where, 1)

    if (goldExperience)
        settings::replay = true;

    // TODO: 等待一段时间再激活 key。
    // 算了不想做了。
    // 做好了。
    if (rotKey && !settings::spinning && settings::waiting <= 0 && !settings::replay)
    {
        int rotType = keyToRotType(rotKey, shift);

        settings::spinning = true;
        settings::rotType = rotType;
        settings::progress = 0;
        settings::waiting = 512;

        updateRecord(rotType);
    }

    if (!settings::spinning && settings::waiting <= 0 && !settings::replay)
    {
        switch (where)
        {
            case 1:
                goForward();
                break;
            case -1:
                goBack();
                break;
        }
    }

    if (ctrl)
    {
        switch (directionKey)
        {
            case 1:
                settings::view[0] = between<float>(settings::view[0] - 0.01, 20, 5);
                break;
            case 2:
                settings::view[0] = between<float>(settings::view[0] + 0.01, 20, 5);
                break;
        }
    }
    else
    {
        switch (directionKey)
        {
            case 1:
                settings::view[1] = between<float>(settings::view[1] + 0.001, glm::radians(75.0f),
                                                   glm::radians(-75.0f));
                break;
            case 2:
                settings::view[1] = between<float>(settings::view[1] - 0.001, glm::radians(75.0f),
                                                   glm::radians(-75.0f));
                break;
            case 3:
                settings::view[2] = settings::view[2] - 0.001;
                break;
            case 4:
                settings::view[2] = settings::view[2] + 0.001;
                break;
        }
    }

    settings::zoom = between<float>(settings::zoom + zoom * 0.01, 60, 30);

    if (directionKey || (zoom != 0))
    {
        updateView();
    }
}

void framebuffer_size_callback(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    GLFWwindow* window;
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Rubix Cube", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    Shader ourShader("../shader/core.vert", "../shader/core.frag", nullptr);


    unsigned int VBO, VAO, EBO, texture;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    int width, height, nrChannels;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned char* data = stbi_load("../resources/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("aTexture", 0);


    glm::mat4 projection = glm::perspective(glm::radians(settings::zoom),
                                            (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                            0.1f,
                                            100.0f);
    ourShader.setMat4("projection", projection);

    glEnable(GL_DEPTH_TEST);

    settings::rotType = -1;
    settings::progress = 0;
    settings::spinning = false;

    double zaWarudo = glfwGetTime(); // Ko no DIO da!
    initState(settings::state);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture);

        ourShader.use();

        projection = glm::perspective(glm::radians(settings::zoom),
                                      (float) SCR_WIDTH / (float) SCR_HEIGHT,
                                      0.1f,
                                      100.0f);
        ourShader.setMat4("projection", projection);

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);

        glBindVertexArray(VAO);

        if (!settings::spinning)
        {
            zaWarudo = glfwGetTime();
            if (settings::waiting > 0)
                settings::waiting -= 1;
        }

        settings::progress = (glfwGetTime() - zaWarudo) / 1.2;

        if (settings::rotType >= 0 && settings::rotType < 12)
            animateCube(settings::rotType, settings::state, settings::progress * (pi / 2), settings::animation);

        replay();

        if (settings::progress > 1)
        {
            settings::progress = 0;
            settings::spinning = false;

            rotateCube(settings::rotType, settings::state);

            initRotation(settings::animation);
            settings::rotType = -1;
        }

        for (unsigned int i = 0; i < 8; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);

            model = glm::translate(model, initCubePositions[i]);
            model = settings::animation[i] * settings::state.rotation[i] * model;

            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}