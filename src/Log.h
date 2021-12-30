//@group Core

#ifndef LOG_H
#define LOG_H

#include <memory>

#include "spdlog/spdlog.h"
#include "LogLevel.h"

namespace DgE
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
#define LOG_CRITICAL(...) ::DgE::impl::Logger::GetLogger()->critical(__VA_ARGS__)
#else
#define LOG_CRITICAL(...) do {} while (0)
#endif

#if LL_ERROR >= LOGLEVEL
#define LOG_ERROR(...) ::DgE::impl::Logger::GetLogger()->error(__VA_ARGS__)
#else
#define LOG_ERROR(...) do {} while (0)
#endif

#if LL_WARN >= LOGLEVEL
#define LOG_WARN(...) ::DgE::impl::Logger::GetLogger()->warn(__VA_ARGS__)
#else
#define LOG_WARN(...) do {} while (0)
#endif

#if LL_INFO >= LOGLEVEL
#define LOG_INFO(...) ::DgE::impl::Logger::GetLogger()->info(__VA_ARGS__)
#else
#define LOG_INFO(...) do {} while (0)
#endif

#if LL_DEBUG >= LOGLEVEL
#define LOG_DEBUG(...) ::DgE::impl::Logger::GetLogger()->debug(__VA_ARGS__)
#else
#define LOG_DEBUG(...) do {} while (0)
#endif

#if LL_TRACE >= LOGLEVEL
#define LOG_TRACE(...) ::DgE::impl::Logger::GetLogger()->trace(__VA_ARGS__)
#else
#define LOG_TRACE(...) do {} while (0)
#endif

}

#endif