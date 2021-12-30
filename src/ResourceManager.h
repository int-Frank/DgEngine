//@group Memory

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <typeinfo>
#include <stdint.h>
#include "DgOpenHashMap.h"
#include "Log.h"

namespace DgE
{
  // IDs above 0x80000000 are reserved
  typedef uint32_t ResourceID;
#define INVALID_RESOURCE_ID 0xFFFFFFFF

  // User Defioned IDs:   0 <= id < 0x80000000
  // Sequential IDs:      0x80000000 <= id < 0xF0000000
  // Internal static IDs: 0xF0000000 <= id < 0xFFFFFFFF
  enum InternalResourceID : ResourceID
  {
    ir_InternalIDStart = 0x80000000,
    ir_GUIBoxShader = 0xF0000000,
    ir_GUITextShader,
    ir_GUIBoxBorderShader
  };

  ResourceID GetNextResourceID();

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

  template<typename T>
  class ResourceWrapper : public ResourceWrapperBase
  {
  public:

    ResourceWrapper();
    ResourceWrapper(T *);
    ~ResourceWrapper();
  };

  class ResourceManager
  {
    ResourceManager();
    ~ResourceManager();

  public:

    static void Init();
    static void ShutDown();
    static ResourceManager* Instance();

    void Clear();
    void Erase(ResourceID);

    template<typename T>
    void RegisterResource(ResourceID a_id, T * a_pObj)
    {
      m_resourceMap.insert(a_id, new ResourceWrapper<T>(a_pObj));
    }

    template<typename T>
    T* GetResource(ResourceID a_id)
    {
      ResourceWrapperBase ** ppData = m_resourceMap.at(a_id);
      if (ppData == nullptr)
        return nullptr;

      if (typeid(T) != (*ppData)->GetType())
        LOG_WARN("Attempt to retrieve resource of different type! Casting anyway...");
      return static_cast<T *>((*ppData)->GetPointer());
    }

  private:

    static ResourceManager* s_instance;
    Dg::OpenHashMap<ResourceID, ResourceWrapperBase *> m_resourceMap;
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

#endif