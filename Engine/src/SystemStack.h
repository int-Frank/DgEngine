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

#ifndef EN_SYSTEMSTACK_H
#define EN_SYSTEMSTACK_H

#include <stdint.h>
#include "DgMap_AVL.h"
#include "System.h"

namespace Engine
{
  class System;

  class SystemStack
  {
  public:

    SystemStack();
    ~SystemStack();

    bool PushLayer(System *, System::ID);
    void PopLayer(System::ID);
    System * GetLayer(System::ID);
    void Clear();

    Dg::Map_AVL<System::ID, System *>::iterator begin();
    Dg::Map_AVL<System::ID, System *>::iterator end();

  private:

    Dg::Map_AVL<System::ID, System *>  m_layerStack;
  };
}

#endif
