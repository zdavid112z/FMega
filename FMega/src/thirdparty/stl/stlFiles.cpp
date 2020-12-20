#include "fmega.h"
#include "stlFiles.h"
#include <cstdio>

namespace fmega {

	std::string STLFiles::ReadFile(const std::string& path)
	{
		FILE* file = fopen(path.c_str(), "r");
		if (file == NULL)
		{
			LOG_ERROR("Could not open file '", path, "'!");
			return "";
		}
		std::string ss;
		char buffer[1024];
		while (fgets(buffer, 1024, file) != NULL)
			ss.append(buffer);
		fclose(file);
		return ss;
	}

	void STLFiles::WriteFile(const std::string& path, const std::string& contents)
	{
		FILE* file = fopen(path.c_str(), "w");
		if (file == NULL)
		{
			LOG_ERROR("Could not create or open file '", path, "'!");
			return;
		}
		fprintf(file, "%s", contents.c_str());
		fclose(file);
	}

	byte* STLFiles::ReadBinFile(const std::string& path, size_t& size)
	{
		FILE* file = fopen(path.c_str(), "rb");
		fseek(file, 0, SEEK_END);
		size = (size_t)_ftelli64(file);
		fseek(file, 0, SEEK_SET);
		byte* data = new byte[size];
		fread(data, 1, size, file);
		fclose(file);
		return data;
	}

	void STLFiles::WriteBinFile(const std::string& path, byte const* data, size_t size)
	{
		FILE* file = fopen(path.c_str(), "wb");
		fwrite(data, 1, size, file);
		fclose(file);
	}

}