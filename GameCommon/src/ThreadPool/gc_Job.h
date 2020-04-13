#ifndef GC_JOB_H
#define GC_JOB_H

#include <memory>

#include "gc_Mediator.h"

namespace GC
{
  class Job
  {
  public:
    Job(std::shared_ptr<Mediator> a_pMediator = nullptr)
      : m_pMediator(a_pMediator)
    {}

    virtual ~Job() {}
    virtual void Run() = 0;
    virtual void Done() = 0;

  protected:

    std::shared_ptr<Mediator> m_pMediator;
  };
}

#endif