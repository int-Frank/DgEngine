//@group Core

#ifndef LOG_H
#define LOG_H

#include <memory>

#include "spdlog/spdlog.h"
#include "LogLevel.h"

// TODO do not use spdlog. Write your own
namespace Engine
{
  namespace impl
  {
    class Logger
    {
    public:
      static void Init_stdout(char const * name);
      static void Init_file(char const * name, char const * fileName);

      inline static std::shared_ptr<spdlog::logger> & GetLogger() { return s_logger; }
    private:
      static std::shared_ptr<spdlog::logger> s_logger;
    };
  }

#if LL_CRITICAL >= LOGLEVEL
#define LOG_CRITICAL(...) ::Engine::impl::Logger::GetLogger()->critical(__VA_ARGS__)
#else
#define LOG_CRITICAL(...)
#endif

#if LL_ERROR >= LOGLEVEL
#define LOG_ERROR(...) ::Engine::impl::Logger::GetLogger()->error(__VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if LL_WARN >= LOGLEVEL
#define LOG_WARN(...) ::Engine::impl::Logger::GetLogger()->warn(__VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#if LL_INFO >= LOGLEVEL
#define LOG_INFO(...) ::Engine::impl::Logger::GetLogger()->info(__VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LL_DEBUG >= LOGLEVEL
#define LOG_DEBUG(...) ::Engine::impl::Logger::GetLogger()->debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if LL_TRACE >= LOGLEVEL
#define LOG_TRACE(...) ::Engine::impl::Logger::GetLogger()->trace(__VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

}

#endif