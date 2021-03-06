#pragma once

#include "Window.hpp"
#include "GameObject.hpp"
#include "Rendering/Skybox.hpp"
#include "Flow3D/Components/Lighting.hpp"

// The scene represents a game world, you can have several scenes which can be switched in the application.
// It is made up of GameObjects (1 GameObject root and its children) with components.
class Scene
{
public:

	Scene();
	Scene(std::string name);
	~Scene();
	Scene(const Scene& scene);

	void AddToScene(std::shared_ptr<GameObject> gameObject);

	void OnAttach();
	void OnDetach();
	void OnUpdate(double deltaTime);
	void OnEvent(Event& event);

	GameObject* FindGameObject(std::string name);

	inline std::string GetName() { return m_Name; };
	inline GameObject& GetRoot() { return *m_Root; } 
	inline GameObject& GetMainCamera() { return *m_MainCamera; }

	inline Skybox& GetSkybox() { return *m_Skybox; }
	void SetSkybox(std::shared_ptr<Skybox> skybox);

	void SetMainCamera(GameObject* mainCamera);	

	void SetDirectionalLight(DirectionalLight* directionalLight);
	inline DirectionalLight& GetDirectionalLight() { return *m_DirectionalLight; }

	void AddPointLight(PointLight* pointLight);
	void RemovePointLight(PointLight* pointLight);
	inline std::vector<PointLight*> GetPointLights() { return m_PointLights; }

	void AddSpotLight(SpotLight* spotLight);
	void RemoveSpotLight(SpotLight* spotLight);
	inline std::vector<SpotLight*> GetSpotLights() { return m_SpotLights; }

	void SetBackgroundColor(Color color) { m_BackgroundColor = color; }
	inline Color GetBackgroundColor() { return m_BackgroundColor; }

	std::string m_Name;
	std::string m_SkyboxName;
	Color m_BackgroundColor;

private:
	std::unique_ptr<GameObject> m_Root;
	
	GameObject* m_MainCamera;
	std::shared_ptr<Skybox> m_Skybox;
	DirectionalLight* m_DirectionalLight;
	std::vector<PointLight*> m_PointLights;
	std::vector<SpotLight*> m_SpotLights;
};

#include <MetaStuff/include/Meta.h>

namespace meta {

	template <>
	inline auto registerMembers<Scene>()
	{
		return members(
			member("m_Name", &Scene::m_Name),
			member("m_SkyboxName", &Scene::m_SkyboxName),
			member("m_BackgroundColor", &Scene::m_BackgroundColor)
		);
	}

} // end of namespace meta