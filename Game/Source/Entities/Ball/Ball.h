#pragma once

#include <Entities/Entity.h>
#include <Model/Model.h>


namespace Entities {	
	
	class Ball : public Render::Model
		, public Entity
	{
	public:
		Ball();

		void Update(float delta) override;
	
	private:

		glm::vec3 position;
		glm::vec3 direction;			
		int m_angle;
		float m_speed{0.f};				

		unsigned m_diffuseMap;		

		void m_SetLightUniforms();
		void m_HandleTransformations(float delta);

		

		inline auto m_NormalOnVec(const glm::vec3& direction) -> glm::vec3
		{
			return glm::normalize(glm::vec3{ direction.z, 0.f, -direction.x });
		}
		
	};
}