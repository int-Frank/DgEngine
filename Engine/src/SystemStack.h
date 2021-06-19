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
#include "DgDoublyLinkedList.h"
#include "System.h"

namespace Engine
{
  class System;

  class SystemStack
  {
  public:
    typedef Dg::DoublyLinkedList<std::pair<SystemID, System *>> List;

    SystemStack();
    ~SystemStack();

    bool PushSystem(System *, SystemID);
    void PopSystem(SystemID);
    System * GetSystem(SystemID);
    void Clear();

    List::iterator begin();
    List::iterator end();

  private:

    List::iterator Find(SystemID);

  private:

    List  m_systemStack;
  };
}

#endif
