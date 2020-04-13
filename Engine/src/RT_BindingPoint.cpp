//@group Renderer/RenderThread

#include <glad/glad.h>

#include "RT_BindingPoint.h"
#include "core_Assert.h"
#include "core_Log.h"

namespace Engine
{
  RT_BindingPoint::Domain RT_BindingPoint::s_addresses[ShaderDomain_COUNT][ShaderDomain_COUNT];

  //------------------------------------------------------------------------------------------------
  // BindingPointID
  //------------------------------------------------------------------------------------------------

  BindingPointID::BindingPointID()
    : m_data(INVALID_ID)
  {
    SetInvalid();
  }

  bool BindingPointID::IsValid() const
  {
    return m_data != INVALID_ID;
  }

  uint32_t BindingPointID::Address() const
  {
    return m_data & 0xFFFF;
  }

  ShaderDomain BindingPointID::Domain() const
  {
    return static_cast<ShaderDomain>((m_data >> 16) & 0xFF);
  }

  StorageBlockType BindingPointID::Type() const
  {
    return static_cast<StorageBlockType>((m_data >> 24) & 0xFF);
  }

  void BindingPointID::SetInvalid()
  {
    m_data = INVALID_ID;
  }

  void BindingPointID::Set(StorageBlockType a_type,
                           ShaderDomain a_domain,
                           uint32_t a_index)
  {
    BSR_ASSERT(a_index < 0xFFFF, "Index to high!");
    BSR_ASSERT(a_type != StorageBlockType::COUNT, "Invalid Storage block type!");

    m_data = a_index;
    m_data = m_data | (static_cast<uint32_t>(a_domain) << 16);
    m_data = m_data | (static_cast<uint32_t>(a_type) << 24);
  }

  //------------------------------------------------------------------------------------------------
  // RT_BindingPoint
  //------------------------------------------------------------------------------------------------

  void RT_BindingPoint::Init()
  {
    int values[ShaderDomain_COUNT][ShaderDomain_COUNT];

    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &values[SBT32(Uniform)][SD32(Vertex)]);
    glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &values[SBT32(Uniform)][SD32(Geometry)]);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &values[SBT32(Uniform)][SD32(Fragment)]);

    glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &values[SBT32(ShaderStorage)][SD32(Vertex)]);
    glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &values[SBT32(ShaderStorage)][SD32(Geometry)]);
    glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &values[SBT32(ShaderStorage)][SD32(Fragment)]);

    for (int i = 0; i < SBT32(COUNT); i++)
    {
      uint16_t begin = 0;
      for (int j = 0; j < ShaderDomain_COUNT; j++)
      {
        s_addresses[i][j].begin = begin;
        s_addresses[i][j].count = values[i][j];
        begin += values[i][j];
      }
    }
  }

  bool RT_BindingPoint::Capture(StorageBlockType a_type, ShaderDomain a_domain)
  {
    Release();

    uint32_t domInd = static_cast<uint32_t>(a_domain);
    uint32_t typeInd = static_cast<uint32_t>(a_type);

    for (uint16_t i = 0; i < s_addresses[typeInd][domInd].count; i++)
    {
      uint32_t val = (1 << i) & s_addresses[typeInd][domInd].bindingPoints;
      if (val == 0)
      {
        uint32_t index = s_addresses[typeInd][domInd].begin + i;
        m_bindingIndex.Set(a_type, a_domain, index);
        s_addresses[typeInd][domInd].bindingPoints = s_addresses[typeInd][domInd].bindingPoints | (1 << i);
        return true;
      }
    }
    LOG_WARN("Binding point failed to capture a value!");
    return false;
  }

  void RT_BindingPoint::Release()
  {
    uint32_t domInd = static_cast<uint32_t>(m_bindingIndex.Domain());
    uint32_t typeInd = static_cast<uint32_t>(m_bindingIndex.Type());

    uint32_t index = m_bindingIndex.Address() - s_addresses[typeInd][domInd].begin;
    s_addresses[typeInd][domInd].bindingPoints =
      s_addresses[typeInd][domInd].bindingPoints & ~(1 << index);
  }

  bool RT_BindingPoint::IsBound() const
  {
    return m_bindingIndex.IsValid();
  }

  BindingPointID RT_BindingPoint::GetID() const
  {
    return m_bindingIndex;
  }
}