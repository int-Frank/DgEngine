#ifndef BSR_ASSERT_H
#define BSR_ASSERT_H

#define DG_ASSERTS_ENABLED
#include "DgAssert.h"

#ifdef BSR_DEBUG
#define BSR_ASSERT(...) DG_ASSERT(__VA_ARGS__)
#else
#define BSR_ASSERT(...)
#endif

#endif