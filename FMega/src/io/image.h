#pragma once

#include "fmega.h"

namespace fmega {

	struct Image
	{
		int32 width, height, pitch, numComponents, bitsPerComponent;
		byte* data;
	};

	class ImageIO
	{
	public:
		static Image LoadImage(const std::string& path);
		static Image LoadImageFromMemory(byte* data, size_t size);
		static Image LoadImage(const std::string& path, int32 numComponents);
		static Image LoadImageFromMemory(byte* data, size_t size, int32 numComponents);
		static void FreeImage(const Image& image);
	};

}