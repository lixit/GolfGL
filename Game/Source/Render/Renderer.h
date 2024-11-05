#pragma once

#include <Entities/EntityManager.h>
#include <Window/Window.h>

#ifdef _DEBUG
#include <Window/DebugWindow.h>
#endif

namespace Render
{
    class Framebuffer;

    class Renderer
    {
    public:
        Renderer(GLFWwindow *m_glfwWindow);

        void Update();

        ~Renderer();

        GLFWwindow *m_glfwWindow;

    private:
        float lastTime;

#ifdef _DEBUG
        Window::DebugWindow m_DebugWindow;
#endif

        Framebuffer *m_FrameBuff                = nullptr;
        Entities::EntityManager *m_EntryManager = nullptr;

        inline static float m_clearColorRgba[4] = {0.5f, 0.5f, 0.5f, 1.f};

        inline void m_clearColor();
    };
} // namespace Render
