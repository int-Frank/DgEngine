//@group Memory

#include "ResourceManager.h"
#include "core_Assert.h"

namespace Engine
{
  //--------------------------------------------------------------------------------------
  // ResourceWrapperBase
  //--------------------------------------------------------------------------------------
  ResourceWrapperBase::ResourceWrapperBase(void * a_pObj, std::type_info const& a_info)
    : m_pObj(a_pObj)
    , m_info(a_info)
  {

  }

  ResourceWrapperBase::~ResourceWrapperBase()
  {

  }

  void* ResourceWrapperBase::GetPointer()
  {
    return m_pObj;
  }

  std::type_info const& ResourceWrapperBase::GetType()
  {
    return m_info;
  }

  //--------------------------------------------------------------------------------------
  // Resource Manager
  //--------------------------------------------------------------------------------------
  ResourceManager * ResourceManager::s_instance = nullptr;

  void ResourceManager::Init()
  {
    BSR_ASSERT(s_instance == nullptr, "Trying to initialise ResourceManager more than once!");
    s_instance = new ResourceManager();
  }

  void ResourceManager::ShutDown()
  {
    delete s_instance;
    s_instance = nullptr;
  }

  ResourceManager* ResourceManager::Instance()
  {
    return s_instance;
  }

  ResourceManager::ResourceManager()
  {

  }

  ResourceManager::~ResourceManager()
  {
    Clear();
  }

  void ResourceManager::Clear()
  {
    for (auto kv : m_resourceMap)
    {
      delete kv.second;
      kv.second = nullptr;
    }
    m_resourceMap.clear();
  }

  void ResourceManager::Erase(ResourceID a_id)
  {
    ResourceWrapperBase ** ppData = m_resourceMap.at(a_id);
    if (ppData == nullptr)
      return;

    delete (*ppData);
    m_resourceMap.erase(a_id);
  }
}