#pragma once

namespace Render
{
    class Renderer;
}

namespace Window
{
    class Window
    {
    public:

        static GLFWwindow *GetGlfwWindow();

        Window() = default;

        static void FrameBufferResizeCallback(GLFWwindow *glfwWindow, int width, int height);

        inline static GLFWwindow *m_glfwWindow = nullptr;
    };
} // namespace Window
