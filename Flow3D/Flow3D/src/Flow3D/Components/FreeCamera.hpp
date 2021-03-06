#pragma once

#include "Component.hpp"
#include "Flow3D/Math.hpp"
#include "Flow3D/Window.hpp"
#include "Flow3D/Input.hpp"
#include "Flow3D/Application.hpp"
#include "Flow3D/Log.hpp"

static const float PI = 3.1415926f;

// TODO: should be able to set these somewhere
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;
const float ZNEAR = 0.1f;
const float ZFAR = 100.0f;

// Constructor: FreeCamera(GameObject& gameObject, const Window& window, bool enabled = true)
// Gives functionality and data for a free moving camera which can be moved with 
// PAGE UP, PAGE DOWN for vertical movement,
// WASD or UP, DOWN, LEFT, RIGHT 
// and rotated with mouse movement
class FreeCamera : public Component
{
	CLASS_DECLARATION(FreeCamera)

public:
	FreeCamera() : m_Window(Application::Get().GetWindow()), m_Input(Input::Get()) 
	{

	}

	FreeCamera(GameObject* gameObject, const Window& window, bool enabled = true, float yaw = -90.0f, float pitch = 0.0f, bool m_isActive = false)
		: m_Window(window), Component(gameObject, enabled, "FreeCamera"), m_Input(Input::Get()), m_Yaw(yaw), m_Pitch(pitch)
	{
		m_MovementSpeed = SPEED;
		m_MouseSensitivity = SENSITIVITY;
		m_Zoom = ZOOM;
		m_ZNear = ZNEAR;
		m_ZFar = ZFAR;

		m_WorldUp = Vec3(0.0f, 1.0f, 0.0f);

		GetTransform().SetIsCamera(true);

		// make sure that the camera can't be moved
		firstMouse = true;
		// center the mouse position
		lastMouse = Vec2((float)m_Window.GetWidth() / 2, (float)m_Window.GetHeight() / 2);

		UpdateVectors();
	}

	~FreeCamera()
	{
	}

	virtual void OnUpdate(double deltaTime) override
	{
		if (m_IsActive)
		{
			// calculate the velocity depending on the frame rate and user set movement speed
			float velocity = m_MovementSpeed * (float)deltaTime;

			if (m_Input.GetKey(Keycode::W))
			{
				Vec3 forward = GetTransform().GetForwardVector();
				forward = forward * velocity;
				GetTransform().Translate(forward);
			}
			if (m_Input.GetKey(Keycode::S))
			{
				Vec3 forward = GetTransform().GetForwardVector();
				forward = forward * velocity * -1.0f;
				GetTransform().Translate(forward);
			}
			if (m_Input.GetKey(Keycode::D))
			{
				Vec3 right = GetTransform().GetRightVector();
				right = right * velocity;
				GetTransform().Translate(right);
			}
			if (m_Input.GetKey(Keycode::A))
			{
				Vec3 right = GetTransform().GetRightVector();
				right = right * velocity * -1.0f;
				GetTransform().Translate(right);
			}
			if (m_Input.GetKey(Keycode::PageUp))
			{
				Vec3 up = GetTransform().GetUpVector();
				up = up * velocity;
				GetTransform().Translate(up);
			}
			if (m_Input.GetKey(Keycode::PageDown))
			{
				Vec3 up = GetTransform().GetUpVector();
				up = up * velocity * -1.0f;
				GetTransform().Translate(up);
			}
		}
	}

	virtual void OnEvent(Event& event) override
	{
		if (m_IsActive)
		{
			EventDispatcher dispatcher(event);
			dispatcher.Dispatch<MouseMovedEvent>(FLOW_BIND_EVENT_FUNCTION(FreeCamera::OnMouseMoved));
			// TODO: mouse scroll
		}
	}

	Mat4 GetViewMatrix() 
	{ 
		Vec3 position = GetTransform().GetPosition();
		return Mat4::LookAt(position, position + m_Front, m_Up);
	}

	void SetMovementSpeed(float movementSpeed) { m_MovementSpeed = movementSpeed; }
	void SetMouseSensitivity(float mouseSensitivity) { m_MouseSensitivity = mouseSensitivity; }
	void SetZoom(float zoom) { m_Zoom = zoom; }
	void SetZNear(float ZNear) { m_ZNear = ZNear; }
	void SetZFar(float ZFar) { m_ZFar = ZFar; }
	void SetIsActive(bool isActive)
	{ 
		if (!isActive && m_IsActive)
		{
			Application::Get().GetCurrentScene().SetMainCamera(nullptr);
			GetTransform().SetIsCamera(false);
		}
			
		m_IsActive = isActive;

		if (m_IsActive)
		{
			Application::Get().GetCurrentScene().SetMainCamera(m_GameObject);
			UpdateVectors();
			GetTransform().SetIsCamera(true);
		}
			
	}

	bool GetIsActive() const { return m_IsActive; }
	float GetMovementSpeed() const { return m_MovementSpeed; }
	float GetMouseSensitivity() const { return m_MouseSensitivity; }
	float GetZoom() const { return m_Zoom; }
	float GetZNear() const { return m_ZNear; }
	float GetZFar() const { return m_ZFar; }

	float m_Pitch;
	float m_Yaw;
	bool m_IsActive;

private:
	const Window& m_Window;
	bool firstMouse = true;
	Vec2 lastMouse;

	Vec3 m_WorldUp;
	Vec3 m_Front;
	Vec3 m_Right;
	Vec3 m_Up;

	float m_MovementSpeed;
	float m_MouseSensitivity;
	float m_Zoom;
	float m_ZNear;
	float m_ZFar;	

	Input& m_Input;

	bool OnMouseMoved(MouseMovedEvent& e)
	{
		Vec2 mousePosition = Vec2(e.GetX(), e.GetY());

		// set lastMouse equal to mousePosition when it the first mousemovement is detected
		// so the offset will be 0 for the first movement
		// otherwise the camera will rotate with the first movement and position itself wrong
		if (firstMouse)
		{
			lastMouse = mousePosition;
			firstMouse = false;
		}

		float xOffset = mousePosition.x - lastMouse.x;
		float yOffset = lastMouse.y - mousePosition.y;

		lastMouse = mousePosition;

		xOffset *= m_MouseSensitivity;
		yOffset *= m_MouseSensitivity;

		Look(xOffset, yOffset);

		return false;
	}

	// adapted from: https://community.khronos.org/t/how-to-limit-x-axis-rotation/75515/11
	void Look(float xOffset, float yOffset) {
		m_Yaw += xOffset;
		m_Pitch += yOffset;

		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;

		if (m_Yaw < -450.0f)
			m_Yaw = -90.0f;
		if (m_Yaw >= 270.0f)
			m_Yaw = -90.0f;

		UpdateVectors();
	}

	void UpdateVectors()
	{
		// Calculate the new Front vector
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = Vec3(glm::normalize(front));
		// Also re-calculate the Right and Up vector
		m_Right = Vec3(glm::normalize(glm::cross(glm::vec3(m_Front.x, m_Front.y, m_Front.z), glm::vec3(m_WorldUp.x, m_WorldUp.y, m_WorldUp.z))));
		// Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		m_Up = glm::normalize(glm::cross(glm::vec3(m_Right.x, m_Right.y, m_Right.z), glm::vec3(m_Front.x, m_Front.y, m_Front.z)));

		Vec3 rotationVector = Vec3(- 1 * m_Pitch, m_Yaw + 90.0f, 0.0f);
			
		Quaternion rotation = Quaternion(rotationVector);
		GetTransform().SetOrientation(rotation);
		GetTransform().SetFrontVector(m_Front);
		GetTransform().SetRightVector(m_Right);
		GetTransform().SetUpVector(m_Up);
	}
};

#include <MetaStuff/include/Meta.h>

namespace meta {

	template <>
	inline auto registerMembers<FreeCamera>()
	{
		return std::tuple_cat(
			meta::getMembers<Component>(),
			members(
				member("m_MovementSpeed", &FreeCamera::GetMovementSpeed, &FreeCamera::SetMovementSpeed),
				member("m_MouseSensitivity", &FreeCamera::GetMouseSensitivity, &FreeCamera::SetMouseSensitivity),
				member("m_Zoom", &FreeCamera::GetZoom, &FreeCamera::SetZoom),
				member("m_ZNear", &FreeCamera::GetZNear, &FreeCamera::SetZNear),
				member("m_ZFar", &FreeCamera::GetZFar, &FreeCamera::SetZFar),
				member("m_Pitch", &FreeCamera::m_Pitch),
				member("m_Yaw", &FreeCamera::m_Yaw),
				member("m_IsMainCamera", &FreeCamera::m_IsActive)
			)			
		);
	}

} // end of namespace meta

