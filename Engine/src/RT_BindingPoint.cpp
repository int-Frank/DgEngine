//@group Renderer/RenderThread


#include "RT_BindingPoint.h"
#include "BSR_Assert.h"
#include <glad/glad.h>

namespace Engine
{
  RT_BindingPoint::BindingPointData RT_BindingPoint::s_addresses[SBT32(COUNT)][ShaderDomain_COUNT] = {};
  static uint32_t const INVALID_BINDING_ID = 0xFFFFFFFF;

  RT_BindingPoint::RT_BindingPoint()
    : m_bindingData(INVALID_BINDING_ID)
  {
  
  }

  RT_BindingPoint::~RT_BindingPoint()
  {
    Release();
  }

  /*
    If the max uniform blocks for each domain is 16, this function sets up binding points thusly:

    Storage Block Type | Domain   | Binding point begin | count
    -------------------|----------|---------------------|------
    Uniform            | Vertex   | 0                   | 16
    Uniform            | Geometry | 16                  | 16
    Uniform            | Fragment | 32                  | 16
    Shader Storage     | Vertex   | 0                   | 16
    Shader Storage     | Geometry | 16                  | 16
    Shader Storage     | Fragment | 32                  | 16
  */
  void RT_BindingPoint::Init()
  {
    int values[SBT32(COUNT)][ShaderDomain_COUNT] = {};

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
        if (values[i][j] > (sizeof(BindingPointData::bindingPoints) * CHAR_BIT))
          values[i][j] = (sizeof(BindingPointData::bindingPoints) * CHAR_BIT);

        s_addresses[i][j].begin = begin;
        s_addresses[i][j].count = values[i][j];
        begin += values[i][j];
      }
    }
  }

  uint32_t RT_BindingPoint::Address() const
  {
    return m_bindingData & 0xFFFF;
  }

  ShaderDomain RT_BindingPoint::Domain() const
  {
    return static_cast<ShaderDomain>((m_bindingData >> 16) & 0xFF);
  }

  StorageBlockType RT_BindingPoint::Type() const
  {
    return static_cast<StorageBlockType>((m_bindingData >> 24) & 0xFF);
  }

  void RT_BindingPoint::Set(StorageBlockType a_type,
                            ShaderDomain a_domain,
                            uint32_t a_index)
  {
    BSR_ASSERT(a_index < 0xFFFF, "Index to high!");
    BSR_ASSERT(a_type != StorageBlockType::COUNT, "Invalid Storage block type!");

    m_bindingData = a_index;
    m_bindingData = m_bindingData | (static_cast<uint32_t>(a_domain) << 16);
    m_bindingData = m_bindingData | (static_cast<uint32_t>(a_type) << 24);
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
        Set(a_type, a_domain, index);
        s_addresses[typeInd][domInd].bindingPoints = s_addresses[typeInd][domInd].bindingPoints | (1 << i);
        return true;
      }
    }
    return false;
  }

  void RT_BindingPoint::Release()
  {
    if (!IsBound())
      return;

    uint32_t domInd = static_cast<uint32_t>(Domain());
    uint32_t typeInd = static_cast<uint32_t>(Type());

    uint32_t index = Address() - s_addresses[typeInd][domInd].begin;
    s_addresses[typeInd][domInd].bindingPoints = s_addresses[typeInd][domInd].bindingPoints & ~(1 << index);

    m_bindingData = INVALID_BINDING_ID;
  }

  bool RT_BindingPoint::IsBound() const
  {
    return m_bindingData != INVALID_BINDING_ID;
  }
}