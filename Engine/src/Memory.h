//@group Memory

#ifndef REF_H
#define REF_H

#include <memory>
#include <type_traits>
#include <stdint.h>
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

  //A wrapper which constructs and stores objects on the tempory buffer. Objects must be
  //trivially destructable.
  template<typename T>
  class TRef;

  template<typename A, typename B>
  TRef<A> StaticPointerCast(TRef<B> const&);

  template<typename A, typename B>
  TRef<A> DynamicPointerCast(TRef<B> const&);

  template<typename T>
  using Ref = std::shared_ptr<T>;

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
}

#endif