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
        Window() = default;

        static GLFWwindow *GetGlfwWindow();
        
        inline static GLFWwindow *m_glfwWindow = nullptr;
    };
} // namespace Window
