//@group Renderer

#ifndef RENDERSTATE_H
#define RENDERSTATE_H

#include <stdint.h>

#include "DgBit.h"

#define RENDER_STATE_ATTR \
 ITEM(Layer)\
 ITEM(Type)\
 ITEM(Command)\
 ITEM(Group)\
 ITEM(Translucency)\
 ITEM(Depth)\
 ITEM(VAO)\
 ITEM(Material)\

namespace Engine
{
  class RenderState
  {
  private:

    struct Count
    {
      enum : uint32_t
      {
        BitBegin = 0,
        BitCount = 7,

        Layer         = 4,
        Type          = 4,

        //Type == Command
        Command       = 40,
        Group         = 16, //internal, do not use

        //Type == DrawCall
        Translucency  = 2,
        Depth         = 16,
        VAO           = 8,
        Material      = 30,
      };

      static_assert(Layer + Type + Command + Group == 64);
      static_assert(Layer + Type + Translucency + Depth + VAO + Material == 64);
    };

    struct Begin
    {
      enum : uint32_t
      {
        BitBegin = Count::BitBegin + Count::BitCount,
        BitCount = 6,

        Command       = 0,
        Group         = Count::Command, //internal

        Material      = 0,
        VAO           = Count::Material,
        Depth         = VAO + Count::VAO,
        Translucency  = Depth + Count::Depth,
        Type          = Translucency + Count::Translucency,
        Layer         = Type + Count::Type,
      };
    };

  public:

    struct Type
    {
      enum : uint64_t
      {
        DrawCall,
        Command
      };
    };

    struct Command
    {
      enum : uint64_t
      {
        None,

        SwapWindow,
        SetClearColor,
        Clear,
        Draw,

        BufferCreate,
        BufferDelete,
        BufferSetData,
        BufferBind,
        BufferSetLayout,
        Buffer_END,

        VertexArrayCreate,
        VertexArrayDelete,
        VertexArrayBind,
        VertexArrayUnbind,
        VertexArrayAddVertexBuffer,
        VertexArraySetIndexBuffer,
        VertexArray_END,

        IndexedBufferBind,

        BindingPointCreate,
        BindingPointDelete,

        RendererProgramCreate,
        RendererProgramDelete,
        RendererProgramInit,
        RendererProgramDestroy,
        RendererProgramBind,
        RendererProgramUnbind,
        RendererProgramUploadUniform,

        MaterialBind,

        TextureCreate,
        TextureDelete,
        TextureBindToSlot,
      };
    };

    struct Layer
    {
      enum : uint64_t
      {
        None,
        Game,
        Effect,
        HUD
      };
    };

    struct Translucency
    {
      enum : uint64_t
      {
        None,
        Additive,
        Subtractive
      };
    };

    typedef uint32_t AttrInt;

    struct Attr
    {
      enum : AttrInt
      {
#undef ITEM
#define ITEM(x) x = (Begin::x << Begin::BitBegin) | (Count::x),
        RENDER_STATE_ATTR
#undef ITEM
      };
    };


  public:
     
    static RenderState Create();

    //No constructors/destructor. We want this to be a POD.

    template<AttrInt T>  void Set(uint64_t);
    template<AttrInt T>  uint64_t Get() const;

#define ITEM(x) template<>\
    inline void Set<Attr::x>(uint64_t a_val)\
    {\
      m_data &= ~Dg::Mask<uint64_t, Begin::x, Count::x>::value;\
      uint64_t val = (a_val << Begin::x) & Dg::Mask<uint64_t, Begin::x, Count::x>::value;\
      m_data |= val;\
    }
    RENDER_STATE_ATTR
#undef ITEM

#define ITEM(x) template<>\
      inline uint64_t Get<Attr::x>() const\
    {\
      return (m_data & Dg::Mask<uint64_t, Begin::x, Count::x>::value) >> Begin::x;\
    }
      RENDER_STATE_ATTR
#undef ITEM

    void Set(AttrInt, uint64_t val);
    uint64_t Get(AttrInt) const;

    static uint64_t ComputeNormalizedDepth(float a_min, float a_max, float a_val);

  private:

    uint64_t m_data;
  };
}

#endif