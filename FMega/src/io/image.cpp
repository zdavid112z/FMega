#include "fmega.h"
#include "image.h"

namespace fmega {

	extern Image LoadImageInternal(const std::string& path, int32 numComponents);
	extern Image LoadImageFromMemoryInternal(byte* data, size_t size, int32 numComponents);
	extern void FreeImageInternal(const Image& image);

	Image ImageIO::LoadImage(const std::string& path)
	{
		return LoadImage(path, 0);
	}

	Image ImageIO::LoadImageFromMemory(byte* data, size_t size)
	{
		return LoadImageFromMemory(data, size, 0);
	}
	
	Image ImageIO::LoadImage(const std::string& path, int32 numComponents)
	{
		return LoadImageInternal(path, numComponents);
	}

	Image ImageIO::LoadImageFromMemory(byte* data, size_t size, int32 numComponents)
	{
		return LoadImageFromMemoryInternal(data, size, numComponents);
	}

	void ImageIO::FreeImage(const Image& image)
	{
		FreeImageInternal(image);
	}

}