#ifndef COMMON_H
#define COMMON_H

#include "System.h"

#include <stdint.h>

#include "Message.h"
#include "DgWorkerPool.h"

uint32_t NextID();

enum MyMessageClass
{
  MMC_Input = DgE::MC_CLIENT_BEGIN
};

struct State
{
  Dg::WorkerPool *pWorkerPool;
};

void InitWorkerPool(uint32_t threadCount);
void DestroyWorkerPool();
Dg::WorkerPool * GetWorkerPool();

#endif