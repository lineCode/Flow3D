#pragma once

#include "Component.hpp"
#include "Flow3D/Input.hpp"

#include <tuple>
#include <vector>
#include <algorithm>
#include <iostream>

// Constructor: ComponentToggler(GameObject& gameObject, Component& componentToToggle, bool enabled = true)
// gives the object the functionality and data to enable/disable the referenced component
class ComponentToggler : public Component
{
	CLASS_DECLARATION(ComponentToggler)

public:
	ComponentToggler() : m_Input(Input::Get()) {}
	ComponentToggler(GameObject* gameObject, bool enabled = true) 
		: Component(gameObject, enabled, "ComponentToggler"), m_Input(Input::Get()) {}

	virtual void OnEvent(Event& e) override
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(FLOW_BIND_EVENT_FUNCTION(ComponentToggler::OnKeyPressed));
	}

	void AddComponentToToggle(std::tuple<Component*, Keycode> component, bool fromFile)
	{
		m_ComponentsToToggle.push_back(component);
		if (!fromFile)
			componentsToToggle.push_back(std::make_tuple(std::get<0>(component)->GetName(), (int)std::get<1>(component)));
	}

	void RemoveComponentToToggle(Component* component)
	{
		for (int i = 0; i < m_ComponentsToToggle.size(); i++)
		{
			if (std::get<0>(m_ComponentsToToggle[i]) == component)
			{
				m_ComponentsToToggle.erase(m_ComponentsToToggle.begin() + i);
				componentsToToggle.erase(componentsToToggle.begin() + i);
				i--;
			}
		}
	}

	std::vector<std::tuple<Component*, Keycode>>& GetComponentsToToggle() { return m_ComponentsToToggle; }

	// used for serialization
	std::vector<std::tuple<std::string, int>> componentsToToggle;

private:
	Input& m_Input;	
	std::vector<std::tuple<Component*, Keycode>> m_ComponentsToToggle;

	bool OnKeyPressed(KeyPressedEvent& e)
	{
		for (std::vector<std::tuple<Component*, Keycode>>::const_iterator i = m_ComponentsToToggle.begin(); i != m_ComponentsToToggle.end(); ++i)
			if (e.GetKeyCode() == (int)std::get<1>(*i))
				std::get<0>(*i)->SetEnabled(!std::get<0>(*i)->GetEnabled());

		return false; // should not block other events right now because it is only for testing
	}
};

#include <MetaStuff/include/Meta.h>

namespace meta {

	template <>
	inline auto registerMembers<ComponentToggler>()
	{
		return std::tuple_cat(
			meta::getMembers<Component>(),
			members(
				member("componentsToToggle", &ComponentToggler::componentsToToggle)
			)
		);
	}

} // end of namespace meta



