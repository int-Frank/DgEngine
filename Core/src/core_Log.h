#ifndef LOG_H
#define LOG_H

#include <memory>

#include "spdlog/spdlog.h"
#include "core_LogLevel.h"

namespace Core
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
#define LOG_CRITICAL(...) ::Core::impl::Logger::GetLogger()->critical(__VA_ARGS__)
#else
#define LOG_CRITICAL(...)
#endif

#if LL_ERROR >= LOGLEVEL
#define LOG_ERROR(...) ::Core::impl::Logger::GetLogger()->error(__VA_ARGS__)
#else
#define LOG_ERROR(...)
#endif

#if LL_WARN >= LOGLEVEL
#define LOG_WARN(...) ::Core::impl::Logger::GetLogger()->warn(__VA_ARGS__)
#else
#define LOG_WARN(...)
#endif

#if LL_INFO >= LOGLEVEL
#define LOG_INFO(...) ::Core::impl::Logger::GetLogger()->info(__VA_ARGS__)
#else
#define LOG_INFO(...)
#endif

#if LL_DEBUG >= LOGLEVEL
#define LOG_DEBUG(...) ::Core::impl::Logger::GetLogger()->debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#if LL_TRACE >= LOGLEVEL
#define LOG_TRACE(...) ::Core::impl::Logger::GetLogger()->trace(__VA_ARGS__)
#else
#define LOG_TRACE(...)
#endif

}

#endif