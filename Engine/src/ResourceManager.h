//@group Memory

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <typeinfo>
#include <mutex>
#include <atomic>
#include <stdint.h>
#include "DgOpenHashMap.h"
#include "ResourceID.h"

namespace Engine
{
  namespace impl
  {
    enum IDType : uint16_t
    {
      T_UserID    = 1,
      T_Generated = 2
    };

    class ResourceWrapperBase
    {
    public:

      ResourceWrapperBase(void*, std::type_info const &);
      virtual ~ResourceWrapperBase();
      void * GetPointer();
      std::type_info const& GetType();
    protected:
      std::type_info const & m_info;
      void* m_pObj;
    };

    class IDEqualTo
    {
    public:
      bool operator()(impl::ResourceID64 const& a_id0,
                      impl::ResourceID64 const& a_id1) const
      {
        return a_id0.GetID() == a_id1.GetID();
      }
    };

    class IDHasher
    {
    public:
      size_t operator()(impl::ResourceID64 const& a_id) const
      {
        return static_cast<size_t>(a_id.GetID());
      }
    };

    class ResourceManager
    {
    public:

      struct Data
      {
        ResourceWrapperBase *  pResource;
        uint32_t        counter;
      };

      //static ResourceID const NULL_ResourceID = 0;

      ResourceManager();

      static ResourceManager* Instance();

      ResourceWrapperBase* GetResource(ResourceID64, bool registerUser);
      uint32_t GenerateUnique32();

      void RegisterUser(ResourceID64);
      void DeregisterUser(ResourceID64);

      void RegisterResource(ResourceID64, ResourceWrapperBase*);
      void DestroyResource(ResourceID64);

    private:

      static ResourceManager* s_instance;

      std::atomic<uint32_t>               m_currentID;
      std::mutex                          m_mutex;
      Dg::OpenHashMap<ResourceID64, Data, IDHasher, IDEqualTo> m_resourceMap;
    };

    template<typename T>
    class ResourceWrapper : public ResourceWrapperBase
    {
    public:

      ResourceWrapper();
      ResourceWrapper(T*);
      ~ResourceWrapper();
    };

    template<typename T>
    ResourceWrapper<T>::ResourceWrapper()
      : ResourceWrapperBase(nullptr)
    {

    }

    template<typename T>
    ResourceWrapper<T>::ResourceWrapper(T* a_pObj)
      : ResourceWrapperBase(a_pObj, typeid(T))
    {

    }

    template<typename T>
    ResourceWrapper<T>::~ResourceWrapper()
    {
      delete static_cast<T*>(m_pObj);
      m_pObj = nullptr;
    }
  }
}

#endif