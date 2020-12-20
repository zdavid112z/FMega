#pragma once

#include "vector.h"

namespace fmega {

	template <typename T>
	class string
	{
	public:
		string();
		string(const string<T>& other);
		string(string<T>&& other);
		string(const T* begin, uint32 count);
		string()
	};

}