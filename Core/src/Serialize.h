#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <string>
#include <type_traits>
#include <new>

#include "core_utils.h"
#include "core_Assert.h"

namespace Core
{
  template<typename T>
  void * Serialize(void * a_dest, T const * a_ptr, size_t a_count = 1)
  {
    static_assert(std::is_fundamental<T>::value, "T is not a fundamental type");
    BSR_ASSERT(a_ptr != nullptr, "");

    void * pOut = a_dest;
    for (size_t i = 0; i < a_count; i++)
    {
      new(pOut) T(a_ptr[i]);
      pOut = AdvancePtr(pOut, sizeof(T));
    }

    return pOut;
  }

  template<>
  void* Serialize<byte>(void* dest, byte const* ptr, size_t count);

  template<typename T>
  void const* Deserialize(void const * a_src, T * a_out, size_t a_count = 1)
  {
    static_assert(std::is_fundamental<T>::value, "T is not a fundamental type");
    BSR_ASSERT(a_src != nullptr, "");

    void const * pOut = a_src;
    for (size_t i = 0; i < a_count; i++)
    {
      a_out[i] = *static_cast<T const*>(pOut);
      pOut = AdvancePtr(pOut, sizeof(T));
    }

    return pOut;
  }

  template<typename T>
  uint32_t SerializedSize(T const & a_val)
  {
    static_assert(std::is_pod<T>::value, "");
    return sizeof(T);
  }

  template<>
  uint32_t SerializedSize<std::string>(std::string const& a_str);

  template<>
  void* Serialize<std::string>(void* dest, std::string const * str, size_t count);

  template<>
  void const* Deserialize<std::string>(void const * src, std::string * str, size_t count);
}

#endif