#pragma once

#include "Component.hpp"

#include "Flow3D/Input.hpp"


// Constructor: Rotatable(GameObject& gameObject, bool enabled = true)
// Gives functionality and data for rotating this GameObject
class Rotatable : public Component
{
	CLASS_DECLARATION(Rotatable)

public: 
	Rotatable() : m_Input(Input::Get()) {}
	Rotatable(GameObject* gameObject, bool enabled = true) : Component(gameObject, enabled, "Rotatable"), m_Input(Input::Get()), rotationSpeed(1.0f) {}

	virtual void OnUpdate(double deltaTime) override
	{
		if (m_Input.GetKey(Keycode::U))
			GetTransform().Rotate(Vec3(1.0f, 0.0f, 0.0f), rotationSpeed);

		if (m_Input.GetKey(Keycode::I))
			GetTransform().Rotate(Vec3(0.0f, 1.0f, 0.0f), rotationSpeed);

		if (m_Input.GetKey(Keycode::O))
			GetTransform().Rotate(Vec3(0.0f, 0.0f, 1.0f), rotationSpeed);

		if (m_Input.GetKey(Keycode::J))
			GetTransform().Rotate(Vec3(1.0f, 0.0f, 0.0f), -rotationSpeed);

		if (m_Input.GetKey(Keycode::K))
			GetTransform().Rotate(Vec3(0.0f, 1.0f, 0.0f), -rotationSpeed);

		if (m_Input.GetKey(Keycode::L))
			GetTransform().Rotate(Vec3(0.0f, 0.0f, 1.0f), -rotationSpeed);
	}

	float rotationSpeed;

private:
	Input& m_Input;	
};

#include <MetaStuff/include/Meta.h>

namespace meta {

	template <>
	inline auto registerMembers<Rotatable>()
	{
		return std::tuple_cat(
			meta::getMembers<Component>(),
			members(
				member("rotationSpeed", &Rotatable::rotationSpeed)
			)
		);
	}

} // end of namespace meta
