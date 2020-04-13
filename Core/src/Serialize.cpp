
#include <cstring>
#include <stdint.h>

#include "Serialize.h"

namespace Core
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
    return (uint32_t)a_str.length() + sizeof(uint32_t);
  }

  template<>
  void* Serialize<std::string>(void * a_dest, std::string const* a_str, size_t a_count)
  {
    void* pOut = a_dest;
    for (size_t i = 0; i < a_count; i++)
    {
      uint32_t length = (uint32_t)a_str[i].length();
      *static_cast<uint32_t*>(pOut) = length;
      pOut = AdvancePtr(pOut, sizeof(length));
      memcpy(pOut, a_str[i].data(), a_str[i].length());
      pOut = AdvancePtr(pOut, length);
    }
    return pOut;
  }

  template<>
  void const* Deserialize<std::string>(void const* a_src, std::string* a_str, size_t a_count)
  {
    void const * pIn = a_src;
    for (size_t i = 0; i < a_count; i++)
    {
      uint32_t length = *static_cast<uint32_t const*>(pIn);
      pIn = AdvancePtr(a_src, sizeof(length));
      a_str[i].clear();
      a_str[i].append(static_cast<char const*>(pIn), length);
      pIn = AdvancePtr(pIn, length);
    }
    return pIn;
  }
}