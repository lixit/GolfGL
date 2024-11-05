#pragma once

namespace Entities {
    class Entity;
    class EntityManager;
}
using EntMan = Entities::EntityManager;

namespace Window
{
    /* Wrapper for ImGui window */
    class DebugWindow
    {
    public:
        bool Init(EntMan* entityManager = nullptr, GLFWwindow * glfwWindow = nullptr);
        bool Update();

        void Destroy();

        DebugWindow()  = default;
        ~DebugWindow() = default;

    private:
#ifdef _DEBUG
        EntMan* m_EntityManager;
        GLFWwindow *m_glfwWindow;
        void m_DrawGFXSettings() const;
        void m_DrawCameraMenu()  const;
        void m_DrawLightMenu()   const;
        void m_DrawBallMenu()    const;        
#endif

        bool m_DebugMenuActive = true;
        bool m_ViewportsEnable = false;
        
    };
} // namespace Window
