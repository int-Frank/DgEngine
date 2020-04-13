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

#ifndef EN_LAYERSTACK_H
#define EN_LAYERSTACK_H

#include <stdint.h>
#include "DgMap_AVL.h"
#include "Layer.h"

namespace Engine
{
  class Layer;

  class LayerStack
  {
  public:

    LayerStack();
    ~LayerStack();

    bool PushLayer(Layer *, Layer::ID);
    void PopLayer(Layer::ID);
    Layer * GetLayer(Layer::ID);
    void Clear();

    Dg::Map_AVL<Layer::ID, Layer *>::iterator begin();
    Dg::Map_AVL<Layer::ID, Layer *>::iterator end();

  private:

    Dg::Map_AVL<Layer::ID, Layer *>  m_layerStack;
  };
}

#endif
