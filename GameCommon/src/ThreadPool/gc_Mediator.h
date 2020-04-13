#ifndef GC_MEDIATOR_H
#define GC_MEDIATOR_H

#include <string>
#include <atomic>

namespace GC
{
  class Mediator
  {
  public:

    Mediator()
      : m_progress(0.0f)
      , m_shouldStop(false)
      , m_done(false)
    {

    }

    virtual ~Mediator() {}

    void Report(float a_progress)
    {
      if (a_progress < 0.0f)
        a_progress = 0.0f;
      else if (a_progress > 100.0f)
        a_progress = 100.0f;

      m_progress = floorf(a_progress * 4.0f) * 0.25f;
    }

    float GetProgress() const {return m_progress;}
    void Done() {m_done = true;}
    bool IsDone() const {return m_done;}
    bool ShouldStop() const {return m_shouldStop;}
    void FlagStop() {m_shouldStop = true;}

  private:

    std::atomic<float> m_progress;
    std::atomic<bool> m_shouldStop;
    std::atomic<bool> m_done;
  };
}

#endif