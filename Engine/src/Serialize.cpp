//@group Core

#include <cstring>
#include <stdint.h>

#include "Serialize.h"

namespace Engine
{
  template<>
  void* Serialize<byte>(void* a_dest, byte const* a_ptr, size_t a_count)
  {
    memcpy(a_dest, a_ptr, a_count);
    return AdvancePtr(a_dest, a_count);
  }

  template<>
  uint32_t SerializedSize<std::string>(std::string const& a_str)
  {
    return (uint32_t)a_str.length() + 1;
  }

  template<>
  void* Serialize<std::string>(void * a_dest, std::string const* a_str, size_t a_count)
  {
    void* pOut = a_dest;
    for (size_t i = 0; i < a_count; i++)
    {
      memcpy(pOut, a_str[i].data(), a_str[i].length());
      pOut = AdvancePtr(pOut, a_str[i].length() + 1);
      *((uint8_t*)pOut - 1) = 0; // null terminated
    }
    return pOut;
  }

  template<>
  void const* Deserialize<std::string>(void const* a_src, std::string* a_str, size_t a_count)
  {
    void const * pIn = a_src;
    for (size_t i = 0; i < a_count; i++)
    {
      a_str[i] = (char *)pIn;
      pIn = AdvancePtr(pIn, a_str[i].length() + 1);
    }
    return pIn;
  }
}