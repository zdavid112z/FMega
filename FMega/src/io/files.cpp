#include "fmega.h"
#include "files.h"
#include <cstdio>

namespace fmega {

	std::string Files::ReadFile(const std::string& path)
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

	void Files::WriteFile(const std::string& path, const std::string& contents)
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

	byte* Files::ReadBinFile(const std::string& path, size_t& size)
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

	void Files::WriteBinFile(const std::string& path, byte const* data, size_t size)
	{
		FILE* file = fopen(path.c_str(), "wb");
		fwrite(data, 1, size, file);
		fclose(file);
	}

	std::string Files::ReadString(FILE* file)
	{
		uint32 size;
		fread(&size, 4, 1, file);
		char* buffer = new char[size + 1];
		fread(buffer, 1, size, file);
		buffer[size] = 0;
		return std::string(buffer);
	}

	void Files::WriteString(FILE* file, const std::string& str)
	{
		uint32 count = str.size();
		fwrite(&count, 4, 1, file);
		if (count != 0)
			fwrite(str.data(), 1, count, file);
	}

}