//@group Systems

/*
  Original Copyright Yan Chernikov <github.com/TheCherno/Hazel-dev> and contributors.

  The following code is a derivative work of the code from the GitHub project 'Hazel-dev',
  which is licensed under:

                                  Apache License
                             Version 2.0, January 2004
                          http://www.apache.org/licenses

  This code therefore is also licensed under the terms
  of the Apache License Version 2.0

  Copyright 2017-2019 Frank Hart <frankhart010@gmail.com>
*/

#include "SystemStack.h"
#include "System.h"

namespace Engine
{
  SystemStack::SystemStack()
  {

  }

  SystemStack::~SystemStack()
  {
    Clear();
  }

  bool SystemStack::PushSystem(System * a_pLayer, SystemID a_ID)
  {
    BSR_ASSERT(a_pLayer != nullptr);

    if (Find(a_ID) != m_systemStack.end())
      return false;

    m_systemStack.push_back({a_ID, a_pLayer});
    a_pLayer->OnAttach();
    return true;
  }

  void SystemStack::PopSystem(SystemID a_ID)
  {
    auto it = Find(a_ID);
    if (it != m_systemStack.end())
    {
      it->second->OnDetach();
      delete it->second;
      m_systemStack.erase(it);
    }
  }

  void SystemStack::Clear()
  {
    for (auto kv : m_systemStack)
      delete kv.second;
    m_systemStack.clear();
  }

  System * SystemStack::GetSystem(SystemID a_ID)
  {
    System * result(nullptr);
    auto it = Find(a_ID);
    if (it != m_systemStack.end())
      result = it->second;
    return result;
  }

  SystemStack::List::iterator SystemStack::Find(SystemID a_id)
  {
    List::iterator it = m_systemStack.begin();
    for (; it != m_systemStack.end(); it++)
    {
      if (it->first == a_id)
        break;
    }
    return it;
  }

  SystemStack::List::iterator SystemStack::begin()
  {
    return m_systemStack.begin();
  }

  SystemStack::List::iterator SystemStack::end()
  {
    return m_systemStack.end();
  }
}