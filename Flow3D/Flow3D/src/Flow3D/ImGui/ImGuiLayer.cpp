#include "ImGuiLayer.hpp"

#include "imgui/imgui_impl_opengl3.h"

//Temporary
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Flow3D/Core.hpp"
#include "Flow3D/Application.hpp"
#include "Flow3D/Log.hpp"
#include "ImGuiHelper.hpp"
#include "ImGuiTextureEditor.hpp"
#include "ImGuiShaderEditor.hpp"
#include "ImGuiModelsEditor.hpp"
#include "ImGuiSkyboxEditor.hpp"

#include "Flow3D/Serializer.hpp"
#include "Flow3D/Components/ComponentManager.hpp"

ImGuiLayer::ImGuiLayer() {}

void ImGuiLayer::OnAttach()
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

	//Temporary:
	// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
	io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
	io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

	ImGui_ImplOpenGL3_Init("#version 410");

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
	colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);

	m_Logger = std::make_unique<Flow3DLog>();
	m_Inspector = std::make_unique<Flow3DInspector>();
}

void ImGuiLayer::OnDetach()
{
}

void ImGuiLayer::OnUpdate(double deltaTime)
{
	ImGuiIO& io = ImGui::GetIO();
	Application& app = Application::Get();
	int width = app.GetWindow().GetWidth();
	int height = app.GetWindow().GetHeight();
	io.DisplaySize = ImVec2((float)width, (float)height);

	float time = (float)glfwGetTime();
	io.DeltaTime = m_Time > 0.0f ? (time - m_Time) : (1.0f / 60.0f);
	m_Time = time;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui::NewFrame();
		
	static bool showResourceManager = false;
	static bool showSceneManager = false;

	ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Flow3D"))
		{
			
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu("Scene"))
		{
			ImGui::MenuItem("Create new Scene");

			static char sceneName[32] = "NewScene";
			if (ImGui::BeginPopupContextItem("New Scene", 1))
			{
				ImGui::Text("Edit name:");
				ImGui::InputText("##edit", sceneName, IM_ARRAYSIZE(sceneName));
				if (ImGui::Button("Create Scene"))
				{
					app.CreateAndSetNewScene(sceneName);
					ImGui::CloseCurrentPopup();
				}
					
				ImGui::EndPopup();
			}

			ImGui::SameLine();
			ShowHelpMarker("right click to create a new scene");

			ImGui::MenuItem("Load scene");
			if (ImGui::BeginPopupContextItem("Load scene", 1))
			{
				static int currentScene = -1;
				std::vector<const char*> sceneNames;
				std::vector<std::string> allSceneNames = ResourceManager::Get().GetAllSceneNames();
				for (unsigned int i = 0; i < allSceneNames.size(); i++)
				{
					if (app.GetCurrentScene().GetName() != allSceneNames[i])
					{
						const char* sceneName = allSceneNames[i].c_str();
						sceneNames.push_back(sceneName);
					}
				}

				ImGui::PushItemWidth(160);
				ImGui::Combo("Scenes", &currentScene, &sceneNames[0], (int)sceneNames.size());

				if (ImGui::Button("Load Scene"))
				{
					if (currentScene != -1)
					{
						app.CreateAndSetNewScene(sceneNames[currentScene]);
						ImGui::CloseCurrentPopup();
					}						
				}

				ImGui::EndPopup();
			}

			ImGui::SameLine();
			ShowHelpMarker("right click to load a scene");

			if (ImGui::MenuItem("Save current Scene"))
			{
				Serializer::Serialize(app.GetCurrentScene());
			}

			ImGui::Separator();

			ImGui::MenuItem("Scene Manager", NULL, &showSceneManager);

			ImGui::EndMenu();
		}

		if (showSceneManager)
		{
			ImGui::SetNextWindowContentSize(ImVec2(360.0f, 240.0f));
			if (ImGui::Begin("Scene Manager"), &showSceneManager, ImGuiWindowFlags_NoResize)
			{
				ImGui::Separator();

				Scene& scene = app.GetCurrentScene();

				bool skyboxActive;

				if (&scene.GetSkybox() != nullptr)
					skyboxActive = scene.GetSkybox().IsShown();
				else
					skyboxActive = false;

				if (ImGui::Checkbox("Skybox active", &skyboxActive))
				{
					if (&scene.GetSkybox() != nullptr)
						scene.GetSkybox().SetShown(skyboxActive);
				}

				std::vector<std::shared_ptr<Skybox>> skyboxes = ResourceManager::Get().GetAllSkyboxes();
				static int currentSkybox = -1;
				std::vector<const char*> skyboxNames;
				for (unsigned int i = 0; i < skyboxes.size(); i++)
				{

					const char* skyboxName = skyboxes[i]->m_Name.c_str();
					skyboxNames.push_back(skyboxName);

					if (&scene.GetSkybox() != nullptr)
						if (skyboxes[i]->GetName() == scene.GetSkybox().GetName())
							currentSkybox = i;
				}

				ImGui::PushItemWidth(160);
				if (ImGui::Combo("Skyboxes", &currentSkybox, &skyboxNames[0], (int)skyboxNames.size()))
				{
					scene.SetSkybox(skyboxes[currentSkybox]);
				}

				Color sceneBgColor = scene.GetBackgroundColor();
				static ImVec4 color = ImColor(sceneBgColor.r, sceneBgColor.g, sceneBgColor.b, sceneBgColor.a);
				ImGui::Text("Background Color:");
				if (ImGui::ColorEdit4("##sceneBgColor", (float*)&color))
					scene.SetBackgroundColor(Color(color.x, color.y, color.z, color.w));
				

				ImGui::End();
			}
		}

		if (ImGui::BeginMenu("Tools"))
		{
			Skybox& skybox = app.GetCurrentScene().GetSkybox();
			if (&skybox != nullptr)
			{
				if (ImGui::MenuItem("Skybox", NULL, skybox.IsShown()))
					skybox.ToggleShow();
			}
			else
			{
				FLOW_CORE_ERROR("no skybox is loaded");
			}

			ImGui::MenuItem("Resource Manager", NULL, &showResourceManager);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		if (showResourceManager)
		{
			ImGui::SetNextWindowContentSize(ImVec2(960.0f, 640.0f));
			if (ImGui::Begin("Resource Manager"), &showResourceManager, ImGuiWindowFlags_NoResize)
			{
				ImGui::Separator();

				ImGui::Columns(4);
				static int selected = -1;

				if (ImGui::Selectable("Textures", selected == 0))
					selected = 0;
						
				ImGui::NextColumn();
				if (ImGui::Selectable("Shaders", selected == 1))
					selected = 1;

				ImGui::NextColumn();
				if (ImGui::Selectable("Models", selected == 2))
					selected = 2;

				ImGui::NextColumn();
				if (ImGui::Selectable("Skyboxes", selected == 3))
					selected = 3;

				ImGui::Columns(1);
				ImGui::Separator();					

				if (selected == 0)
				{
					TextureEditor textureEditor;
					textureEditor.Draw();						
				}
				else if (selected == 1)
				{
					ShaderEditor shaderEditor;
					shaderEditor.Draw();
				}
				else if (selected == 2)
				{
					ModelsEditor modelsEditor;
					modelsEditor.Draw();
				}
				else if (selected == 3)
				{
					SkyboxEditor skyboxEditor;
					skyboxEditor.Draw();
				}

				ImGui::End();
			}
		}
	}

	ImGui::SetNextWindowContentSize(ImVec2(240.0f, 960.0f));
	if (ImGui::Begin("Hierarchy", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar))
	{
		if (ImGui::Button("Add GameObject", ImVec2(240.0f, 20.0f)))
			ImGui::OpenPopup("Add GameObject");

		if (ImGui::BeginPopup("Add GameObject"))
		{
			static char nameBuffer[64] = "GameObject";
			ImGui::InputText("Name of GameObject", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);

			static bool addAsChild = false;
			ImGui::Checkbox("Add as a child of the selected object", &addAsChild);

			if (ImGui::Button("Add GameObject", ImVec2(360.0f, 20.0f)))
			{
				std::shared_ptr<GameObject> newGO = std::make_shared<GameObject>(nameBuffer);
				if (addAsChild && currentGameObject != nullptr)
					currentGameObject->AddChild(newGO);
				else
					app.GetCurrentScene().AddToScene(newGO);

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();

		if (ImGui::Button("Duplicate GameObject", ImVec2(240.0f, 20.0f)))
			ImGui::OpenPopup("Duplicate GameObject");

		if (ImGui::BeginPopup("Duplicate GameObject"))
		{
			static char nameBuffer[64] = "GameObject";
			ImGui::InputText("Name of GameObject", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue);

			static bool addAsChild = false;
			ImGui::Checkbox("Add as a child of the selected object", &addAsChild);

			if (ImGui::Button("Duplicate GameObject", ImVec2(360.0f, 20.0f)))
			{
				if (currentGameObject != nullptr)
				{
					// copy game object with transform
					std::shared_ptr<GameObject> newGO = std::make_shared<GameObject>(nameBuffer, 
						currentGameObject->GetTransform().GetPosition(), currentGameObject->GetTransform().GetOrientation(),
						currentGameObject->GetTransform().GetScale(), currentGameObject->GetIsActive());

					// copy components
					const std::vector<std::shared_ptr<Component>>& allComponents = currentGameObject->GetComponents();
					for (unsigned int i = 0; i < allComponents.size(); i++)
					{
						ComponentManager::DuplicateComponent(*allComponents[i], *newGO);
					}					

					if (addAsChild)
						currentGameObject->AddChild(newGO);
					else
						currentGameObject->GetParent()->AddChild(newGO);
				}

				

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::Separator();
		ImGui::Columns(1);

		ImGui::BeginChild("Hierarchy");

		// Iterate through game objects
		std::vector<std::shared_ptr<GameObject>> allGameObjects = app.GetAllGameObjects();
		for (int i = 0; i < allGameObjects.size(); i++)
			ShowGameObject(allGameObjects[i]->GetName().c_str(), allGameObjects[i]->GetObjectID(), *allGameObjects[i]);

		ImGui::EndChild();
		ImGui::Separator();

		ImGui::End();
	}
			
	ImGui::SetNextWindowContentSize(ImVec2(1280.0f, 720.0f));
	// create our ImGui window
	if (ImGui::Begin("Viewport", NULL, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
	{
		//get the mouse position
		ImVec2 pos = ImGui::GetCursorScreenPos();

		//pass the texture of the FBO
		//app.GetRenderTexture() is the texture of the FBO
		//the next parameter is the upper left corner for the uvs to be applied at
		//the third parameter is the lower right corner
		//the last two parameters are the UVs
		//they have to be flipped (normally they would be (0,0);(1,1) 
		ImGui::GetWindowDrawList()->AddImage(
			(void *)app.GetRenderTexture(),
			ImVec2(ImGui::GetCursorScreenPos()),
			ImVec2(ImGui::GetCursorScreenPos().x + 1280,
				ImGui::GetCursorScreenPos().y + 720), ImVec2(0, 1), ImVec2(1, 0));

		//we are done working with this window
		ImGui::End();
	}	

	m_Inspector->Draw();

	m_Logger->Draw("Flow3D Log");

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::ShowGameObject(const char* prefix, int uid, GameObject& child)
{
	ImGui::PushID(uid);                      // Use object uid as identifier. Most commonly you could also use the object pointer as a base ID.
	ImGui::AlignTextToFramePadding();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
	const std::vector<std::shared_ptr<GameObject>>& children = child.GetChildren();
	if (children.size() > 0)
	{
		bool node_open = ImGui::TreeNodeEx(prefix, ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow);
		if (ImGui::IsItemClicked())
		{
			currentGameObject = &child;
			m_Inspector->SetGameObject(&child);
		}
							

		ImGui::NextColumn();
		if (node_open)
		{
			for (int i = 0; i < children.size(); i++)
				ShowGameObject(children[i]->GetName().c_str(), children[i]->GetObjectID(), *children[i]);
			ImGui::TreePop();
		}
	}
	else
	{
		ImGui::TreeNodeEx(prefix, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet);
		if (ImGui::IsItemClicked())
		{
			currentGameObject = &child;
			m_Inspector->SetGameObject(&child);
		}				
				
		ImGui::NextColumn();
	}

	ImGui::PopID();
}

void ImGuiLayer::OnEvent(Event& event)
{
	// only dispatch events to imgui when it is active; at the moment this means that the demo window is shown
	if (m_ReceiveEvents)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseButtonPressedEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<MouseMovedEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnMouseScrolledEvent));
		dispatcher.Dispatch<KeyPressedEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.Dispatch<KeyTypedEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnKeyTypedEvent));
		dispatcher.Dispatch<WindowResizeEvent>(FLOW_BIND_EVENT_FUNCTION(ImGuiLayer::OnWindowResizeEvent));		}
		
}

bool ImGuiLayer::OnMouseButtonPressedEvent(MouseButtonPressedEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[event.GetMouseButton()] = true;

	return m_ReceiveEvents;
}

bool ImGuiLayer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[event.GetMouseButton()] = false;

	return m_ReceiveEvents;
}

bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(event.GetX(), event.GetY());

	return m_ReceiveEvents;
}

bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel += event.GetYOffset();
	io.MouseWheelH += event.GetXOffset();

	return m_ReceiveEvents;
}

bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[event.GetKeyCode()] = true;

	io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
	io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
	io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
	return m_ReceiveEvents;
}

bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.KeysDown[event.GetKeyCode()] = false;

	return m_ReceiveEvents;
}

bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	int keycode = event.GetKeyCode();
	if (keycode > 0 && keycode < 0x10000)
		io.AddInputCharacter((unsigned short)keycode);

	return m_ReceiveEvents;
}

bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent & event)
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2((float)event.GetWidth(), (float)event.GetHeight());
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	glViewport(0, 0, event.GetWidth(), event.GetHeight());

	return m_ReceiveEvents;
}
