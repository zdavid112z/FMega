#pragma once

#include "shader.h"
#include "texture2d.h"
#include "gpuBuffer.h"

namespace fmega {

	enum class MaterialType : uint32
	{
		DEFAULT
	};

	struct DefaultMaterialData
	{
		glm::vec4 diffuseColor;
		glm::vec4 specularColor;
		float shininess;
		float unused1;
		float unused2;
		float unused3;
	};

	struct Material
	{
		MaterialType type;
		Shader* shader;
		std::vector<Texture2D*> textures;
		GPUBuffer* buffer;
		byte data[1024];
		uint32 dataSize;

		~Material()
		{
			delete buffer;
		}
	};

}
