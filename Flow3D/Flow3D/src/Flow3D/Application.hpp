#pragma once

#include "Core.hpp"
#include "Events/Event.hpp"
#include "Events/ApplicationEvent.hpp"
#include "Input.hpp"
#include "Rendering/RenderingEngine.hpp"
#include "Scene.hpp"
#include "ImGui/ImGuiLayer.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Texture.hpp"
#include "Rendering/Model.hpp"
#include "ResourceManager.hpp"
#include "Window.hpp"
#include "Math.hpp"

// This class manages the application. All systems will be started in the constructor,
// it controls the application in the Run-Loop, forwards events, handles exiting the application and
// holds references to the most important systems. There can only be one instance of an application
// which will be created outside of the engine.
class Application
{
public:
	Application();
	virtual ~Application();

	void Run();
	void Quit();

	void OnEvent(Event& e);

	void CreateAndSetNewScene(std::string name);

	inline static Application& Get() { return *s_Instance; }
	inline const Window& GetWindow() const { return *m_Window; }	
	inline Scene& GetCurrentScene() { return *m_CurrentScene.get(); }
	const std::vector<std::shared_ptr<GameObject>>& GetAllGameObjects() const { return m_CurrentScene->GetRoot().GetChildren(); }		
	inline const unsigned int GetRenderTexture() const { return m_RenderingEngine->GetRenderTexture(); }

	int GetNextObjectID();

private:
	static Application* s_Instance;

	bool m_Running = true;
	std::unique_ptr<Window> m_Window;		
	std::unique_ptr<Input> m_Input;
	std::unique_ptr<RenderingEngine> m_RenderingEngine;
	std::unique_ptr<Scene> m_CurrentScene;
	std::unique_ptr<ImGuiLayer> m_ImGui;	
	std::unique_ptr<ResourceManager> m_ResourceManager;

	int m_NextObjectID;

	bool OnWindowClose(WindowCloseEvent& e);	
	bool OnKeyPressed(KeyPressedEvent& e);
};

// to be defined in client
Application* CreateApplication();

