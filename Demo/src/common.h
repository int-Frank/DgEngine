#ifndef COMMON_H
#define COMMON_H

#include "System.h"

#include <stdint.h>

#include "Message.h"
#include "DgWorkerPool.h"

uint32_t NextID();

enum SystemID : Engine::SystemID
{
  SID_RenderDemo = Engine::RSID_BEGIN,
  SID_GUIDemo,
};
enum MyMessageClass
{
  MMC_Input = Engine::MC_CLIENT_BEGIN
};

struct State
{
  Dg::WorkerPool *pWorkerPool;
};


void InitWorkerPool(uint32_t threadCount);
void DestroyWorkerPool();
Dg::WorkerPool * GetWorkerPool();#endif