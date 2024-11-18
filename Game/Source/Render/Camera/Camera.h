#pragma once

#include <precomp.h>

#ifdef _DEBUG
    namespace Window { class DebugWindow; }
#endif

enum class Direction
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class Camera
{
public:
	Camera(const Camera&)  = delete;   	
	void operator=(const Camera&) = delete;
	
	static void Init();
	inline static Camera& GetCamera() { return *ms_Camera; }

	void Look(Direction direction, float dt);
    glm::mat4 LookAt();

	const glm::vec3& GetCameraPos();
	float GetCameraZoom();

	const glm::vec3& GetCameraFront();


	void ProcessMouseMovement(float xoffset, float yoffset);
	void ProcessMouseScroll(float yoffset);

private:

	inline static std::unique_ptr<Camera> ms_Camera;	

	inline static glm::vec3 cameraPos {11.4f, -1.7f, 25.f};
	inline static glm::vec3 cameraFront { 0.f, 0.f, -1.f };
	inline static glm::vec3 cameraUp{ 0.f, 1.f, 0.f };

	inline static float Yaw = -90.f;
	inline static float Pitch = 0.f;		
	inline static float MovementSpeed = 10.0f;
	inline static float MouseSensitivity = 0.1f;
	inline static float Zoom = 45.f;

	/* Bounds */
	inline static const float constrainPitchBound = 89.f;
	inline static const float zoomLowerBound = 1.f;
	inline static const float zoomUpperBound = 89.f;
	
	Camera() = default;

	void m_UpdateCameraVectors();

	/* Keeping angles in range [lower, upper], to prevent flipping */
	inline void m_BoundAngleInRange(float& angle, float lower, float upper) const
    {
		angle = std::max(lower, angle);
		angle = std::min(angle, upper);
	}

#ifdef _DEBUG
	friend Window::DebugWindow;
#endif
};

#define CAMERA Camera::GetCamera()