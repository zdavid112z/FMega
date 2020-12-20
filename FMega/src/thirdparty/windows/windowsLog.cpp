#include "fmega.h"
#if (defined(FMEGA_WIN32) || defined(FMEGA_WIN64)) && (defined(FMEGA_DEBUG) || !defined(FMEGA_REMOVE_FORCE_LOG))

#include "utils/log.h"
#include <iostream>
#include <Windows.h>
#undef ERROR

namespace fmega {

	HANDLE g_ConsoleHandle = nullptr;

	void PrintToConsoleInternal(const std::string& str)
	{
		std::cout << str.c_str();
	}

	void SetLogWarningLevelInternal(LogWarningLevel level)
	{
		if (g_ConsoleHandle == nullptr)
			g_ConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		switch (level)
		{
		case LogWarningLevel::VERB:
			SetConsoleTextAttribute(g_ConsoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
			break;
		case LogWarningLevel::INFO:
			SetConsoleTextAttribute(g_ConsoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogWarningLevel::WARN:
			SetConsoleTextAttribute(g_ConsoleHandle, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogWarningLevel::ERROR:
			SetConsoleTextAttribute(g_ConsoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogWarningLevel::FATAL:
			SetConsoleTextAttribute(g_ConsoleHandle, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY | BACKGROUND_RED);
			break;
		}
	}

}

#endif