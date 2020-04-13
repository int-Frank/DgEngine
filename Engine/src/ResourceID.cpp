//@group Memory

#include "DgBit.h"
#include "ResourceID.h"

namespace Engine
{
  namespace impl
  {
    ResourceID64::ResourceID64()
      : m_data(0)
    {

    }

    void ResourceID64::SetFlag(Flag a_flag, bool a_on)
    {
      if (a_on)
        m_data = m_data | static_cast<uint64_t>(a_flag);
      else
        m_data = m_data & ~static_cast<uint64_t>(a_flag);
    }

    void ResourceID64::SetType(uint16_t a_type)
    {
      m_data = Dg::SetSubInt<uint64_t, Begin_Type, Count_Type>(m_data, uint64_t(a_type));
    }

    void ResourceID64::SetID(uint32_t a_id)
    {
      m_data = Dg::SetSubInt<uint64_t, Begin_ID, Count_ID>(m_data, uint64_t(a_id));
    }

    uint64_t ResourceID64::GetID() const
    {
      return m_data & Dg::Mask<uint64_t, 0, Count_Type + Count_ID>::value;
    }

    bool ResourceID64::Is(Flag a_flag) const
    {
      return (m_data & static_cast<uint64_t>(a_flag)) != 0;
    }

    void ResourceID64::SetNull()
    {
      m_data = 0;
    }

    bool ResourceID64::IsNull() const
    {
      return m_data == 0;
    }
  }
}