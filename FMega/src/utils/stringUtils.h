#pragma once

#include "type.h"
#include <string>
#include <glm/glm.hpp>

namespace fmega {

	class StringUtils
	{
	public:
		static std::string ToString(float val) { return std::to_string(val); }
		static std::string ToString(double val) { return std::to_string(val); }
		static std::string ToString(long double val) { return std::to_string(val); }
		static std::string ToString(int32 val) { return std::to_string(val); }
		static std::string ToString(uint32 val) { return std::to_string(val); }
		static std::string ToString(int64 val) { return std::to_string(val); }
		static std::string ToString(uint64 val) { return std::to_string(val); }
		static std::string ToString(const glm::vec2& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")"; }
		static std::string ToString(const glm::vec3& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")"; }
		static std::string ToString(const glm::vec4& v) { return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w) + ")"; }
		static std::string ToString(const glm::mat3& v) { return "[ " + ToString(v[0]) + ",\n  " + ToString(v[1]) + ",\n  " + ToString(v[2]) + " ]"; }
		static std::string ToString(const glm::mat4& v) { return "[ " + ToString(v[0]) + ",\n  " + ToString(v[1]) + ",\n  " + ToString(v[2]) + ",\n  " + ToString(v[3]) + " ]"; }
		static std::string ToString(char v) { return std::string(&v, 1); }
		static std::string ToString(const std::string& val) { return val; }
		static std::string ToString(const char* val) { return std::string(val); }
		template <class T>
		static std::string ToString(const std::vector<T>& v)
		{
			std::string result = "(";
			if (v.size() > 0)
				result.append(StringUtils::ToString(v[0]));
			for (size_t i = 1; i < v.size(); i++)
			{
				result.push_back(',');
				result.push_back(' ');
				result.append(StringUtils::ToString(v[i]));
			}
			result.push_back(')');
			return result;
		}
		static std::string ToString(void* val) { 
			std::stringstream ss;
			ss << std::hex << reinterpret_cast<size_t>(val);
			return ss.str();
		}

		static bool StartsWith(const std::string& str, const std::string& prefix) {
			return str.size() >= prefix.size() && memcmp(str.data(), prefix.data(), prefix.size()) == 0;
		}
	};

}
