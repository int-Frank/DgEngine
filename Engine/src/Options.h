//@group Core

#ifndef EN_OPTIONS_H
#define EN_OPTIONS_H

#include "LogLevel.h"

//----------------------------------------------------------------------------
// Switches
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
#define LOG_OUTPUT_FILE "log-output.txt"
#define CRASH_REPORT_FILE "crash-report.txt"

#define TEMP_BUFFER_SIZE (16 * 1024 * 1024)
#define RENDER_COMMAND_BUFFER_SIZE (1 * 1024 * 1024)
#define RENDER_COMMAND_BUFFER_MEM_POOL (64 * 1024 * 1024)
#define FONTATLAS_DEFAULT_TEXTURE_DIMENSION 2048

//----------------------------------------------------------------------------
// Logging
//----------------------------------------------------------------------------
#if defined(BSR_DEBUG)
#define LOGLEVEL LL_TRACE
#elif defined(BSR_RELEASE)
#define LOGLEVEL LL_WARN
#else
#define LOGLEVEL LL_OFF
#endif

#endif