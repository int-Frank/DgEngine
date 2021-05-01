//@group Renderer/RenderThread

#ifndef RT_BINDINGPOINT_H
#define RT_BINDINGPOINT_H

#include <stdint.h>
#include "ShaderUniform.h"

namespace Engine
{
  class RT_BindingPoint
  {
  public:

    RT_BindingPoint();
    ~RT_BindingPoint();

    static void Init();

    bool Capture(StorageBlockType, ShaderDomain);  //Finds the next binding point if one exists
    void Release();                                //Equivilant of Destroy()
    bool IsBound() const;
    void Unbind();

    uint32_t Address() const;
    ShaderDomain Domain() const;
    StorageBlockType Type() const;

  private:

    void Set(StorageBlockType, ShaderDomain, uint32_t index);

    struct BindingPointData
    {
      uint32_t bindingPoints;
      uint16_t begin;
      uint16_t count;
    };

    static BindingPointData s_addresses[SBT32(COUNT)][ShaderDomain_COUNT];

  private:
    uint32_t  m_bindingData;
  };
}

#endif