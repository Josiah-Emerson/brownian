#pragma once
#include <string>
#ifndef NDEBUG
#include <chrono>
#include <cstdint>
#include <cstdio>

// TODO: If you put a message in a destructor for something, and the OUTPUT_STATS 
// is called before the end of the scope, then the count of messages will likely be off,
// although the messages are still recorded. Example: consider the class Foo, where a message 
// is logged in both constructor in destructor. The following pseudo code would result in an output 
// stating there is only one message, but checking the log file will result in the expected 2 logs 
// because the stat output was called prior than the destructor for the class Foo:
/*
   int main(){
      FIG_INITIALIZE_LOGGING
      class Foo { }; // constructor is called and the message is logged
      // ... do whatever or nothing
      FIG_LOG_OUTPUT_LOG_INFO // log info is ouptut (i.e. message count is 1)
   } // Foo destructor is called, and another message is logged
 */

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

   // shortens file path to relative to src/
   // if not found just returns std::string { path }
   std::string shortenFilePath(const char* path);

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
#define FIG_ASSERT(condition, msg) static_cast<bool>(condition) ? void(0) : Core::Macros::figAssert(msg, FIG_FLF);
#define FIG_UNCREACHABLE(msg)  FIG_ASSERT(false, msg)
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
#define FIG_UNCREACHABLE(msg)
#define FIG_LOG_OUTPUT_LOG_INFO 
#endif
