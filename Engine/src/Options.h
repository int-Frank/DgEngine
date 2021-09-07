//@group Core

#ifndef EN_OPTIONS_H
#define EN_OPTIONS_H

#ifndef DG_ASSERTS_ENABLED
// Comment out for no Asserts
#define DG_ASSERTS_ENABLED
#endif
#include "DgAssert.h"
#include "LogLevel.h"

//----------------------------------------------------------------------------
// Switches
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------
// TODO Load all these from a settings file.
//      Also all GUI defaults.

#define LOG_OUTPUT_FILE "log-output.txt"
#define CRASH_REPORT_FILE "crash-report.txt"

// Renderer...
#define TEMP_BUFFER_SIZE (16 * 1024 * 1024)
#define RENDER_COMMAND_BUFFER_SIZE (1 * 1024 * 1024)
#define RENDER_COMMAND_BUFFER_MEM_POOL (64 * 1024 * 1024)

// Fonts and text...
#define FONTATLAS_DEFAULT_TEXTURE_DIMENSION 1024
#define MAX_TEXT_CHARACTERS 65536

//----------------------------------------------------------------------------
// Logging
//----------------------------------------------------------------------------
#if defined(DGE_DEBUG)
#define LOGLEVEL LL_TRACE
#elif defined(DGE_RELEASE)
#define LOGLEVEL LL_WARN
#else
#define LOGLEVEL LL_OFF
#endif

#endif