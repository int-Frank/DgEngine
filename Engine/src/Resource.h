//@group Memory

#pragma once

#include <stdint.h>
#include "DgBit.h"
#include "ResourceManager.h"
#include "ResourceID.h"

namespace Engine
{
  //Client-side. This is what the game can use.
  typedef uint32_t ResourceID;

  void DestroyResource(ResourceID a_id);

  //If fail, returns false, resource not touched.
  //If succeed, returns true, ownership of a_obj taken
  template<typename T>
  bool RegisterResource(ResourceID a_id, T* a_obj)
  {
    impl::ResourceID64 id64;
    id64.SetFlag(impl::ResourceID64::Flag::Persistant, true);
    id64.SetType(impl::T_UserID);
    id64.SetID(a_id);
    impl::ResourceWrapperBase * res = impl::ResourceManager::Instance()->GetResource(id64, false);
    if (res == nullptr)
    {
      impl::ResourceManager::Instance()->RegisterResource(id64, new impl::ResourceWrapper<T>(a_obj));
      return true;
    }
    return false;
  }

  //Derive from this class if you want a resource to have access to its resource ID.
  class Resource
  {
  public:

    virtual ~Resource(){}

    impl::ResourceID64 GetRefID() const
    {
      return m_resourceID;
    }

    void SetRefID(impl::ResourceID64 a_id)
    {
      m_resourceID = a_id;
    }

  private:
    impl::ResourceID64 m_resourceID;
  };
}