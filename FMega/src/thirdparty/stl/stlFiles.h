#pragma once

#include "fmega.h"

namespace fmega {

	class STLFiles
	{
	public:
		static std::string ReadFile(const std::string& path);
		static void WriteFile(const std::string& path, const std::string& contents);
		static byte* ReadBinFile(const std::string& path, size_t& size);
		static void WriteBinFile(const std::string& path, byte const* data, size_t size);
	private:
	};

}