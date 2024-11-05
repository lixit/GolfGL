#pragma once

#include <Render/Shader.h>
#include <Render/Texture.h>
#include <Entities/Material.h>

#ifdef _DEBUG
namespace Window { class DebugWindow;}
#endif

namespace Entities
{

    // Forward declaration
    class EntityManager;

    class Entity
    {
    public:
        virtual void Update(float delta) = 0;

        virtual ~Entity() = default;

    protected:
        Entity(const std::string &shaderName) : shader(shaderName) { shader.Compile(); }

        inline static glm::mat4 projection() {

            return glm::perspective(glm::radians(CAMERA.GetCameraZoom()), 
                            WindowData::W / WindowData::H, 
                            0.1f, 2000.0f);
        };
        inline static glm::mat4 view{Math::I4};
        glm::mat4 model{Math::I4};
        glm::vec3 position{0.f};

        Render::Shader shader;

        inline void setUniformPVM() const
        {
            shader.setMat4("PV", projection() * view);            
            shader.setMat4("model", model);
        }

        friend EntityManager;
#ifdef _DEBUG
        friend Window::DebugWindow;
#endif

    };
} // namespace Entities
