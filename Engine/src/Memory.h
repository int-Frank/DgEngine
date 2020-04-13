//@group Memory

#ifndef REF_H
#define REF_H

#include <type_traits>
#include <stdint.h>
#include "ResourceManager.h"
#include "ResourceID.h"
#include "Resource.h"
#include "core_Log.h"
#include "core_Assert.h"

namespace Engine
{
  //---------------------------------------------------------------------------------------
  // Declarations
  //---------------------------------------------------------------------------------------

  //Allocate on the tempory buffer. The tempory buffer is a chunk of memory we use and then
  //clear each frame.
  void* TBUFAlloc(size_t);

  //Clear the tempory buffer
  void TBUFClear();

  //Moved to core_utils
  //Advance a void pointer a number of bytes
  //void * AdvancePtr(void *, size_t);

  //A wrapper which constructs and stores objects on the tempory buffer. Objects must be
  //trivially destructable.
  template<typename T>
  class TRef;

  //Similar functionality to std::shared_ptr. Objects are stored in the Resource Manager.
  template<typename T>
  class Ref;

  template<typename A, typename B>
  TRef<A> StaticPointerCast(TRef<B> const&);

  template<typename A, typename B>
  TRef<A> DynamicPointerCast(TRef<B> const&);

  //---------------------------------------------------------------------------------------
  // Definitions
  //---------------------------------------------------------------------------------------

  //TODO add ability to allocate arrays in the per-frame memory
  //See: https://stackoverflow.com/questions/13061979/shared-ptr-to-an-array-should-it-be-used

  //Tempory Reference. A per-frame reference. Memory is cleared at the end of each frame,
  //but objects are not destructed
  template<typename T>
  class TRef
  {
    static_assert(std::is_trivially_destructible<T>::value, "TRef<T>: 'T' cannot have a custom destructor");

    template<typename A, typename B>
    friend TRef<A> StaticPointerCast(TRef<B> const&);

    template<typename A, typename B>
    friend TRef<B> StaticPointerCast(TRef<A> const&);

    template<typename A, typename B>
    friend TRef<A> DynamicPointerCast(TRef<B> const&);

    template<typename A, typename B>
    friend TRef<B> DynamicPointerCast(TRef<A> const&);

    TRef(T * a_ptr)
      : m_pObject(a_ptr)
    {

    }

  public:

    TRef()
      : m_pObject(nullptr)
    {

    }

    template<typename ... Args>
    static TRef New(Args&&... args)
    {
      TRef ref(static_cast<T*>(TBUFAlloc(sizeof(T))));
      new (ref.m_pObject) T(std::forward<Args>(args)...);
      return ref;
    }

    static TRef MakeCopy(T const * a_ptr)
    {
      TRef ref(static_cast<T*>(TBUFAlloc(sizeof(T))));
      new (ref.m_pObject) T(*a_ptr);
      return ref;
    }

    T * operator->() const noexcept
    {
      return m_pObject;
    }

    T & operator*() const noexcept
    {
      return *m_pObject;
    }

    T* Get() const noexcept
    {
      return m_pObject;
    }

  private:

    T * m_pObject;
  };

  template<typename A, typename B>
  TRef<A> StaticPointerCast(TRef<B> const& a_tref)
  {
    return TRef<A>(static_cast<A*>(a_tref.m_pObject));
  }

  template<typename A, typename B>
  TRef<A> DynamicPointerCast(TRef<B> const& a_tref)
  {
    return TRef<A>(dynamic_cast<A*>(a_tref.m_pObject));
  }

  template<typename T>
  class Ref
  {
  public:

    Ref();

    // default constructor when T is not derived from Resource
    template
      <
      typename U = T,
      typename = typename std::enable_if< !std::is_base_of<Resource, U>::value >::type
      >
      constexpr Ref(T* a_pObj)
      : m_pObject(a_pObj)
    {
      if (m_pObject == nullptr)
        return;

      m_id.SetType(impl::T_Generated);
      m_id.SetID(impl::ResourceManager::Instance()->GenerateUnique32());
      impl::ResourceManager::Instance()->RegisterResource(m_id, new impl::ResourceWrapper<T>(m_pObject));
      impl::ResourceManager::Instance()->RegisterUser(m_id);
    }

    // default constructor when T is derived from Resource
    template
      <
      typename U = T,
      typename = typename std::enable_if< std::is_base_of<Resource, U>::value >::type,
      typename = U
      >
      constexpr Ref(T* a_pObj)
      : m_pObject(a_pObj)
    {
      if (m_pObject == nullptr)
        return;

      m_id.SetType(impl::T_Generated);
      m_id.SetID(impl::ResourceManager::Instance()->GenerateUnique32());
      impl::ResourceManager::Instance()->RegisterResource(m_id, new impl::ResourceWrapper<T>(m_pObject));
      impl::ResourceManager::Instance()->RegisterUser(m_id);

      //TODO If T is const, we need to cast to Resource const *
      //TODO Once const is supported, search for and replace Refs that should be const
      dynamic_cast<Resource*>(m_pObject)->SetRefID(m_id);
    }

    //Construct from an already registered resource
    Ref(ResourceID a_id);

    //Construct from an already registered resource
    Ref(impl::ResourceID64 a_id);

    ~Ref();

    Ref(Ref const&);
    Ref& operator=(Ref const&);

    void reset();
    void reset(T *);

    T* operator->() noexcept;
    T& operator*() noexcept;
    T const * operator->() const noexcept;
    T const & operator*() const noexcept;

    bool IsNull() const;

  private:

    void RetrieveResource(impl::ResourceID64 a_id);

    impl::ResourceID64  m_id;
    T* m_pObject;
  };

  template<typename T>
  Ref<T>::Ref()
    : m_pObject(nullptr)
  {

  }

  template<typename T>
  void Ref<T>::RetrieveResource(impl::ResourceID64 a_id)
  {
    impl::ResourceWrapperBase* res = impl::ResourceManager::Instance()->GetResource(a_id, true);
    if (res != nullptr)
    {
      if (typeid(T) != res->GetType())
        LOG_WARN("Attempt to retrieve resource of different type! Casting anyway...");
      m_pObject = static_cast<T*>(res->GetPointer());
      m_id = a_id;
    }
    else
      m_id.SetNull();
  }

  template<typename T>
  Ref<T>::Ref(ResourceID a_id)
    : m_pObject(nullptr)
  {
    m_id.SetFlag(impl::ResourceID64::Flag::Persistant, true);
    m_id.SetType(impl::T_UserID);
    m_id.SetID(a_id);
    RetrieveResource(m_id);
  }

  template<typename T>
  Ref<T>::Ref(impl::ResourceID64 a_id)
    : m_pObject(nullptr)
    , m_id(a_id)
  {
    RetrieveResource(m_id);
  }

  template<typename T>
  Ref<T>::~Ref()
  {
    impl::ResourceManager::Instance()->DeregisterUser(m_id);
  }

  template<typename T>
  Ref<T>::Ref(Ref const& a_other)
    : m_id(a_other.m_id)
    , m_pObject(a_other.m_pObject)
  {
    impl::ResourceManager::Instance()->RegisterUser(m_id);
  }

  template<typename T>
  Ref<T>& Ref<T>::operator=(Ref const& a_other)
  {
    if (this != &a_other)
    {
      impl::ResourceManager::Instance()->DeregisterUser(m_id);
      m_id = a_other.m_id;
      m_pObject = a_other.m_pObject;
      impl::ResourceManager::Instance()->RegisterUser(m_id);
    }
    return *this;
  }

  template<typename T>
  T const * Ref<T>::operator->() const noexcept
  {
    return m_pObject;
  }

  template<typename T>
  T const & Ref<T>::operator*() const noexcept
  {
    return *m_pObject;
  }

  template<typename T>
  T* Ref<T>::operator->() noexcept
  {
    return m_pObject;
  }

  template<typename T>
  T& Ref<T>::operator*() noexcept
  {
    return *m_pObject;
  }

  template<typename T>
  bool Ref<T>::IsNull() const
  {
    return m_pObject == nullptr;
  }
}

#endif