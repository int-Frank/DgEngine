//@group Renderer/RenderThread

#ifndef RT_BINDINGPOINT_H
#define RT_BINDINGPOINT_H

#include <stdint.h>
#include "ShaderUniform.h"

namespace Engine
{
  //Domain is included in the ID
  class BindingPointID
  {
    friend class RT_BindingPoint;
  public:

    BindingPointID();

    bool IsValid() const;
    uint32_t Address() const;
    ShaderDomain Domain() const;
    StorageBlockType Type() const;

  private:

    void Set(StorageBlockType, ShaderDomain, uint32_t index);
    void SetInvalid();

  private:
    static uint32_t const INVALID_ID = 0xFFFFFFFF;
    uint32_t m_data;
  };

  class RT_BindingPoint
  {
  public:

    static void Init();

    bool Capture(StorageBlockType, ShaderDomain);  //Finds the next binding point if it exists
    void Release();                                //Equivilant of Destroy()
    bool IsBound() const;
    BindingPointID GetID() const;

  private:

    struct Domain
    {
      Domain()
        : bindingPoints(0)
        , begin(0)
        , count(0)
      {
      }

      uint32_t bindingPoints;
      uint16_t begin;
      uint16_t count;
    };

    static Domain s_addresses[ShaderDomain_COUNT][ShaderDomain_COUNT];

  private:
    BindingPointID  m_bindingIndex;
  };
}

#endif