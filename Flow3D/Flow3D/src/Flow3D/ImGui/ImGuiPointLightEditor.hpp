#pragma once

#include <imgui/imgui.h>
#include "Flow3D/Components/Lighting.hpp"
#include "ImGuiBaseLightEditor.hpp"
#include "ImGuiAttenuationEditor.hpp"

struct PointLightEditor
{

	PointLightEditor() {}

	void Draw(PointLight* pl)
	{
		if (pl != nullptr)
		{
			BaseLightEditor editor = BaseLightEditor();
			editor.Draw(dynamic_cast<BaseLight*>(pl), "PointLight");

			AttenuationEditor attEditor = AttenuationEditor();
			attEditor.Draw(pl->GetAttenuation(), "PointLight");
		}			
	}
};

