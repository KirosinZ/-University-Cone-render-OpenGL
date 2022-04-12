#include <iostream>
#include <vector>
#include <gl/glew.h>
#include <GLFW/glfw3.h>

#include "program/shader.h"
#include "imglib/stb_image.h"
#include "camera/camera.h"

#include "coneobject/cone_approx.h"
#include "pointlight/pointlight.h"
#include "skybox/skybox.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "skyboxes.h"

#define LIGHT_COLOR 1.0f
#define LIGHT_INTENSITY 10.0f

#define AMBIENT_PART 0.1f
#define DIFFUSE_PART 0.45f
#define SPECULAR_PART 0.45f

#define LIGHT_ATTENUATION_CONSTANT 1.0f
#define LIGHT_ATTENUATION_LINEAR 0.3f
#define LIGHT_ATTENUATION_QUADRATIC 0.24f

#define CONE_HEIGHT 4.0f
#define CONE_RADIUS 2.0f

#define CONE_COLOR 0.0f, 0.5f, 1.0f
#define CONE_SPECULAR_COLOR 1.0f
#define CONE_SHININESS 64.0f

#define SKYBOX_INDEX 3

// View globals
Camera camera(glm::vec3(0.0f));
float cameraSpeed = camera.MovementSpeed;

float wndWidth = 2560.0f;
float wndHeight = 1440.0f;

bool firstMouse = true;
float lastX = wndWidth / 2;
float lastY = wndHeight / 2;

// Misc globals
float deltaTime = 0.0f;
float lastFrame = 0.0f;

bool wireframeKeyPressed = false;
bool wireframeToggle = false;

float coneOpacity = 1.0f;

// Light globals
glm::vec3 lightPos = glm::vec3(0.0f);
glm::vec3 lightCol = glm::vec3(LIGHT_COLOR);

float lightYaw = 0.0f;
float lightPitch = 30.0f;
float lightDistance = 4.0f;
float lightSpeed = 1.25f;

// UI + callbacks

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    wndWidth = width;
    wndHeight = height;
    glViewport(0, 0, wndWidth, wndHeight);
}

void keyboard(GLFWwindow* wnd)
{
    if (glfwGetKey(wnd, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(wnd, GLFW_TRUE);

    if (glfwGetKey(wnd, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.MovementSpeed = cameraSpeed * 5;
    else
        camera.MovementSpeed = cameraSpeed;

    if (glfwGetKey(wnd, GLFW_KEY_1) != GLFW_PRESS) wireframeKeyPressed = false;
    if (glfwGetKey(wnd, GLFW_KEY_1) == GLFW_PRESS && !wireframeKeyPressed)
    {
        wireframeKeyPressed = true;
        wireframeToggle = !wireframeToggle;
    }

    if (glfwGetKey(wnd, GLFW_KEY_UP) == GLFW_PRESS)
        lightPitch = fmin(lightPitch + lightSpeed, 89.0f);
    if (glfwGetKey(wnd, GLFW_KEY_DOWN) == GLFW_PRESS)
        lightPitch = fmax(lightPitch - lightSpeed, -89.0f);
    if (glfwGetKey(wnd, GLFW_KEY_LEFT) == GLFW_PRESS)
        lightYaw += lightSpeed;
    if (glfwGetKey(wnd, GLFW_KEY_RIGHT) == GLFW_PRESS)
        lightYaw -= lightSpeed;

    if (glfwGetKey(wnd, GLFW_KEY_W) == GLFW_PRESS)
        coneOpacity = fmin(coneOpacity + 0.01f, 1.0f);
    if (glfwGetKey(wnd, GLFW_KEY_S) == GLFW_PRESS)
        coneOpacity = fmax(coneOpacity - 0.01f, 0.0f);

}

void mouse_callback(GLFWwindow* wnd, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset, GL_TRUE);
}

void mouse_scroll_callback(GLFWwindow* wnd, double xoffset, double yoffset)
{
    lightDistance = glm::clamp(lightDistance + lightSpeed * (float)yoffset, 4.0f, 15.0f);
}

GLFWwindow* init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(wndWidth, wndHeight, "OpenGL", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Window could not be opened." << std::endl;
        glfwTerminate();
        return window;
    }
    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);

    glewInit();
    glViewport(0, 0, wndWidth, wndHeight);

    glfwMaximizeWindow(window);

    return window;
}

int main()
{

    GLFWwindow* window = init();
    if (window == nullptr) return -1;

    Shader cone_shader("../shaders/shader.vert", "../shaders/shader.frag");
    Shader light_shader("../shaders/lightshader.vert", "../shaders/lighshader.frag");
    Shader skybox_shader("../shaders/skybox.vert", "../shaders/skybox.frag");

    float aspect = wndHeight / wndWidth;
    float width = 20.0f;
    glm::mat4 projection = glm::ortho(-0.5f * width, 0.5f * width, -0.5f * aspect * width, 0.5f * aspect * width, -100.0f, 100.0f);

    // Skybox definition
    skybox skybox(skybox_textures[SKYBOX_INDEX]);

    skybox_shader.use();
    skybox_shader.setMat4("projection", glm::perspective(glm::radians(45.0f), wndWidth / wndHeight, 0.1f, 100.0f));
    skybox_shader.setInt("skybox", 0);
    //---------------------

    // Cone definition
    cone_approx<>::generate_vertices();
    cone_approx<> cone;

    glm::mat4 cone_model = glm::mat4(1.0f);
    cone_model = glm::scale(cone_model, glm::vec3(CONE_RADIUS, CONE_HEIGHT, CONE_RADIUS));

    glm::mat4 cone_normal = cone_model;
    cone_normal = glm::inverse(cone_normal);
    cone_normal = glm::transpose(cone_normal);

    cone_shader.use();
    cone_shader.setMat4("model", cone_model);
    cone_shader.setMat4("normalmodel", cone_normal);
    cone_shader.setMat4("projection", projection);

    cone_shader.setFloat("pointlight.constant", LIGHT_ATTENUATION_CONSTANT);
    cone_shader.setFloat("pointlight.linear", LIGHT_ATTENUATION_LINEAR);
    cone_shader.setFloat("pointlight.quadratic", LIGHT_ATTENUATION_QUADRATIC);
    cone_shader.setVec3("pointlight.ambient", AMBIENT_PART * LIGHT_INTENSITY * lightCol);
    cone_shader.setVec3("pointlight.diffuse", DIFFUSE_PART * LIGHT_INTENSITY * lightCol);
    cone_shader.setVec3("pointlight.specular", SPECULAR_PART * LIGHT_INTENSITY * lightCol);

    cone_shader.setVec3("material.diffuse", glm::vec3(CONE_COLOR));
    cone_shader.setVec3("material.specular", glm::vec3(CONE_SPECULAR_COLOR));
    cone_shader.setFloat("material.shininess", CONE_SHININESS);
    //---------------------

    // Light definition
    pointlight light(15.0f);

    glm::mat4 light_model(1.0f);

    light_shader.use();
    light_shader.setMat4("projection", projection);

    light_shader.setVec3("lightColor", lightCol);
    //------------------

    glm::mat4 view = camera.view();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glFrontFace(GL_CW);
    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    while(!glfwWindowShouldClose(window))
    {
        auto currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        keyboard(window);

        view = camera.view();

        // Light render
        float cp = cosf(glm::radians(lightPitch));
        float sp = sinf(glm::radians(lightPitch));
        float cy = cosf(glm::radians(lightYaw));
        float sy = sinf(glm::radians(lightYaw));
        lightPos = lightDistance * glm::vec3(-sy * cp, sp, cy * cp);

        light_model = glm::translate(glm::mat4(1.0f), lightPos);

        light_shader.use();
        light_shader.setMat4("model", light_model);
        light_shader.setMat4("view", view);


        light.draw();
        //-------------

        // Skybox render
        glm::mat4 skyview = glm::mat4(glm::mat3(view));
        skybox_shader.use();
        skybox_shader.setMat4("view", skyview);

        skybox.draw();
        //--------------

        // Cone render
        cone_shader.use();
        cone_shader.setMat4("view", view);
        cone_shader.setVec3("pointlight.position", lightPos);

        cone_shader.setFloat("material.opacity", coneOpacity);

        cone_shader.setVec3("toView", -camera.Front);


        if (wireframeToggle) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        cone.draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //-------------

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
