#pragma once

#include "fmega.h"
#include <cstdio>

namespace fmega {

	class Files
	{
	public:
		static std::string ReadFile(const std::string& path);
		static void WriteFile(const std::string& path, const std::string& contents);
		static byte* ReadBinFile(const std::string& path, size_t& size);
		static void WriteBinFile(const std::string& path, byte const* data, size_t size);
		static std::string ReadString(FILE* file);
		template <typename T>
		static std::vector<T> ReadVector(FILE* file)
		{
			uint32 count;
			fread(&count, 4, 1, file);
			std::vector<T> vec(count);
			fread(vec.data(), sizeof(T), count, file);
			return vec;
		}
		static void WriteString(FILE* file, const std::string& str);
		template <typename T>
		static void WriteVector(FILE* file, const std::vector<T>& vec)
		{
			uint32 count = vec.size();
			fwrite(&count, 4, 1, file);
			if (count != 0)
				fwrite(vec.data(), sizeof(T), count, file);
		}
		template <typename T>
		static void ReadAllocator(FILE* file, T& alloc)
		{
			uint32 size;
			fread(&size, 4, 1, file);
			alloc.DeleteAndResize(size);
			fread(alloc.Begin(), 1, size, file);
			uint32 lastRemoved, numRemoved;
			fread(&lastRemoved, 4, 1, file);
			fread(&numRemoved, 4, 1, file);
			alloc.SetRemovedInfo(lastRemoved, numRemoved);
		}
		template <typename T>
		static void WriteAllocator(FILE* file, T& alloc)
		{
			uint32 size;
			uint32 lastRemoved, numRemoved;
			size = alloc.GetSize();
			fwrite(&size, 4, 1, file);
			fwrite(alloc.Begin(), 1, size, file);
			alloc.GetRemovedInfo(lastRemoved, numRemoved);
			fwrite(&lastRemoved, 4, 1, file);
			fwrite(&numRemoved, 4, 1, file);
		}
	private:
	};

}