#ifndef GROUP_H
#define GROUP_H

#include <stdint.h>
#include "PODArray.h"

namespace Core
{
  class Group
  {
  public:

    typedef uint16_t IDType;
    IDType const NONE = 0;

    Group();

    IDType BeginNewGroup();
    IDType GetCurrentID() const;
    void EndCurrentGroup();
    void Reset();
  
  private:
  
    IDType            m_currentID;
    PODArray<IDType>  m_IDStack;
  };
}

#endif