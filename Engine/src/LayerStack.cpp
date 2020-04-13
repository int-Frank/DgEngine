//@group Layers

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

#include "LayerStack.h"
#include "Layer.h"

namespace Engine
{
  LayerStack::LayerStack()
  {

  }

  LayerStack::~LayerStack()
  {
    Clear();
  }

  bool LayerStack::PushLayer(Layer * a_layer, Layer::ID a_ID)
  {
    if (m_layerStack.find(a_ID) != m_layerStack.end())
      return false;

    m_layerStack.insert(a_ID, a_layer);
    a_layer->OnAttach();
    return true;
  }

  void LayerStack::PopLayer(Layer::ID a_ID)
  {
    auto it = m_layerStack.find(a_ID);
    if (it != m_layerStack.end())
    {
      it->second->OnDetach();
      delete it->second;
      m_layerStack.erase(it);
    }
  }

  void LayerStack::Clear()
  {
    for (auto kv : m_layerStack)
      delete kv.second;
    m_layerStack.clear();
  }

  Layer * LayerStack::GetLayer(Layer::ID a_ID)
  {
    Layer * result(nullptr);
    auto it = m_layerStack.find(a_ID);
    if (it != m_layerStack.end())
      result = it->second;
    return result;
  }

  Dg::Map_AVL<Layer::ID, Layer *>::iterator LayerStack::begin()
  {
    return m_layerStack.begin();
  }

  Dg::Map_AVL<Layer::ID, Layer *>::iterator LayerStack::end()
  {
    return m_layerStack.end();
  }
}