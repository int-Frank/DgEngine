//@group Renderer

#include "Renderer.h"
#include "core_Log.h"
#include "DgBit.h"
#include "DgMath.h"

namespace Engine
{
  RenderState RenderState::Create()
  {
    RenderState state;
    state.m_data = 0;
    state.Set<Attr::Layer>(Layer::None);
    return state;
  }

  void RenderState::Set(AttrInt a_attr, uint64_t a_val)
  {
    uint32_t bitBegin = Dg::GetSubInt<uint32_t, Begin::BitBegin, Begin::BitCount>(a_attr);
    uint32_t bitCount = Dg::GetSubInt<uint32_t, Count::BitBegin, Count::BitCount>(a_attr);
    m_data = Dg::SetSubInt<uint64_t>(m_data, a_val, bitBegin, bitCount);
  }

  uint64_t RenderState::Get(AttrInt a_attr) const
  {
    uint32_t bitBegin = Dg::GetSubInt<uint32_t, Begin::BitBegin, Begin::BitCount>(a_attr);
    uint32_t bitCount = Dg::GetSubInt<uint32_t, Count::BitBegin, Count::BitCount>(a_attr);
    return Dg::GetSubInt<uint64_t>(m_data, bitBegin, bitCount);
  }

  uint64_t RenderState::ComputeNormalizedDepth(float a_min, float a_max, float a_val)
  {
    float val = a_val - a_min;
    float range = a_max - a_min;
    Dg::ClampNumber(0.0f, range, val);

    float upper = static_cast<float>((1 << Count::Depth) - 1);

    return static_cast<uint64_t>((val / range) * upper);
  }
}