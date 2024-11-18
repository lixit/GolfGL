#include <precomp.h>
#include "Ball.h"
#include "ballDefault.h"

namespace Entities
{
    Ball::Ball()        
        : Entity("pbr")
        , m_material("material")
        , m_model(ballDefault::modelPath)
        , m_speed(ballDefault::speed)                
        , m_scale(ballDefault::scale)
        , m_frictionFactor(ballDefault::frictionFactor)
        , m_angle(ballDefault::angle)        
        , m_scaleMatrix(glm::scale(Math::I4, ballDefault::scale * Math::e3) )
    {
        m_material.Init(&shader, ballDefault::modelDir);
        this->position = ballDefault::position;        
        m_UpdateModelMatrix();

        // diffuse texture is loaded in parent class
        for(auto& tex : m_model.GetTextures())
            LOG_TRACE("Texture {}, {}", tex.GetID(), tex.GetName());

        shader.ActivateShader();
        setUniformPVM();
        m_material.Bind();
        shader.DeactivateShader();        
    }

    void Ball::Update(float delta)
    {
        shader.ActivateShader();
        m_HandleTransformations(delta);
        m_SetLightUniforms();
        setUniformPVM();

        int index = m_material.Activate(GL_TEXTURE0);
        m_model.Draw(&shader);
        Material::UnbindTextures(index);                

        shader.DeactivateShader();
    }

    inline void setOneLight(const Render::Shader& shader, const PhongLight& light, int i)
    {
        shader.setVec3("light[" + std::to_string(i) + "].position", light.Position);
        shader.setVec3("light[" + std::to_string(i) + "].color", 250.f * light.Color);
    }
    inline void Ball::m_SetLightUniforms() const
    {
        int i = 0;
        for(int l = 0; l < LIGHT.Size(); l++)
        {
            if(LIGHT[l].Type == LightType::DISABLED)
                continue;
            else if (LIGHT[l].Type == LightType::DIRECTIONAL)
            {
                shader.setVec3("light[" + std::to_string(i) + "].position", position - 10.f * glm::normalize(LIGHT[l].Position));
                shader.setVec3("light[" + std::to_string(i) + "].color", 250.f * LIGHT[l].Color);
                ++i;
            }
            else
                setOneLight(shader, LIGHT[l], i++);
        }
        if(LIGHT.Flashlight) { setOneLight(shader, LIGHT.GetFlashlight(), i++); }
        shader.SetValue("numLights", i);
        shader.setVec3("viewPos", CAMERA.GetCameraPos());        
    }

    void Ball::m_UpdateModelMatrix()
    {
        model = glm::translate(Math::I4, position) * m_rotationMatrix * m_scaleMatrix;
    }

    inline void Ball::m_HandleTransformations(float delta)
    {
        if (m_speed != glm::vec3{0.f})
        {
            if (m_ShootEvent())
            {
                m_speed = ballDefault::shootSpeed * glm::normalize(CAMERA.GetCameraFront());
            }
            auto rot = m_NormalOnVec(m_speed);            
            m_angle = (delta / 3.14f)* glm::length(m_speed);            

            m_rotationMatrix = glm::rotate(Math::I4, -m_angle, rot) * m_rotationMatrix;
            m_UpdateModelMatrix();
        }
        else if (m_ShootEvent())
        {
            m_speed = ballDefault::shootSpeed * glm::normalize(CAMERA.GetCameraFront());
        }
    }
} // namespace Entities
