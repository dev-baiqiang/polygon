// Copyright (c) 2018 BaiQiang. All rights reserved.

#include <iostream>
#include <algorithm>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "shader.h"
#include "path.h"
#include "screenshot.h"

#include <vector>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/string_cast.hpp>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>


const unsigned int WINDOW_WIDTH = 800;
const unsigned int WINDOW_HEIGHT = 600;
static int shot = 0;

static void onSizeChange(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS) {
        shot = 1;
    }
}

static GLuint program;
static GLuint VAO, VBO, IBO;

void onInputText(GLFWwindow *window, unsigned int codepoint, int mods) {
    std::cout << codepoint << " : " << mods << std::endl;
    if (mods == 1) {
        std::cout << "Shift key pressed aswell" << std::endl;
    }
}

GLFWwindow *initWindow() {
    glfwInit();
#ifdef __APPLE__
    /* We need to explicitly ask for a 3.2 context on OS X */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif
    glfwWindowHint(GLFW_SAMPLES, 16);
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "LearnOpenGL", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    int width, height, nrChannels;
    unsigned char *data = stbi_load("res/icon.png", &width, &height, &nrChannels, 0);
    const GLFWimage icon = {
            width,
            height,
            data
    };
    glfwSetWindowIcon(window, 1, &icon);
    stbi_image_free(data);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onSizeChange);
    glfwSetCharModsCallback(window, onInputText);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
    }
    glfwSwapInterval(1);
    return window;
}

void initGL() {
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glClearColor(1, 1, 1, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(WINDOW_WIDTH), 0.0f,
                                      static_cast<GLfloat>(WINDOW_HEIGHT));

    program = shader_load("res/vs.glsl",
                          "res/fs.glsl");
    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &IBO);
    glBindVertexArray(0);
}

typedef glm::vec2 Point;

void drawPath(polygon::Path path) {
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, path.vs.size() * 2 * sizeof(float), &path.vs[0], GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);

    if (path.getStyle() == polygon::Path::kFill) {
        glDrawArrays(GL_LINE_LOOP, 0, path.vs.size());
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, path.is.size() * sizeof(GLuint), &path.is[0], GL_DYNAMIC_DRAW);

        glDrawElements(GL_TRIANGLES, path.is.size() * sizeof(GLuint), GL_UNSIGNED_INT, nullptr);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

int main() {


    GLFWwindow *window = initWindow();
    initGL();

    std::vector<Point> mPoints = {Point(50, 330), Point(250, 230), Point(350, 430), Point(50, 430), Point(50, 530),
                                  Point(450, 330), Point(80, 230)};

    polygon::Path path1(mPoints);
    path1.addPoint(Point(550, 150));
    path1.build();


    polygon::Path path2({});
    for (int i = 0; i < 9; i++) {
        float x;
        if (i % 2 == 0) {
            x = 650;
        } else {
            x = 700;
        }
        float y = i * 50 + 150;
        path2.addPoint({Point(x, y)});
    }
    path2.build();

    polygon::Path path3(
            {Point(30, 30), Point(130, 130), Point(230, 30), Point(330, 130), Point(430, 30), Point(530, 130)});
    path3.addPoint({Point(630, 30)});
    path3.addPoint({Point(730, 130)});
    path3.build();

    polygon::Path path4(
            {Point(500, 300), Point(600, 300), Point(600, 400), Point(500, 400)}, polygon::Path::kStrokeAndFill, 8.0f);
    path4.build();

    polygon::Path path5(
            {Point(500, 450), Point(600, 450), Point(600, 550), Point(500, 550)}, polygon::Path::kFill);
    path5.build();

    auto polygonColor = glGetUniformLocation(program, "polygonColor");
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        float value = sin(timeValue);
        glUseProgram(program);

        glUniform3f(polygonColor, 0, 1.0, value);
        drawPath(path1);

        glUniform3f(polygonColor, 0.5, 0, value);
        drawPath(path2);

        glUniform3f(polygonColor, 0, 0.5, value);
        drawPath(path3);

        glUniform3f(polygonColor, 1, 0.5, value);
        drawPath(path4);

        glUniform3f(polygonColor, 0.5, 0.5, value);
        drawPath(path5);

        if (shot) {
            shot = 0;
            saveScreenShot(window, "screenshot.bmp");
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &IBO);
    glDeleteProgram(program);
    glfwTerminate();
    return 0;
}
