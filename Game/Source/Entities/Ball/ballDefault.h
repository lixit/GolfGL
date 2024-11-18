#pragma once

#ifdef _DEBUG
#include <Window/DebugWindow.h>
#endif

namespace Entities
{
    class ballDefault
    {
#ifdef _DEBUG
        friend Window::DebugWindow;
#endif
        friend class Ball;

        inline static constexpr const char* modelDir  = "Resources/Objects/golfBall";
        inline static constexpr const char* modelPath = "Resources/Objects/golfBall/golfBall.obj";
        inline static glm::vec3 position{14.f, -1.85f, 3.5f};
        inline static glm::vec3 speed{0.f};        
        inline static float scale = 1.f;
        inline static float frictionFactor = -0.2f;
        inline static float shootSpeed     = 100.f;
        inline static float timeout        = 0.2f;
        inline static float angle          = 5.f;
    };
} // namespace Entities
