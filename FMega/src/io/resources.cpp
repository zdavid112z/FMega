#include "fmega.h"
#include "resources.h"

namespace fmega {

	pugi::xml_document Resources::document;
	Resources::ResourceMap<Shader> Resources::shaderMap = Resources::ResourceMap<Shader>("shaders");
	Resources::ResourceMap<Texture2D> Resources::texture2dMap = Resources::ResourceMap<Texture2D>("textures2d");
	Resources::ResourceMap<Mesh> Resources::meshMap = Resources::ResourceMap<Mesh>("meshes");

}