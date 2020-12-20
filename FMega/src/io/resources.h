#pragma once

#include "pugixml.hpp"
#include <graphics/texture2d.h>
#include <graphics/shader.h>
#include <graphics/mesh.h>
#include <graphics/material.h>

namespace fmega
{

	class Resources
	{
	public:
		static void Init()
		{
			pugi::xml_parse_result result = document.load_file("assets/resources.xml");
			ASSERT(result, std::string("Could not load resource file xml\n") + result.description());
			ParseNames(shaderMap);
			ParseNames(meshMap);
			ParseNames(texture2dMap);
		}

		static void Destroy()
		{
			shaderMap.Destroy();
			meshMap.Destroy();
			texture2dMap.Destroy();
		}

		template <typename T>
		static T* CreateResource(const pugi::xml_node& n)
		{
			return nullptr;
		}

		template <>
		static Shader* CreateResource(const pugi::xml_node& n)
		{
			std::string vertPath = std::string("assets/shaders/") + n.child("vert").text().as_string();
			std::string fragPath = std::string("assets/shaders/") + n.child("frag").text().as_string();
			uint32 dynamicObjectBufferSize = n.child("dynamicObjectBufferSize").text().as_uint(0);
			return new Shader(vertPath, fragPath, dynamicObjectBufferSize);
		}

		template <>
		static Texture2D* CreateResource(const pugi::xml_node& n)
		{
			std::string path = std::string("assets/textures/") + n.child("path").text().as_string();
			TextureFilter filter = TextureFilter::LINEAR;
			auto filterNode = n.child("filter");
			if (filterNode != nullptr)
			{
				if (strcmp(filterNode.text().as_string(), "nearest") == 0)
					filter = TextureFilter::NEAREST;
				else if (strcmp(filterNode.text().as_string(), "linear") == 0)
					filter = TextureFilter::LINEAR;
				else LOG_WARN("Unknown filter type in texture ", path, ". Using linear");
			}
			TextureWrap wrap = TextureWrap::CLAMP_TO_EDGE;
			auto wrapNode = n.child("wrap");
			if (wrapNode != nullptr)
			{
				if (strcmp(wrapNode.text().as_string(), "clamp") == 0)
					wrap = TextureWrap::CLAMP_TO_EDGE;
				else if (strcmp(wrapNode.text().as_string(), "repeat") == 0)
					wrap = TextureWrap::REPEAT;
				else if (strcmp(wrapNode.text().as_string(), "mirroredRepeat") == 0)
					wrap = TextureWrap::MIRRORED_REPEAT;
				else LOG_WARN("Unknown wrap type in texture ", path, ". Using clamp");
			}
			bool mips = true;
			if (n.child("nomipmaps") != nullptr)
				mips = false;
			Image image = ImageIO::LoadImage(path);
			if (image.data == nullptr)
			{
				LOG_WARN("Could not load image ", path, " for resource texture2d");
				return nullptr;
			}
			Texture2D* tex = new Texture2D(image, filter, wrap, mips);
			delete image.data;
			return tex;
		}

		template <typename T>
		struct ResourceMap
		{
			ResourceMap(const std::string& name) : name(name) {}
			void Destroy()
			{
				for (T* res : resArray)
				{
					if (res != nullptr)
						delete res;
				}
				map.clear();
				resArray.clear();
				idToName.clear();
				nameToId.clear();
				nextId = 0;
			}
			std::unordered_map<std::string, T*> map;
			std::vector<T*> resArray;
			std::vector<std::string> idToName;
			std::unordered_map<std::string, uint32> nameToId;
			std::vector<std::string> names;
			uint32 nextId = 0;
			std::string name;
			T* GetResource(const std::string& resname)
			{
				auto it = map.find(resname);
				if (it != map.end())
					return it->second;
				pugi::xml_node node = document.child("resources").child(name.c_str()).find_child_by_attribute("name", resname.c_str());
				if (node == nullptr)
				{
					LOG_WARN("Could not find resource with name ", resname);
					return nullptr;
				}
				T* res = CreateResource<T>(node);
				map[resname] = res;
				pugi::xml_attribute idattribute = node.attribute("id");
				uint32 id;
				if (idattribute)
					id = idattribute.as_uint();
				else
				{
					id = nextId++;
					LOG_WARN("Resource with name ", resname, " does not have an id! Using ", id, " as its id");
				}

				if (resArray.size() <= id)
				{
					resArray.resize(id + 1, nullptr);
					idToName.resize(id + 1);
				}
				resArray[id] = res;
				idToName[id] = resname;
				nameToId[resname] = id;

				return res;
			}
			T* GetResource(uint32 id)
			{
				if (resArray.size() > id)
				{
					if (resArray[id] != nullptr)
						return resArray[id];
				}
				pugi::xml_node node = document.child("resources").child(name.c_str()).find_child_by_attribute("id", std::to_string(id).c_str());
				if (node == nullptr)
				{
					LOG_WARN("Could not find resource with id ", id);
					return nullptr;
				}
				T* res = CreateResource<T>(node);
				if (resArray.size() <= id)
				{
					resArray.resize(id + 1, nullptr);
					idToName.resize(id + 1);
				}
				resArray[id] = res;
				pugi::xml_attribute nameattribute = node.attribute("name");
				std::string resname;
				if (nameattribute)
				{
					resname = nameattribute.as_string();
				}
				else
				{
					resname = "Unnamed_" + std::to_string(id);
					LOG_WARN("Resource with id ", id, " does not have a name! Using ", resname, " as its name");
				}
				idToName[id] = resname;
				nameToId[resname] = id;
				map[resname] = res;
				return res;
			}
		};

		template <typename T>
		static ResourceMap<T>* GetResourceMap()
		{
			LOG_ERROR("Tried to get the resource map of a type that does not have one!");
			return nullptr;
		}

		template <>
		static ResourceMap<Shader>* GetResourceMap()
		{
			return &shaderMap;
		}

		template <>
		static ResourceMap<Mesh>* GetResourceMap()
		{
			return &meshMap;
		}

		template <>
		static ResourceMap<Texture2D>* GetResourceMap()
		{
			return &texture2dMap;
		}

		template <>
		static ResourceMap<Material>* GetResourceMap()
		{
			return &materialsMap;
		}

		template <typename T>
		static T* GetResource(const std::string& name)
		{
			return GetResourceMap<T>()->GetResource(name);
		}
		
		template <typename T>
		static T* GetResource(uint32 id)
		{
			return GetResourceMap<T>()->GetResource(id);;
		}

		template <typename T>
		static uint32 GetResourceId(const std::string& name)
		{
			auto resmap = GetResourceMap<T>();
			GetResource<T>(name);
			auto it = resmap->nameToId.find(name);
			DEBUG_ONLY(
				if (it == resmap->nameToId.end())
				{
					LOG_ERROR("Resource with name ", name, " from ResourceMap ", resmap->name, " does not exist!");
					return -1;
				}
			);
			return it->second;
		}

		template <typename T>
		static std::string GetResourceName(uint32 id)
		{
			auto resmap = GetResourceMap<T>();
			GetResource<T>(id);
			DEBUG_ONLY(
				if(resmap->idToName.size() <= id)
				{
					LOG_ERROR("Resource with id ", id , " from ResourceMap ", resmap->name, " does not exist!");
					return "";
				}
			);
			return resmap->idToName[id];
		}

		template <typename T>
		static void ParseNames(ResourceMap<T>& map)
		{
			uint32 nextId = 0;
			for(pugi::xml_node node = document.child("resources").child(map.name.c_str()).first_child(); node != nullptr; node = node.next_sibling())
			{
				auto attr = node.attribute("name");
				if (attr)
				{
					map.names.push_back(attr.as_string());
				}
				nextId = std::max(nextId, (uint32)node.attribute("id").as_uint(0) + 1);
			}
			map.nextId = nextId;
		}
		static ResourceMap<Shader> shaderMap;
		static ResourceMap<Texture2D> texture2dMap;
		static ResourceMap<Mesh> meshMap;
		static ResourceMap<Material> materialsMap;
	private:
		static pugi::xml_document document;
	};

}