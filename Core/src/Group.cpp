#include "Group.h"
#include "core_Assert.h"

namespace Core
{
  Group::Group()
    : m_currentID(NONE)
  {
    m_IDStack.push_back(NONE);
  }

  Group::IDType Group::BeginNewGroup()
  {
    BSR_ASSERT(m_currentID < 0xFFFF, "To many groups!");
    m_currentID++;
    m_IDStack.push_back(m_currentID);
    return m_currentID;
  }

  void Group::EndCurrentGroup()
  {
    if (m_IDStack.size() > 1)
      m_IDStack.pop_back();
  }

  Group::IDType Group::GetCurrentID() const
  {
    return m_IDStack.cback();
  }

  void Group::Reset()
  {
    m_currentID = NONE;
    m_IDStack.clear();
    m_IDStack.push_back(NONE);
  }
}