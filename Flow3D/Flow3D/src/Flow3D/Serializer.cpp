#include "Serializer.hpp"

#include "Flow3D/ResourceManager.hpp"
#include "Flow3D/Components/ComponentManager.hpp"

#include <io.h>     // For access().
#include <sys/types.h>  // For stat().
#include <sys/stat.h>   // For stat().
#include <filesystem>

void Serializer::LoadSceneNames()
{
	std::string path = "serialization";
	for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
	{
		std::string fileExtension = ".scene";
		if (entry.path().extension() == fileExtension)
		{
			std::string path_string = entry.path().u8string();
			size_t fileExtensionPosition = path_string.find(".scene");
			std::string sceneName = path_string.substr(path.size() + 1, path_string.size() - path.size() - 1 - fileExtension.size());
			ResourceManager::Get().AddSceneName(sceneName);
			FLOW_CORE_INFO("scene added to scene names");
		}
	}
	FLOW_CORE_INFO("load scene names ending");
}

void Serializer::Serialize(Scene& scene)
{
	GameObject& rootObject = scene.GetRoot();
	json rootAsJSON = rootObject;

	std::ofstream myfile;

	CreateDirectory("serialization", NULL);
	
	json sceneAsJson = scene;

	std::string directory = "serialization/" + scene.GetName();
	std::string sceneFilename = scene.GetName() + ".scene";
	std::string scenePath = "serialization/" + sceneFilename;
	myfile.open(scenePath.c_str());
	myfile << std::setw(4) << sceneAsJson;
	myfile.close();

	CreateDirectory(directory.c_str(), NULL);

	directory = directory + "/";
	std::string filename = rootObject.GetName();
	directory = directory + filename;
	CreateDirectory(directory.c_str(), NULL);
	filename.append(".json");
	std::string path = directory + "/" + filename;
	myfile.open(path.c_str());
	myfile << std::setw(4) << rootAsJSON;
	myfile.close();

	const std::vector<std::shared_ptr<GameObject>>& rootChildren = rootObject.GetChildren();
	SerializeChildren(rootChildren, directory, myfile);

	SerializeResources(myfile);
	
}

void Serializer::Deserialize(Scene& scene)
{
	DeserializeResources();
	FLOW_CORE_INFO("resources deserialized");
	std::string sceneName = scene.GetName();
	std::string serializationDirectory = "serialization\\" + sceneName;

	if (std::experimental::filesystem::exists(serializationDirectory.c_str()))
	{
		FLOW_CORE_INFO("scene directory exists");
		std::string sceneFilepath = serializationDirectory + ".scene";
		std::ifstream sceneFile(sceneFilepath);
		json sceneAsJson;
		sceneFile >> sceneAsJson;
		auto loadedScene = sceneAsJson.get<Scene>();
		scene.SetBackgroundColor(loadedScene.GetBackgroundColor());
		scene.SetSkybox(ResourceManager::Get().FindSkybox(loadedScene.m_SkyboxName));
	}

	GameObject& root = scene.GetRoot();
	if (std::experimental::filesystem::exists(serializationDirectory.c_str()))
	{		
		std::string rootDirectory = serializationDirectory + "\\root";
		if (std::experimental::filesystem::exists(rootDirectory.c_str()))
		{
			std::vector<const char*> allComponentNames = ComponentManager::GetAllComponentNames();
			std::vector<std::shared_ptr<GameObject>> gameObjectsWithGameObjectToggler;
			// find the children of the root object -> all GameObjects in the scene

			DeserializeChildren(rootDirectory, root, allComponentNames, scene, gameObjectsWithGameObjectToggler);

			// after all GameObjects are created the GameObjectToggler needs to add its entries
			for (unsigned int i = 0; i < gameObjectsWithGameObjectToggler.size(); i++)
			{
				GameObjectToggler& gameObjectToggler = gameObjectsWithGameObjectToggler[i]->GetComponent<GameObjectToggler>();
				if (&gameObjectToggler != nullptr)
				{
					std::vector<std::tuple<std::string, int>>& gameObjectsToToggle = gameObjectToggler.gameObjectsToToggle;

					for (unsigned int j = 0; j < gameObjectsToToggle.size(); j++)
					{
						std::string gameObjectName = std::get<0>(gameObjectsToToggle[j]);
						Keycode keycode = static_cast<Keycode>(std::get<1>(gameObjectsToToggle[j]));

						GameObject* gameObjectToToggle = scene.FindGameObject(gameObjectName);
						gameObjectsWithGameObjectToggler[i]->GetComponent<GameObjectToggler>().AddGameObjectToToggle(std::make_tuple(gameObjectToToggle,
							gameObjectName, keycode), true);
					}
				}
			}
		}
	}
}

void Serializer::SerializeRotatable(std::ofstream & myfile, Component* component)
{
	json componentAsJson = *dynamic_cast<Rotatable*>(component);
	myfile << std::setw(4) << componentAsJson;
	myfile.close();
}

void Serializer::SerializeFreeCamera(std::ofstream & myfile, Component* component)
{
	json componentAsJson = *dynamic_cast<FreeCamera*>(component);
	myfile << std::setw(4) << componentAsJson;
	myfile.close();
}

void Serializer::SerializeGameObjectToggler(std::ofstream & myfile, Component* component)
{
	json componentAsJson = *dynamic_cast<GameObjectToggler*>(component);
	myfile << std::setw(4) << componentAsJson;
	myfile.close();
}

void Serializer::SerializeComponentToggler(std::ofstream & myfile, Component* component)
{
	json componentAsJson = *dynamic_cast<ComponentToggler*>(component);
	myfile << std::setw(4) << componentAsJson;
	myfile.close();
}

void Serializer::SerializeDirectionalLight(std::ofstream & myfile, Component* component)
{
	json componentAsJson = *dynamic_cast<DirectionalLight*>(component);
	myfile << std::setw(4) << componentAsJson;
	myfile.close();
}

void Serializer::SerializePointLight(std::ofstream & myfile, Component* component)
{
	json componentAsJson = *dynamic_cast<PointLight*>(component);
	myfile << std::setw(4) << componentAsJson;
	myfile.close();
}

void Serializer::SerializeSpotLight(std::ofstream & myfile, Component* component)
{
	json componentAsJson = *dynamic_cast<SpotLight*>(component);
	myfile << std::setw(4) << componentAsJson;
	myfile.close();
}

void Serializer::SerializeRenderable(std::ofstream & myfile, Component* component, const std::string& componentDirectory)
{
	Renderable& renderableComponent = *dynamic_cast<Renderable*>(component);
	json componentAsJson = renderableComponent;
	myfile << std::setw(4) << componentAsJson;
	myfile.close();

	std::string shaderDirectory = componentDirectory + "/Shader";
	CreateDirectory(shaderDirectory.c_str(), NULL);
	std::string shaderPath = shaderDirectory + "/shader.json";
	myfile.open(shaderPath.c_str());
	json shaderAsJson = renderableComponent.GetShader();
	myfile << std::setw(4) << shaderAsJson;
	myfile.close();

	std::string modelDirectory = componentDirectory + "/Model";
	CreateDirectory(modelDirectory.c_str(), NULL);
	std::string modelPath = modelDirectory + "/model.json";
	myfile.open(modelPath.c_str());
	Model& model = renderableComponent.GetModel();
	json modelAsJson = model;
	myfile << std::setw(4) << modelAsJson;
	myfile.close();

	if (model.GetCube() != nullptr)
	{
		std::string cubeDirectory = modelDirectory + "/Cube";
		CreateDirectory(cubeDirectory.c_str(), NULL);
		std::string cubePath = cubeDirectory + "/cube.json";
		Cube& cube = *model.GetCube().get();
		json cubeAsJson = cube;
		myfile.open(cubePath.c_str());
		myfile << std::setw(4) << cubeAsJson;
		myfile.close();

		if (cube.GetIsTextured())
		{
			std::string texturesDirectory = cubeDirectory + "/Textures";
			CreateDirectory(texturesDirectory.c_str(), NULL);

			std::string diffuseTexturePath = texturesDirectory + "/diffuse.json";
			json diffuseAsJson = cube.GetDiffuseTexture();
			myfile.open(diffuseTexturePath);
			myfile << std::setw(4) << diffuseAsJson;
			myfile.close();

			std::string specularTexturePath = texturesDirectory + "/specular.json";
			json specularAsJson = cube.GetSpecularTexture();
			myfile.open(specularTexturePath);
			myfile << std::setw(4) << specularAsJson;
			myfile.close();
		}
	}
	else if (model.GetPlane() != nullptr)
	{
		std::string planeDirectory = modelDirectory + "/Plane";
		CreateDirectory(planeDirectory.c_str(), NULL);
		std::string planePath = planeDirectory + "/plane.json";
		Plane& plane = *model.GetPlane().get();
		json planeAsJson = plane;
		myfile.open(planePath.c_str());
		myfile << std::setw(4) << planeAsJson;
		myfile.close();

		if (plane.GetIsTextured())
		{
			std::string texturesDirectory = planeDirectory + "/Textures";
			CreateDirectory(texturesDirectory.c_str(), NULL);

			std::string diffuseTexturePath = texturesDirectory + "/diffuse.json";
			json diffuseAsJson = plane.GetDiffuseTexture();
			myfile.open(diffuseTexturePath);
			myfile << std::setw(4) << diffuseAsJson;
			myfile.close();

			std::string specularTexturePath = texturesDirectory + "/specular.json";
			json specularAsJson = plane.GetSpecularTexture();
			myfile.open(specularTexturePath);
			myfile << std::setw(4) << specularAsJson;
			myfile.close();
		}
	}
}

void Serializer::DeserializeRotatable(json& json, GameObject& gameObject, Scene & scene)
{
	auto rotatable = json.get<Rotatable>();
	gameObject.AddComponent<Rotatable>(&gameObject, rotatable.GetEnabled());
}

void Serializer::DeserializeFreeCamera(json & json, GameObject & gameObject, Scene & scene)
{
	auto freeCamera = json.get<FreeCamera>();
	gameObject.AddComponent<FreeCamera>(&gameObject, Application::Get().GetWindow(), freeCamera.GetEnabled(), freeCamera.m_Yaw, freeCamera.m_Pitch);
	FreeCamera& freeCameraComponent = gameObject.GetComponent<FreeCamera>();
	freeCameraComponent.SetMovementSpeed(freeCamera.GetMovementSpeed());
	freeCameraComponent.SetMouseSensitivity(freeCamera.GetMouseSensitivity());
	freeCameraComponent.SetZoom(freeCamera.GetZoom());
	freeCameraComponent.SetZNear(freeCamera.GetZNear());
	freeCameraComponent.SetZFar(freeCamera.GetZFar());
	freeCameraComponent.SetIsActive(freeCamera.m_IsActive);
}

void Serializer::DeserializeGameObjectToggler(json & json, GameObject & gameObject, Scene & scene, std::vector<std::shared_ptr<GameObject>>& gameObjectsWithGameObjectToggler)
{
	auto gameObjectToggler = json.get<GameObjectToggler>();
	gameObject.AddComponent<GameObjectToggler>(&gameObject, gameObjectToggler.GetEnabled());
	gameObject.GetComponent<GameObjectToggler>().gameObjectsToToggle = gameObjectToggler.gameObjectsToToggle;

	gameObjectsWithGameObjectToggler.push_back(std::make_shared<GameObject>(gameObject));
}

void Serializer::DeserializeComponentToggler(json & json, GameObject & gameObject, Scene & scene)
{
	auto componentToggler = json.get<ComponentToggler>();
	gameObject.AddComponent<ComponentToggler>(&gameObject, componentToggler.GetEnabled());
	gameObject.GetComponent<ComponentToggler>().componentsToToggle = componentToggler.componentsToToggle;
}

void Serializer::DeserializeDirectionalLight(json & json, GameObject & gameObject, Scene & scene)
{
	auto directionalLight = json.get<DirectionalLight>();
	gameObject.AddComponent<DirectionalLight>(&gameObject, directionalLight.m_Direction,
		directionalLight.m_Ambient, directionalLight.m_Diffuse, directionalLight.m_Specular, directionalLight.GetEnabled(), directionalLight.m_SetAsSceneLight);
	if (directionalLight.m_SetAsSceneLight)
		scene.SetDirectionalLight(&gameObject.GetComponent<DirectionalLight>());
}

void Serializer::DeserializePointLight(json & json, GameObject & gameObject, Scene & scene)
{
	auto pointLight = json.get<PointLight>();
	gameObject.AddComponent<PointLight>(&gameObject, pointLight.m_Ambient, pointLight.m_Diffuse, pointLight.m_Specular,
		pointLight.GetAttenuation(), pointLight.GetEnabled());
	scene.AddPointLight(&gameObject.GetComponent<PointLight>());
}

void Serializer::DeserializeSpotLight(json & json, GameObject & gameObject, Scene & scene)
{
	auto spotLight = json.get<SpotLight>();
	gameObject.AddComponent<SpotLight>(&gameObject, spotLight.m_Ambient, spotLight.m_Diffuse, spotLight.m_Specular,
		spotLight.m_Cutoff, spotLight.m_OuterCutoff, spotLight.GetAttenuation(), spotLight.GetEnabled());
	scene.AddSpotLight(&gameObject.GetComponent<SpotLight>());
}

void Serializer::DeserializeRenderable(json & componentAsJson, GameObject & gameObject, Scene & scene, const std::string & componentsDirectory)
{
	auto renderable = componentAsJson.get<Renderable>();
	std::string shaderName = GetShaderName(componentsDirectory);
	if (shaderName.empty())
		FLOW_CORE_ERROR("Shader not found");

	std::string modelFilepath = GetModelFilepath(componentsDirectory);
	if (modelFilepath.empty())
	{
		// is cube or plane or error
		std::string cubeDirectory = componentsDirectory + "\\Model\\Cube";
		std::string planeDirectory = componentsDirectory + "\\Model\\Plane";
		if (std::experimental::filesystem::exists(cubeDirectory))
		{
			std::string cubePath = cubeDirectory + "\\cube.json";
			std::ifstream cubeFile(cubePath);
			json cubeAsJson;
			cubeFile >> cubeAsJson;
			auto cube = cubeAsJson.get<Cube>();
			if (cube.GetIsTextured())
			{
				// Get the textures
				std::string texturesDirectory = cubeDirectory + "\\Textures";
				std::string diffusePath;
				std::string specularPath;
				if (std::experimental::filesystem::exists(texturesDirectory))
				{
					diffusePath = GetTexturePath(texturesDirectory, "diffuse");
					specularPath = GetTexturePath(texturesDirectory, "specular");
				}
				else
					FLOW_CORE_ERROR("no textures found");

				gameObject.AddComponent<Renderable>(&gameObject,
					std::make_shared<Model>(std::make_shared<Cube>(ResourceManager::Get().FindTexture(diffusePath),
						ResourceManager::Get().FindTexture(specularPath))),
					ResourceManager::Get().FindShader(shaderName), renderable.GetBlending(), renderable.GetEnabled());
			}
			else
			{
				gameObject.AddComponent<Renderable>(&gameObject,
					std::make_shared<Model>(std::make_shared<Cube>(cube.GetColor())), ResourceManager::Get().FindShader(shaderName),
					renderable.GetBlending(), renderable.GetEnabled());
			}
		}
		else if (std::experimental::filesystem::exists(planeDirectory))
		{
			std::string planePath = planeDirectory + "\\plane.json";
			std::ifstream planeFile(planePath);
			json planeAsJson;
			planeFile >> planeAsJson;
			auto plane = planeAsJson.get<Plane>();
			if (plane.GetIsTextured())
			{
				// Get the textures
				std::string texturesDirectory = planeDirectory + "\\Textures";
				std::string diffusePath;
				std::string specularPath;
				if (std::experimental::filesystem::exists(texturesDirectory))
				{
					diffusePath = GetTexturePath(texturesDirectory, "diffuse");
					specularPath = GetTexturePath(texturesDirectory, "specular");
				}
				else
					FLOW_CORE_ERROR("no textures found");

				gameObject.AddComponent<Renderable>(&gameObject,
					std::make_shared<Model>(std::make_shared<Plane>(ResourceManager::Get().FindTexture(diffusePath),
						ResourceManager::Get().FindTexture(specularPath))),
					ResourceManager::Get().FindShader(shaderName), renderable.GetBlending(), renderable.GetEnabled());
			}
			else
			{
				gameObject.AddComponent<Renderable>(&gameObject,
					std::make_shared<Model>(std::make_shared<Plane>(plane.GetColor())), ResourceManager::Get().FindShader(shaderName),
					renderable.GetBlending(), renderable.GetEnabled());
			}
		}
		else
			FLOW_CORE_ERROR("Model not found");
	}
	else
	{
		gameObject.AddComponent<Renderable>(&gameObject, ResourceManager::Get().FindModel(modelFilepath),
			ResourceManager::Get().FindShader(shaderName), renderable.GetBlending(), renderable.GetEnabled());
	}
}

void Serializer::SerializeChildren(const std::vector<std::shared_ptr<GameObject>>& rootChildren, std::string directory, std::ofstream & myfile)
{
	for (unsigned int i = 0; i < rootChildren.size(); i++)
	{
		std::string newDirectory = directory + "/" + rootChildren[i]->GetName();
		CreateDirectory(newDirectory.c_str(), NULL);
		std::string filename = rootChildren[i]->GetName();
		filename.append(".json");
		std::string path = newDirectory + "/" + filename;
		myfile.open(path.c_str());
		json childAsJson = *rootChildren[i].get();
		myfile << std::setw(4) << childAsJson;
		myfile.close();

		// Serialize all components of the GameObject
		const std::vector<std::shared_ptr<Component>>& components = rootChildren[i]->GetComponents();
		std::string componentDirectory = newDirectory + "/" + rootChildren[i]->GetName() + "_components";
		if (components.size() > 0)
			CreateDirectory(componentDirectory.c_str(), NULL);

		for (unsigned int j = 0; j < components.size(); j++)
		{
			const std::string componentName = components[j]->GetName();
			std::string componentFileName = componentName;
			componentFileName.append(".json");
			std::string componentPath = componentDirectory + "/" + componentFileName;
			myfile.open(componentPath.c_str());

			ComponentManager::SerializeComponent(componentName, myfile, components[j].get(), componentDirectory);			
		}

		if (rootChildren[i]->GetChildren().size() > 0)
			SerializeChildren(rootChildren[i]->GetChildren(), newDirectory, myfile);
	}
}

void Serializer::DeserializeChildren(const std::string rootDirectory, GameObject& parent, std::vector<const char*>& allComponentNames, Scene& scene, std::vector<std::shared_ptr<GameObject>>& gameObjectsWithGameObjectToggler)
{
	std::vector<std::string> rootDirectories = get_directories(rootDirectory);
	for (unsigned int i = 0; i < rootDirectories.size(); i++)
	{
		std::size_t lastBackSlashPosition = rootDirectories[i].find_last_of("\\");
		if (static_cast<int>(lastBackSlashPosition) > 0)
		{
			if (rootDirectories[i].find("_components") == std::string::npos)
			{
				std::string gameObjectName = rootDirectories[i].substr(lastBackSlashPosition + 1);
				// load json of the GameObject
				std::string filepath = rootDirectories[i] + "\\" + gameObjectName + ".json";
				std::ifstream gameObjectFile(filepath);
				json gameObjectJson;
				gameObjectFile >> gameObjectJson;

				// Create GameObject and add it to root
				auto gameObjectFromJson = gameObjectJson.get<GameObject>();
				std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>(gameObjectFromJson.m_Name,
					gameObjectFromJson.m_Transform.m_Position, gameObjectFromJson.m_Transform.m_Orientation,
					gameObjectFromJson.m_Transform.m_Scale, gameObjectFromJson.m_IsActive);
				gameObject->GetTransform().SetIsCamera(gameObjectFromJson.m_Transform.GetIsCamera());
				gameObject->GetTransform().ConstrainPosition(gameObjectFromJson.m_Transform.constrainPositionX, gameObjectFromJson.m_Transform.constrainPositionY,
					gameObjectFromJson.m_Transform.constrainPositionZ);
				gameObject->GetTransform().ConstrainRotation(gameObjectFromJson.m_Transform.constrainRotationX, gameObjectFromJson.m_Transform.constrainRotationY,
					gameObjectFromJson.m_Transform.constrainRotationZ);
				gameObject->GetTransform().ConstrainScale(gameObjectFromJson.m_Transform.constrainScaleX, gameObjectFromJson.m_Transform.constrainScaleY,
					gameObjectFromJson.m_Transform.constrainScaleZ);
				parent.AddChild(gameObject);

				// Check for components and add them to the GameObject
				std::string componentsDirectory = rootDirectories[i] + "\\" + gameObjectName + "_components";
				if (std::experimental::filesystem::exists(componentsDirectory.c_str()))
				{
					for (unsigned int j = 0; j < allComponentNames.size(); j++)
					{
						std::string componentFilepath = componentsDirectory + "\\" + allComponentNames[j] + ".json";
						if (std::experimental::filesystem::exists(componentFilepath))
						{
							// maybe check if the file is empty?
							std::ifstream componentFile(componentFilepath);
							json componentAsJson;
							componentFile >> componentAsJson;

							ComponentManager::DeserializeComponent(allComponentNames[j], componentAsJson, *gameObject, scene, componentsDirectory, gameObjectsWithGameObjectToggler);
						}
					}
				}

				// after all components are added the ComponentToggler needs to add its entries
				ComponentToggler& componentToggler = gameObject->GetComponent<ComponentToggler>();
				if (&componentToggler != nullptr)
				{
					std::vector<std::tuple<std::string, int>>& componentsToToggle = componentToggler.componentsToToggle;

					const std::vector<std::shared_ptr<Component>>& components = gameObject->GetComponents();
					std::vector<std::string> componentNames;
					for (unsigned int j = 0; j < components.size(); j++)
					{
						Component& component = *components[j];
						std::string componentName = component.GetName();
						componentNames.push_back(componentName);
					}

					for (unsigned int j = 0; j < componentsToToggle.size(); j++)
					{
						std::string componentName = std::get<0>(componentsToToggle[j]);
						Keycode keycode = static_cast<Keycode>(std::get<1>(componentsToToggle[j]));

						for (unsigned int k = 0; k < componentNames.size(); k++)
						{
							if (componentName == componentNames[k])
								componentToggler.AddComponentToToggle(std::make_tuple(components[k].get(), keycode), true);
						}
					}
				}

				// recursively for children, create them and add them to the GameObject
				DeserializeChildren(rootDirectories[i], *gameObject, allComponentNames, scene, gameObjectsWithGameObjectToggler);
			}			
		}
	}
}

std::vector<std::string> Serializer::get_directories(const std::string & s)
{
	std::vector<std::string> r;
	for (auto& p : std::experimental::filesystem::recursive_directory_iterator(s))
		if (p.status().type() == std::experimental::filesystem::file_type::directory)
			r.push_back(p.path().string());

	std::vector<std::string> directSubDirectories;
	for (unsigned int i = 0; i < r.size(); i++)
	{
		std::size_t sizeOfCurrentDirectoryName = s.size();
		std::size_t stringPosition = r[i].find(s);
		std::size_t slashAfterRoot = r[i].find("\\", stringPosition + sizeOfCurrentDirectoryName + 2); // +2 because of the backslash and the offest
		if (static_cast<int>(slashAfterRoot) < 0)
			directSubDirectories.push_back(r[i]);
	}
	return directSubDirectories;
}

std::string Serializer::GetShaderName(const std::string & componentsDirectory)
{
	// get shader
	std::string shaderDirectory = componentsDirectory + "\\Shader";
	if (std::experimental::filesystem::exists(shaderDirectory))
	{
		std::string shaderPath = shaderDirectory + "\\Shader.json";
		if (std::experimental::filesystem::exists(shaderPath))
		{
			std::ifstream shaderFile(shaderPath);
			json shaderAsJson;
			shaderFile >> shaderAsJson;
			auto shader = shaderAsJson.get<Shader>();
			return shader.m_Name;
		}
	}
	return "";
}

std::string Serializer::GetModelFilepath(const std::string & componentsDirectory)
{
	// Get Model
	std::string modelDirectory = componentsDirectory + "\\Model";
	if (std::experimental::filesystem::exists(modelDirectory))
	{
		std::string modelPath = modelDirectory + "\\model.json";
		if (std::experimental::filesystem::exists(modelPath))
		{
			std::ifstream modelFile(modelPath);
			json modelAsJson;
			modelFile >> modelAsJson;
			auto model = modelAsJson.get<Model>();
			return model.filepath;
		}
	}
	return "";
}

std::string Serializer::GetTexturePath(const std::string & texturesDirectory, const std::string & type)
{
	std::string texturePath = texturesDirectory + "\\" + type + ".json";
	if (std::experimental::filesystem::exists(texturePath))
	{
		std::ifstream textureFile(texturePath);
		json textureAsJson;
		textureFile >> textureAsJson;
		auto texture = textureAsJson.get<Texture>();
		return texture.path;
	}

	return "";
}

void Serializer::SerializeResources(std::ofstream& myfile)
{
	CreateDirectory("serialization/resources", NULL);

	SerializeTextures(myfile);
	SerializeShaders(myfile);
	SerializeModels(myfile);
	SerializeSkyboxes(myfile);
}

void Serializer::SerializeTextures(std::ofstream& myfile)
{
	CreateDirectory("serialization/resources/textures", NULL);

	std::vector<std::shared_ptr<Texture>> textures = ResourceManager::Get().GetAllTextures();
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		json textureAsJson = *textures[i];

		std::string textureFilepath = "serialization/resources/textures/" + textures[i]->name + ".texture";
		myfile.open(textureFilepath.c_str());
		myfile << std::setw(4) << textureAsJson;
		myfile.close();
	}	
}

void Serializer::SerializeShaders(std::ofstream& myfile)
{
	CreateDirectory("serialization/resources/shaders", NULL);

	std::vector<std::shared_ptr<Shader>> shaders = ResourceManager::Get().GetAllShaders();
	for (unsigned int i = 0; i < shaders.size(); i++)
	{
		json shaderAsJson = *shaders[i];
		
		std::string shaderFilepath = "serialization/resources/shaders/" + shaders[i]->m_Name + ".shader";
		myfile.open(shaderFilepath.c_str());
		myfile << std::setw(4) << shaderAsJson;
		myfile.close();
	}
}

void Serializer::SerializeModels(std::ofstream& myfile)
{
	CreateDirectory("serialization/resources/models", NULL);

	std::vector<std::shared_ptr<Model>> models = ResourceManager::Get().GetAllModels();
	for (unsigned int i = 0; i < models.size(); i++)
	{
		json modelsAsJson = *models[i];

		std::string modelFilepath = "serialization/resources/models/" + models[i]->name + ".model";
		myfile.open(modelFilepath.c_str());
		myfile << std::setw(4) << modelsAsJson;
		myfile.close();
	}	
}

void Serializer::SerializeSkyboxes(std::ofstream& myfile)
{
	CreateDirectory("serialization/resources/skyboxes", NULL);

	std::vector<std::shared_ptr<Skybox>> skyboxes = ResourceManager::Get().GetAllSkyboxes();
	for (unsigned int i = 0; i < skyboxes.size(); i++)
	{
		json skyboxAsJson = *skyboxes[i];

		std::string skyboxFilepath = "serialization/resources/skyboxes/" + skyboxes[i]->m_Name + ".skybox";
		myfile.open(skyboxFilepath.c_str());
		myfile << std::setw(4) << skyboxAsJson;
		myfile.close();
	}
}

void Serializer::DeserializeResources()
{
	DeserializeTextures();
	DeserializeShaders();
	DeserializeModels();
	DeserializeSkyboxes();
}

void Serializer::DeserializeTextures()
{
	std::string path = "serialization/resources/textures";
	for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
	{
		// load json of the skybox		
		std::ifstream textureFile(entry.path());
		json textureJson;
		textureFile >> textureJson;

		// Create Model and add it to the resources
		auto texture = textureJson.get<Texture>();
		ResourceManager::Get().AddTexture(std::make_shared<Texture>(texture.path, texture.type, texture.m_Flip));
	}		
}

void Serializer::DeserializeShaders()
{
	std::string path = "serialization/resources/shaders";
	for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
	{
		// load json of the skybox		
		std::ifstream shaderFile(entry.path());
		json shaderJson;
		shaderFile >> shaderJson;

		// Create Model and add it to the resources
		auto shaderFromJson = shaderJson.get<Shader>();
		ResourceManager::Get().AddShader(std::make_shared<Shader>(shaderFromJson.m_VertexPath.c_str(), shaderFromJson.m_FragmentPath.c_str(), shaderFromJson.m_Name));
	}
}

void Serializer::DeserializeModels()
{
	std::string path = "serialization/resources/models";
	for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
	{
		// load json of the model		
		std::ifstream modelFile(entry.path());
		json modelJson;
		modelFile >> modelJson;

		// Create Model and add it to the resources
		auto modelFromJson = modelJson.get<Model>();
		ResourceManager::Get().AddModel(std::make_shared<Model>(modelFromJson.filepath));
	}
}

void Serializer::DeserializeSkyboxes()
{
	std::string path = "serialization/resources/skyboxes";
	for (const auto & entry : std::experimental::filesystem::directory_iterator(path))
	{
		// load json of the skybox		
		std::ifstream skyboxFile(entry.path());
		json skyboxJson;
		skyboxFile >> skyboxJson;

		// Create Model and add it to the resources
		auto skyboxFromJson = skyboxJson.get<Skybox>();
		ResourceManager::Get().AddSkybox(std::make_shared<Skybox>(skyboxFromJson.m_Directory, skyboxFromJson.m_Filetype, skyboxFromJson.m_Name, skyboxFromJson.m_Show));
	}
}
