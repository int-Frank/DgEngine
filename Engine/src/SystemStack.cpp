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

  bool SystemStack::PushLayer(System * a_pLayer, System::ID a_ID)
  {
    BSR_ASSERT(a_pLayer != nullptr);

    if (m_layerStack.find(a_ID) != m_layerStack.end())
      return false;

    m_layerStack.insert(a_ID, a_pLayer);
    a_pLayer->OnAttach();
    return true;
  }

  void SystemStack::PopLayer(System::ID a_ID)
  {
    auto it = m_layerStack.find(a_ID);
    if (it != m_layerStack.end())
    {
      it->second->OnDetach();
      delete it->second;
      m_layerStack.erase(it);
    }
  }

  void SystemStack::Clear()
  {
    for (auto kv : m_layerStack)
      delete kv.second;
    m_layerStack.clear();
  }

  System * SystemStack::GetLayer(System::ID a_ID)
  {
    System * result(nullptr);
    auto it = m_layerStack.find(a_ID);
    if (it != m_layerStack.end())
      result = it->second;
    return result;
  }

  Dg::Map_AVL<System::ID, System *>::iterator SystemStack::begin()
  {
    return m_layerStack.begin();
  }

  Dg::Map_AVL<System::ID, System *>::iterator SystemStack::end()
  {
    return m_layerStack.end();
  }
}