#pragma once

#include <string>
#ifndef NDEBUG
#include <chrono>
#include <cstdint>
#include <cstdio>
namespace Core{
   namespace Macros{
      enum class WarningLevel{
         LOW,
         MEDIUM,
         HIGH
      };


      struct Stats{
         bool init { false };
         std::chrono::time_point<std::chrono::system_clock> initTime;

         std::uint32_t totalMessages { 0 };

         std::uint32_t totalWarnings { 0 };
         std::uint32_t lowLevelWarnings { 0 };
         std::uint32_t medLevelWarnings { 0 };
         std::uint32_t highLevelWarnings { 0 };

         std::string logFilePath {};
         FILE* logFile { nullptr };
      };

// arguments for file, line and function
#define FIG_FLF_ARGS const char* file, int line, const char* function

   void initLogging(const char* pathToLogFile);
   void logMessage(const char* msg, FIG_FLF_ARGS);
   void logWarning(const char* msg, FIG_FLF_ARGS, WarningLevel lvl);
   void figAssert(const char* msg, FIG_FLF_ARGS);
   void outputStats();

   // returns a formatted string of the time since the program began
   std::string timeSinceprogramStart();

   } // namespace Macros
} // namespace Core

#define FIG_FLF __FILE__, __LINE__, __func__
#define FIG_INITIALIZE_LOGGING_WITH_PATH(path) Core::Macros::initLogging(path);
#define FIG_INITIALIZE_LOGGING FIG_INITIALIZE_LOGGING_WITH_PATH("FigLog.txt");
#define FIG_LOG_MESSAGE(msg) Core::Macros::logMessage(msg, FIG_FLF);
#define FIG_LOG_WARNING(msg, level) Core::Macros::logWarning(msg, FIG_FLF, Core::Macros::WarningLevel::level);
#define FIG_LOG_LOW_WARNING(msg) FIG_LOG_WARNING(msg, LOW)
#define FIG_LOG_MEDIUM_WARNING(msg) FIG_LOG_WARNING(msg, MEDIUM)
#define FIG_LOG_HIGH_WARNING(msg) FIG_LOG_WARNING(msg, HIGH)
#define FIG_ASSERT(msg) Core::Macros::figAssert(msg, FIG_FLF);
#define FIG_LOG_OUTPUT_LOG_INFO Core::Macros::outputStats();

#else
#define FIG_FLF
#define FIG_INITIALIZE_LOGGING_WITH_PATH(path)
#define FIG_INITIALIZE_LOGGING
#define FIG_LOG_MESSAGE(msg)
#define FIG_LOG_WARNING(msg, level)
#define FIG_LOG_LOW_WARNING(msg)
#define FIG_LOG_MEDIUM_WARNING(msg)
#define FIG_LOG_HIGH_WARNING(msg)
#define FIG_ASSERT(msg)
#define FIG_OUTPUT_STATS
#endif
