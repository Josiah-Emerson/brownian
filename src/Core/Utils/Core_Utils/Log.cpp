#include "Log.h"
#include <cassert>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>

// TODO: Add a header to log which updates to reflect the stats at the top of the log
namespace Core{
   namespace Macros{
#define FIG_LOG_INIT_CHECK assert(g_stats.init && \
      "Fig Logging has not been initialized. Please use the macro "\
      "FIG_INITIALIZE_LOGGING at program startup");


      Stats g_stats { };

      void initLogging(const char* pathToLogFile){
         g_stats.initTime = std::chrono::system_clock::now();
         g_stats.logFilePath = pathToLogFile;
         g_stats.logFile = fopen(pathToLogFile, "w");
         if(!g_stats.logFile){
            perror("Core::Macros::initLogging() failed to open a logging file");
            assert(false && "Error in Core::Macros::initLogging() in opening logging file. Check stderr");
         }
         fprintf(g_stats.logFile, 
               "Welcome to Fig Log. The Format is:\nTIME "
               "(from start of program) | Type of Log | "
               "Message | source file information\nExample:\n");

         fprintf(g_stats.logFile, "HH:MM:SS:MS | LOG/WARNING | Message | File:Line File::function()\n\n");
         fflush(g_stats.logFile);

         g_stats.init = true;
      }

      void logMessage(const char* msg, FIG_FLF_ARGS){
         FIG_LOG_INIT_CHECK;
         std::string _file { shortenFilePath(file) };
         fprintf(g_stats.logFile, "%s LOG: %s | %s:%d, %s::%s\n",
               timeSinceprogramStart().c_str(), msg, _file.c_str(), line, _file.c_str(), function);
         fflush(g_stats.logFile);

         ++g_stats.totalMessages;
      }

      void logWarning(const char* msg, FIG_FLF_ARGS, WarningLevel lvl){
         FIG_LOG_INIT_CHECK;
         std::string wlabel;
         switch(lvl){
            case(WarningLevel::LOW):
               wlabel = "WARNING(LOW)";
               ++g_stats.lowLevelWarnings;
               break;
            case(WarningLevel::MEDIUM):
               wlabel = "WARNING(MEDIUM)";
               ++g_stats.medLevelWarnings;
               break;
            case(WarningLevel::HIGH):
               wlabel = "WARNING(HIGH)";
               ++g_stats.highLevelWarnings;
               break;
         }

         ++g_stats.totalMessages;
         ++g_stats.totalWarnings;

         std::string _file { shortenFilePath(file) };
         fprintf(g_stats.logFile, "%s %s: %s | %s:%d, %s::%s\n",
               timeSinceprogramStart().c_str(), wlabel.c_str(), msg, _file.c_str(), line, _file.c_str(), function);
         fflush(g_stats.logFile);
      }

      void figAssert(const char* msg, FIG_FLF_ARGS){
         FIG_LOG_INIT_CHECK;
         std::string _file { shortenFilePath(file) };
         std::size_t sz = snprintf(NULL, 0, "%s FATAL ERROR: %s | %s:%d, %s::%s\n",
               timeSinceprogramStart().c_str(), msg, _file.c_str(), line, _file.c_str(), function);
         char* str = new char[sz + 1];
         sprintf(str, "%s FATAL ERROR: %s | %s:%d, %s::%s\n",
               timeSinceprogramStart().c_str(), msg, _file.c_str(), line, _file.c_str(), function);
         fprintf(g_stats.logFile, "%s", str);
         fprintf(stderr, "%s", str);
         fflush(g_stats.logFile);

         assert(false && "FIG_ASSERT encounterd. See stderr or log for more information");
      }

      void outputStats(){
         FIG_LOG_INIT_CHECK;
         printf("Total Log Messages: %d\n"
                "Total Warnings: %d\n"
                "High Level Warnings: %d\n"
                "Medium Level Warnings: %d\n"
                "Low Level Warnings: %d\n"
                "\nThe log can be found at %s\n",
                g_stats.totalMessages, g_stats.totalWarnings,
                g_stats.highLevelWarnings, g_stats.medLevelWarnings, 
                g_stats.lowLevelWarnings, g_stats.logFilePath.c_str());
      }

      std::string timeSinceprogramStart(){
         FIG_LOG_INIT_CHECK;
         auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(
               std::chrono::system_clock::now() - g_stats.initTime);
         unsigned long cnt = diff.count();
         const int HRS { static_cast<int>(cnt / (1000 * 60 * 60)) };
         cnt -= HRS * 1000 * 60 * 60;

         const int MINS { static_cast<int>(cnt / (1000 * 60)) };
         cnt -= MINS * 1000 * 60;

         const int SECS { static_cast<int>(cnt / (1000)) };
         cnt -= SECS * 1000;

         return std::to_string(HRS) + ':' + std::to_string(MINS) + ':' + std::to_string(SECS) + ':' + std::to_string(cnt);
      }

      std::string shortenFilePath(const char* path){
         std::string str { path };
         std::size_t idx = str.rfind("src");
         return idx == std::string::npos ? str : str.substr(idx, str.size() - idx);
      }

   } // namespace Macros
} // namespace Core
/*
HH:MM:SS:MS LOG: This is the message I want to send | Log.cpp:124, Log.cpp::function()
HH:MM:SS:MS WARNING(high|medium|low): This is the message I want to send | From file Log.cpp:124, in function logMessage()
 */
