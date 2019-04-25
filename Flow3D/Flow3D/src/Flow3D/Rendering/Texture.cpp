#include "Texture.hpp"

#include "Flow3D\Log.hpp"

#include "glad\glad.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image\stb_image.h"

namespace Flow {

	Texture::Texture()
	{

	}

	Texture::Texture(std::string path, std::string typeName)
		: path(path), type(typeName)
	{
		id = LoadTextureFromFile(path.c_str());
	}

	Texture::~Texture()
	{

	}

	unsigned int Texture::LoadTextureFromFile(const char *path)
	{
		unsigned int textureID;
		glGenTextures(1, &textureID);

		int width, height, numberComponents;
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(path, &width, &height, &numberComponents, 0);
		if (data)
		{
			GLenum format;
			if (numberComponents == 1)
				format = GL_RED;
			else if (numberComponents == 3)
				format = GL_RGB;
			else if (numberComponents == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// TODO: be able to customize parameter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			stbi_image_free(data);
		}
		else
		{
			FLOW_CORE_ERROR("Failed to load texture at {0}", path);
			stbi_image_free(data);
		}

		return textureID;
	}
}