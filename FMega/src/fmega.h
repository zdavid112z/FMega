#pragma once

#include "macros.h"

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <stack>
#include <queue>
#include <functional>

#include <cstdlib>
#include <cstring>
#include <cmath>
#include <sstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/euler_angles.hpp>

#ifdef _DEBUG 
#     define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__) 
#     define new DEBUG_NEW 

#     ifndef malloc 
#          define malloc( size ) _malloc_dbg( size, _NORMAL_BLOCK, __FILE__, __LINE__) 
#     endif 
#endif  

#include "utils/type.h"
#include "utils/log.h"