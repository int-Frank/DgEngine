//@group Memory

#ifndef RESOURCEID_H
#define RESOURCEID_H

#include <stdint.h>

namespace Engine
{
  //Internal use only. 
  typedef uint64_t RefID;
#define INVALID_REFID 0xFFFF'FFFF'FFFF'FFFFULL

  namespace impl
  {
    class ResourceID64
    {
      enum : uint64_t
      {
        Begin_Type = 32,
        Begin_ID   = 0,
        Count_Type = 16,
        Count_ID   = 32,
      };

    public:

      enum class Flag : uint64_t
      {
        Persistant = 0x8000'0000'0000'0000,
        UNUSED1    = 0x4000'0000'0000'0000,
        UNUSED2    = 0x2000'0000'0000'0000,
        UNUSED3    = 0x1000'0000'0000'0000,
        UNUSED4    = 0x800'0000'0000'0000,
        UNUSED5    = 0x400'0000'0000'0000,
        UNUSED6    = 0x200'0000'0000'0000,
        UNUSED7    = 0x100'0000'0000'0000,
      };

    public:

      ResourceID64();

      void SetFlag(Flag, bool);
      void SetType(uint16_t a_id);
      void SetID(uint32_t a_id);

      //48 bit integer. Includes type (16 bits) and id (32 bits)
      uint64_t GetID() const;

      bool Is(Flag) const;
      void SetNull();
      bool IsNull() const;

    private:
      uint64_t m_data;
    };
  }
}

#endif