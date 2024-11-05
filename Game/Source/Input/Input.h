#pragma once

class Input
{
public:
    Input() = default;
    bool Init(GLFWwindow *window);
    void ProcessInput();

    inline static float lastX = WindowData::W / 2.0f;
    inline static float lastY = WindowData::H / 2.0f;
    inline static bool firstMouse = true;

    inline static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mod);
    inline static void mouseCallback(GLFWwindow *window, double xpos, double ypos);
    inline static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    inline static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

private:
    inline static GLFWwindow *m_Window = nullptr;
    inline static float deltaTime;
    inline static float lastFrame;
};
