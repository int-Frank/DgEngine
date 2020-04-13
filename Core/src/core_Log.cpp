
#include "core_Log.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/fmt/ostr.h"

namespace Core
{
  std::shared_ptr<spdlog::logger> impl::Logger::s_logger;

  void impl::Logger::Init_stdout(char const * a_name)
  {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_logger = spdlog::stdout_color_mt(a_name);
    s_logger->set_level(spdlog::level::trace);
  }

  void impl::Logger::Init_file(char const * a_name, char const * a_fileName)
  {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    s_logger = spdlog::basic_logger_mt(a_name, a_fileName);
    s_logger->set_level(spdlog::level::trace);
  }
}