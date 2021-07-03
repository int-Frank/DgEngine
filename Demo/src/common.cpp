#include "common.h"

static Dg::WorkerPool *gpWorkerPool = nullptr;

void InitWorkerPool(uint32_t threadCount)
{
  gpWorkerPool = new Dg::WorkerPool(threadCount);
}

void DestroyWorkerPool()
{
  delete gpWorkerPool;
  gpWorkerPool = nullptr;
}

Dg::WorkerPool * GetWorkerPool()
{
  return gpWorkerPool;
}

uint32_t NextID()
{
  static uint32_t s_ID = 0;
  return ++s_ID;
}