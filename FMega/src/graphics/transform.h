#pragma once

#include "fmega.h"

namespace fmega {

	struct Transform
	{
		glm::vec3 position = glm::vec3(0);
		glm::vec3 rotation = glm::vec3(0);
		glm::vec3 scale = glm::vec3(1);

		glm::mat4 GetModelMatrix() const
		{
			glm::mat4 model = 
				glm::translate(glm::mat4(1), position) * 
				glm::eulerAngleXYZ(rotation.x, rotation.y, rotation.z) * 
				glm::scale(glm::mat4(1), scale);
			return model;
		}
	};

}