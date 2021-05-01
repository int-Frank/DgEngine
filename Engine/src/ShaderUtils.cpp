//@group Renderer

#include <glad/glad.h>

#include "ShaderUtils.h"
#include "BSR_Assert.h"

namespace Engine
{
  struct ShaderDataTypeData
  {
    ShaderDataType      dataType;
    ShaderDataClass     dataClass;
    ShaderDataBaseType  baseType;
    std::string         str;
    GLenum              glenum;
    uint32_t            componentCount;
    ShaderDataType      matColumnType;
    ShaderDataType      matRowType;
  };

#define SET_FLAGS(Class, BaseType) (static_cast<uint32_t>(Class) | (static_cast<uint32_t>(BaseType) << 3))
#define GET_CLASS_FROM_FLAG(i) (i & 7)
#define GET_BASE_TYPE_FROM_FLAG(i) (i >> 3)

  static ShaderDataTypeData const SDT_data[] = 
  { 
    //     Type              |         Class            |      Base type             |      string      | GLenum              |cc | matrix column type  |    matrix row type      |ubs |uba |uss                  |usa      
    {ShaderDataType::NONE,      ShaderDataClass::None,     ShaderDataBaseType::None,     "Invalid Type", GL_INVALID_ENUM,      0,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::BOOL,      ShaderDataClass::Scalar,   ShaderDataBaseType::Bool,     "bool",         GL_BOOL,              1,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::INT,       ShaderDataClass::Scalar,   ShaderDataBaseType::Int,      "int",          GL_INT,               1,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::UINT,      ShaderDataClass::Scalar,   ShaderDataBaseType::UInt,     "uint",         GL_UNSIGNED_INT,      1,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::FLOAT,     ShaderDataClass::Scalar,   ShaderDataBaseType::Float,    "float",        GL_FLOAT,             1,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::BVEC2,     ShaderDataClass::Vector,   ShaderDataBaseType::Bool,     "bvec2",        GL_BOOL_VEC2,         2,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::BVEC3,     ShaderDataClass::Vector,   ShaderDataBaseType::Bool,     "bvec3",        GL_BOOL_VEC3,         3,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::BVEC4,     ShaderDataClass::Vector,   ShaderDataBaseType::Bool,     "bvec4",        GL_BOOL_VEC4,         4,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::IVEC2,     ShaderDataClass::Vector,   ShaderDataBaseType::Int,      "ivec2",        GL_INT_VEC2,          2,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::IVEC3,     ShaderDataClass::Vector,   ShaderDataBaseType::Int,      "ivec3",        GL_INT_VEC3,          3,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::IVEC4,     ShaderDataClass::Vector,   ShaderDataBaseType::Int,      "ivec4",        GL_INT_VEC4,          4,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::UVEC2,     ShaderDataClass::Vector,   ShaderDataBaseType::UInt,     "uvec2",        GL_UNSIGNED_INT_VEC2, 2,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::UVEC3,     ShaderDataClass::Vector,   ShaderDataBaseType::UInt,     "uvec3",        GL_UNSIGNED_INT_VEC3, 3,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::UVEC4,     ShaderDataClass::Vector,   ShaderDataBaseType::UInt,     "uvec4",        GL_UNSIGNED_INT_VEC3, 4,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::VEC2,      ShaderDataClass::Vector,   ShaderDataBaseType::Float,    "vec2",         GL_FLOAT_VEC2,        2,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::VEC3,      ShaderDataClass::Vector,   ShaderDataBaseType::Float,    "vec3",         GL_FLOAT_VEC3,        3,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::VEC4,      ShaderDataClass::Vector,   ShaderDataBaseType::Float,    "vec4",         GL_FLOAT_VEC4,        4,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::MAT2,      ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat2",         GL_FLOAT_MAT2,        4,  ShaderDataType::VEC2,   ShaderDataType::VEC2},
    {ShaderDataType::MAT2x2,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat2x2",       GL_FLOAT_MAT2,        4,  ShaderDataType::VEC2,   ShaderDataType::VEC2},
    {ShaderDataType::MAT3,      ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat3",         GL_FLOAT_MAT3,        9,  ShaderDataType::VEC3,   ShaderDataType::VEC3},
    {ShaderDataType::MAT3x3,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat3x3",       GL_FLOAT_MAT3,        9,  ShaderDataType::VEC3,   ShaderDataType::VEC3},
    {ShaderDataType::MAT4,      ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat4",         GL_FLOAT_MAT4,        16, ShaderDataType::VEC4,   ShaderDataType::VEC4},
    {ShaderDataType::MAT4x4,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat4x4",       GL_FLOAT_MAT4,        16, ShaderDataType::VEC4,   ShaderDataType::VEC4},
    {ShaderDataType::MAT2x3,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat2x3",       GL_FLOAT_MAT2x3,      6,  ShaderDataType::VEC2,   ShaderDataType::VEC2},
    {ShaderDataType::MAT2x4,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat2x4",       GL_FLOAT_MAT2x4,      8,  ShaderDataType::VEC2,   ShaderDataType::VEC4},
    {ShaderDataType::MAT3x2,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat3x2",       GL_FLOAT_MAT3x2,      6,  ShaderDataType::VEC3,   ShaderDataType::VEC2},
    {ShaderDataType::MAT3x4,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat3x4",       GL_FLOAT_MAT3x4,      12, ShaderDataType::VEC3,   ShaderDataType::VEC4},
    {ShaderDataType::MAT4x2,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat4x2",       GL_FLOAT_MAT4x2,      8,  ShaderDataType::VEC4,   ShaderDataType::VEC2},
    {ShaderDataType::MAT4x3,    ShaderDataClass::Matrix,   ShaderDataBaseType::Float,    "mat4x3",       GL_FLOAT_MAT4x3,      12, ShaderDataType::VEC4,   ShaderDataType::VEC3},
    {ShaderDataType::TEXTURE2D, ShaderDataClass::Texture,  ShaderDataBaseType::UInt,     "sampler2D",    GL_TEXTURE_2D,        1,  ShaderDataType::NONE,   ShaderDataType::NONE},
    {ShaderDataType::STRUCT,    ShaderDataClass::Struct,   ShaderDataBaseType::None,     "struct",       GL_INVALID_ENUM,      0,  ShaderDataType::NONE,   ShaderDataType::NONE},
  };

  /*static char const * ShaderResourceType_strings[] = 
  {
    "Invalid Type",
    "sampler2D", "samplerCube"
  };*/

  ShaderDomains::ShaderDomains()
    : m_data(0)
  {

  }

  void ShaderDomains::AddDomain(ShaderDomain a_domain)
  {
    m_data |= (1 << static_cast<uint32_t>(a_domain));
  }

  void ShaderDomains::AddDomains(ShaderDomains const a_domains)
  {
    m_data |= a_domains.m_data;
  }

  void ShaderDomains::RemoveDomain(ShaderDomain a_domain)
  {
    m_data &= ~(1 << static_cast<uint32_t>(a_domain));
  }

  bool ShaderDomains::IsDomain(ShaderDomain a_domain) const
  {
    return ((1 << static_cast<uint32_t>(a_domain)) & m_data) != 0;
  }

  ShaderDataType StringToShaderDataType(std::string const& a_type)
  {
    for (uint32_t i = 1; i < static_cast<uint32_t>(ShaderDataType::STRUCT); i++)
    {
      if (a_type == SDT_data[i].str)
        return static_cast<ShaderDataType>(i);
    }
    return ShaderDataType::NONE;
  }

  std::string ShaderDataTypeToString(ShaderDataType a_type)
  {
    return SDT_data[static_cast<uint32_t>(a_type)].str;
  }

  ShaderDataType uint_ToShaderDataType(uint32_t a_val)
  {
    if (a_val <= static_cast<uint32_t>(ShaderDataType::STRUCT))
      return static_cast<ShaderDataType>(a_val);
    return ShaderDataType::NONE;
  }

  uint32_t GetComponentCount(ShaderDataType a_type)
  {
    return SDT_data[static_cast<uint32_t>(a_type)].componentCount;
  }

  uint32_t SizeOfShaderDataType(ShaderDataType a_type)
  {
    uint32_t nElements = GetComponentCount(a_type);
    ShaderDataBaseType baseType = GetShaderDataBaseType(a_type);
    uint32_t baseTypeSize = SizeOfShaderDataBaseType(baseType);
    return baseTypeSize * nElements;
  }

  ShaderDataType GetRowVectorFromMatrix(ShaderDataType a_type)
  {
    uint32_t ind = static_cast<uint32_t>(a_type);
    BSR_ASSERT(SDT_data[ind].dataClass == ShaderDataClass::Matrix, "");
    return SDT_data[ind].matRowType;
  }

  ShaderDataType GetColumnVectorFromMatrix(ShaderDataType a_type)
  {
    uint32_t ind = static_cast<uint32_t>(a_type);
    BSR_ASSERT(SDT_data[ind].dataClass == ShaderDataClass::Matrix, "");
    return SDT_data[ind].matColumnType;
  }

  ShaderDataClass GetShaderDataClass(ShaderDataType a_type)
  {
    return SDT_data[static_cast<uint32_t>(a_type)].dataClass;
  }

  ShaderDataBaseType GetShaderDataBaseType(ShaderDataType a_type)
  {
    return SDT_data[static_cast<uint32_t>(a_type)].baseType;
  }

  uint32_t SizeOfShaderDataBaseType(ShaderDataBaseType a_type)
  {
    static uint32_t const s_sizes[6] = {0, 4, 4, 4, 8, 4};
    return s_sizes[static_cast<uint32_t>(a_type)];
  }

  GLenum ShaderDataTypeToOpenGLType(ShaderDataType const a_type)
  {
    return SDT_data[static_cast<uint32_t>(a_type)].glenum;
  }

  GLenum ShaderDataBaseTypeToOpenGLType(ShaderDataBaseType a_type)
  {
    static GLenum const s_types[] =
    {
      GL_INVALID_ENUM,
      GL_BOOL,
      GL_INT,
      GL_UNSIGNED_INT,
      GL_INVALID_ENUM,
      GL_FLOAT,
    };
    return s_types[static_cast<uint32_t>(a_type)];
  }

  GLenum ShaderDomainToOpenGLType(ShaderDomain a_type)
  {
    switch (a_type)
    {
      case ShaderDomain::Vertex:
        return GL_VERTEX_SHADER;
      case ShaderDomain::Fragment:
        return GL_FRAGMENT_SHADER;
      case ShaderDomain::Geometry:
        return GL_GEOMETRY_SHADER;
    }
    BSR_ASSERT(false, "");
    return GL_INVALID_ENUM;
  }
}