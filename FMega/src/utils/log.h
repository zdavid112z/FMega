#pragma once

#include "stringUtils.h"
#include "macros.h"

namespace fmega {

	enum class LogWarningLevel
	{
		VERB,
		INFO,
		WARN,
		ERROR,
		FATAL
	};

	extern void PrintToConsoleInternal(const std::string& str);
	extern void SetLogWarningLevelInternal(LogWarningLevel level);

	class Log
	{
	public:
		static void Print() {}
		template <typename T>
		static void Print(const T& val);
		template <typename T, typename... Args>
		static void Print(const T& val, const Args&... args);
		static void SetWarningLevel(LogWarningLevel level)
		{
			SetLogWarningLevelInternal(level);
		}
	};

	template <typename T>
	void Log::Print(const T& val)
	{
		PrintToConsoleInternal(StringUtils::ToString(val));
	}

	template <typename T, typename... Args>
	void Log::Print(const T& val, const Args&... args)
	{
		Print(val);
		Print(args...);
	}

}

#ifdef FMEGA_SHOW_LOGS
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define LOG_VERB(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::VERB); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define LOG_INFO(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::INFO); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define LOG_WARN(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::WARN); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define LOG_ERROR(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::ERROR); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define LOG_FATAL(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::FATAL); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
#else
#define LOG_VERB(...) {}
#define LOG_INFO(...) {}
#define LOG_WARN(...) {}
#define LOG_ERROR(...) {}
#define LOG_FATAL(...) {}
#endif

#ifdef FMEGA_EVAL_ASSERT
#define ASSERT(condition, ...)\
if(condition){}\
else\
{LOG_FATAL("----------------------------------------");\
LOG_FATAL("Assertion failed on: '", #condition, "'!");\
LOG_FATAL(__VA_ARGS__);\
LOG_FATAL("Line: ", __LINE__, "; File: ", __FILE__);\
LOG_FATAL("----------------------------------------");\
__debugbreak();\
}
#else
#define ASSERT(condition, ...) condition
#endif

#ifdef FMEGA_DEBUG
#define DEBUG_ONLY(x) x
#else
#define DEBUG_ONLY(x)
#endif

#ifndef FMEGA_REMOVE_FORCE_LOG
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define FORCE_LOG_VERB(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::VERB); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define FORCE_LOG_INFO(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::INFO); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define FORCE_LOG_WARN(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::WARN); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define FORCE_LOG_ERROR(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::ERROR); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }
// Do not call important functions from the log parameters, they will be omitted in the Release configuration
#define FORCE_LOG_FATAL(...) {fmega::Log::SetWarningLevel(fmega::LogWarningLevel::FATAL); fmega::Log::Print(__VA_ARGS__); fmega::Log::Print('\n'); }

#define FORCE_ASSERT(condition, ...)\
if(condition){}\
else\
{FORCE_LOG_FATAL("----------------------------------------");\
FORCE_LOG_FATAL("Assertion failed on: '", #condition, "'!");\
FORCE_LOG_FATAL(__VA_ARGS__);\
FORCE_LOG_FATAL("Line: ", __LINE__, "; File: ", __FILE__);\
FORCE_LOG_FATAL("----------------------------------------");\
__debugbreak();\
}
#else
#define FORCE_LOG_VERB(...) {}
#define FORCE_LOG_INFO(...) {}
#define FORCE_LOG_WARN(...) {}
#define FORCE_LOG_ERROR(...) {}
#define FORCE_LOG_FATAL(...) {}
#define FORCE_ASSERT(condition, ...) condition
#endif