#pragma once

#include "fmega.h"

namespace fmega {

	struct Camera
	{
		glm::mat4 viewProjection;
		glm::mat4 view, projection;
		glm::vec3 position;
	};

}