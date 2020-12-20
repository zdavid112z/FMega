#include "fmega.h"

#ifdef FMEGA_STB_IMAGE

#include "io/image.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>


namespace fmega {

	Image LoadImageInternal(const std::string& path, int32 numComponents)
	{
		Image result;
		result.data = nullptr;
		int r;
		if (stbi_is_hdr(path.c_str()))
			r = 32;
		else if (stbi_is_16_bit(path.c_str()))
			r = 16;
		else r = 8;

		if (r == 8)
			result.data = stbi_load(path.c_str(), &result.width, &result.height, &result.numComponents, numComponents);
		else if(r == 16)
			result.data = (byte*)stbi_load_16(path.c_str(), &result.width, &result.height, &result.numComponents, numComponents);
		else if (r == 32)
			result.data = (byte*)stbi_loadf(path.c_str(), &result.width, &result.height, &result.numComponents, numComponents);
		if (numComponents != 0)
			result.numComponents = numComponents;
		result.bitsPerComponent = r;
		result.pitch = result.width * result.bitsPerComponent / 8 * result.numComponents;
		if (result.data == nullptr)
			LOG_ERROR("Could not load image '", path, "'!");
		return result;
	}

	Image LoadImageFromMemoryInternal(byte* data, size_t size, int32 numComponents)
	{
		Image result;
		result.data = nullptr;
		int r;
		if (stbi_is_hdr_from_memory(data, size))
			r = 32;
		else if (stbi_is_16_bit_from_memory(data, size))
			r = 16;
		else r = 8;

		if (r == 8)
			result.data = stbi_load_from_memory(data, size, &result.width, &result.height, &result.numComponents, numComponents);
		else if (r == 16)
			result.data = (byte*)stbi_load_16_from_memory(data, size, &result.width, &result.height, &result.numComponents, numComponents);
		else if (r == 32)
			result.data = (byte*)stbi_loadf_from_memory(data, size, &result.width, &result.height, &result.numComponents, numComponents);
		if(numComponents != 0)
			result.numComponents = numComponents;
		result.bitsPerComponent = r;
		result.pitch = result.width * result.bitsPerComponent / 8 * result.numComponents;
		if (result.data == nullptr)
			LOG_ERROR("Could not load image from memory!");
		return result;
	}

	void FreeImageInternal(const Image& image)
	{
		stbi_image_free(image.data);
	}

}

#endif