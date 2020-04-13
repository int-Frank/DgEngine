//@group Renderer

/*
  Original Copyright Yan Chernikov <github.com/TheCherno/Hazel-dev> and contributors.

  The following code is a derivative work of the code from the GitHub project 'Hazel-dev',
  which is licensed under:

                                  Apache License
                             Version 2.0, January 2004
                          http://www.apache.org/licenses

  This code therefore is also licensed under the terms
  of the Apache License Version 2.0

  Copyright 2017-2019 Frank Hart <frankhart010@gmail.com>
*/

#include <regex>

#include "DgStringFunctions.h"

#include "ShaderUniform.h"
#include "Renderer.h"
#include "RT_BindingPoint.h"
#include "RenderThreadData.h"

#include "core_Log.h"
#include "DgMath.h"
#include "core_utils.h"
#include "Serialize.h"
#include "core_Assert.h"
#include "DgBit.h"

#define ALIGN Dg::ForwardAlign<uint32_t>

//Helpful regex expressions
#define REG_UNIFORM "(?:uniform)"
#define REG_STRUCT "(?:struct)"
#define _OS_ "[\\s\\n\\r]*"
#define _S_ "[\\s\\n\\r]+"
#define VAR "([_a-zA-Z][_a-zA-Z0-9]*)"
#define OARRAY "(?:(?:\\[)" _OS_ "([0-9]+)" _OS_ "(?:\\]))?"
#define SC "[;]"

#define STD140_DECL "(?:layout)" _OS_ "[(]" _OS_ "(?:std140)" _OS_ "[)]"
#define BLOCK_CONTENTS "(?:[^{]*)[{]([^}]*)"

#define UNIFORM_BLOCK_EXPRESSION STD140_DECL _OS_ REG_UNIFORM _S_ VAR BLOCK_CONTENTS

#define VAR_EXPRESSION                            VAR _S_ VAR _OS_ OARRAY _OS_ SC
#define UNIFORM_VAR_EXPRESSION    REG_UNIFORM _S_ VAR _S_ VAR _OS_ OARRAY _OS_ SC
#define STRUCT_EXPRESSION         REG_STRUCT  _S_ VAR BLOCK_CONTENTS

namespace Engine
{
  //---------------------------------------------------------------------------------------------------
  // UniformBufferElementHeader
  //---------------------------------------------------------------------------------------------------
  namespace impl_UniformBufferElementHeader
  {

    enum class Size : uint32_t
    {
      Size          = 24,
      Flags         = 8
    };

    enum class Begin : uint32_t
    {
      Size         = 0,
      Flags         = Size + static_cast<uint32_t>(Size::Size)
    };

    static uint32_t const INTSIZE = uint32_t(sizeof(uint32_t));
  }

  UniformBufferElementHeader::UniformBufferElementHeader()
    : m_data(0)
  {
  
  }

  size_t UniformBufferElementHeader::SerializedSize() const
  {
    return sizeof(uint32_t);
  }

  void * UniformBufferElementHeader::Serialize(void * a_buf) const
  {
    return Core::Serialize(a_buf, &m_data);
  }

  void const * UniformBufferElementHeader::Deserialize(void const * a_buf)
  {
    return Core::Deserialize(a_buf, &m_data);
  }

  void UniformBufferElementHeader::SetSize(uint32_t a_count)
  {
    BSR_ASSERT(a_count <= uint32_t(Dg::Mask<uint32_t,
      static_cast<uint32_t>(impl_UniformBufferElementHeader::Begin::Size),
      static_cast<uint32_t>(impl_UniformBufferElementHeader::Size::Size)>::value));
    
    m_data = Dg::SetSubInt<uint32_t,
      static_cast<uint32_t>(impl_UniformBufferElementHeader::Begin::Size),
      static_cast<uint32_t>(impl_UniformBufferElementHeader::Size::Size)>(m_data, a_count);
  }

  void UniformBufferElementHeader::SetFlag(Flag a_flag, bool a_val)
  {
    uint32_t val = a_val ? 1 : 0;
    uint32_t shft = a_flag + static_cast<uint32_t>(impl_UniformBufferElementHeader::Begin::Flags);
    m_data = Dg::SetSubInt<uint32_t>(m_data, val, (1 << shft), 1);
  }

  uint32_t UniformBufferElementHeader::GetSize() const
  {
    return Dg::GetSubInt<uint32_t,
      static_cast<uint32_t>(impl_UniformBufferElementHeader::Begin::Size),
      static_cast<uint32_t>(impl_UniformBufferElementHeader::Size::Size)>(m_data);
  }

  bool UniformBufferElementHeader::Is(Flag a_flag) const
  {
    uint32_t shft = a_flag + static_cast<uint32_t>(impl_UniformBufferElementHeader::Begin::Flags);
    return Dg::GetSubInt<uint32_t>(m_data, (1 << shft), 1) != 0;
  }

  //---------------------------------------------------------------------------------------------------
  // ShaderStruct
  //---------------------------------------------------------------------------------------------------
  class ShaderStruct
  {
  public:

    ShaderStruct(std::string const&);

    friend bool operator==(ShaderStruct const&, ShaderStruct const&);

    void AddField(ShaderUniformDeclaration const&);
    std::string const& GetName() const;
    Dg::DynamicArray<ShaderUniformDeclaration> const& GetFields() const;

  private:
    std::string m_name;
    Dg::DynamicArray<ShaderUniformDeclaration> m_fields;
  };

  struct ShaderData::ShaderStructList
  {
    Dg::DynamicArray<ShaderStruct> data;
  };

  ShaderStruct::ShaderStruct(std::string const & a_name)
    : m_name(a_name)
  {

  }
  
  void ShaderStruct::AddField(ShaderUniformDeclaration const & a_field)
  {
    m_fields.push_back(a_field);
  }

  std::string const & ShaderStruct::GetName() const
  {
    return m_name;
  }

  Dg::DynamicArray<ShaderUniformDeclaration> const & ShaderStruct::GetFields() const
  {
    return m_fields;
  }

  //---------------------------------------------------------------------------------------------------
  // std140ItemDeclaration
  //---------------------------------------------------------------------------------------------------

  std140ItemDeclaration::std140ItemDeclaration(ShaderDataType a_type,
                                               uint32_t a_count,
                                               MatrixLayout a_layout)
    : m_type(a_type)
    , m_count(a_count)
    , m_matLayout(a_layout)
    , m_frontPadding(0)
  {
    BSR_ASSERT(a_type != ShaderDataType::NONE, "");
  }

  void * std140ItemDeclaration::CopyToBuffer(void * a_buffer, void const * a_data) const
  {
    BSR_ASSERT(m_type != ShaderDataType::NONE, "");

    if (m_count == 0)
      return a_buffer;

    //align first
    void * buf = Core::AdvancePtr(a_buffer, m_frontPadding);

    //copy data
    if (m_type == ShaderDataType::STRUCT)
      return buf; //It's just padding

    uint32_t dataSize = 0;
    uint32_t padding = 0;
    uint32_t count = 0;

    ShaderDataBaseType baseType = GetShaderDataBaseType(m_type);
    uint32_t baseTypeSize = SizeOfShaderDataBaseType(baseType);

    if (GetShaderDataClass(m_type) == ShaderDataClass::Matrix)
    {
      ShaderDataType rowType = GetRowVectorFromMatrix(m_type);
      ShaderDataType columnType = GetColumnVectorFromMatrix(m_type);
      uint32_t nRowElements = GetComponentCount(rowType);
      uint32_t nColumnElements = GetComponentCount(columnType);

      if (m_matLayout == MatrixLayout::RowMajor)
      {
        count = m_count * nColumnElements; 
        dataSize = nRowElements * baseTypeSize;
        uint32_t stride = std140StrideArray(rowType);
        padding = stride - dataSize;
      }
      else
      {
        count = m_count * nRowElements;
        dataSize = nColumnElements * baseTypeSize;
        uint32_t stride = std140StrideArray(columnType);
        padding = stride - dataSize;
      }
    }
    else
    {
      uint32_t nElements = GetComponentCount(m_type);
      dataSize = nElements * baseTypeSize;
      uint32_t stride(0);
      if (m_count == 1)
        stride = std140StrideSingle(m_type);
      else
        stride = std140StrideArray(m_type);
      padding = stride - dataSize;
    }

    byte const * pSrc = static_cast<byte const *>(a_data);
    for (uint32_t c = 0; c < count; c++)
    {
      buf = Core::Serialize<byte>(buf, pSrc, dataSize);
      buf = Core::AdvancePtr(buf, padding);
    }

    return buf;
  }

  uint32_t std140ItemDeclaration::Count() const
  {
    return m_count;
  }

  ShaderDataType std140ItemDeclaration::Type() const
  {
    return m_type;
  }

  void std140ItemDeclaration::SetBaseAlignment(uint32_t a_beginOffset)
  {
    BSR_ASSERT(m_type != ShaderDataType::STRUCT, "Cannot set the base alignment of a struct! Use 'SetPadding()' instead");

    uint32_t alignment(0);
    if (m_count == 1)
      alignment = std140BaseAlignmentSingle(m_type);
    else
      alignment = std140BaseAlignmentSingle(m_type);
    uint32_t offset = ALIGN(a_beginOffset, alignment);
    m_frontPadding = offset - a_beginOffset;
  }

  void std140ItemDeclaration::SetFrontPadding(uint32_t a_padding)
  {
    BSR_ASSERT(m_type == ShaderDataType::STRUCT, "Can only set padding of a STRUCT type!");
    m_frontPadding = a_padding;
  }

  uint32_t std140ItemDeclaration::Stride() const
  {
    if (m_count == 1)
      return std140StrideSingle(m_type);
    return std140StrideArray(m_type);
  }

  uint32_t std140ItemDeclaration::FrontPadding() const
  {
    return m_frontPadding;
  }

  bool operator==(std140ItemDeclaration const& a_item_0, std140ItemDeclaration const& a_item_1)
  {
    bool result = a_item_0.m_type == a_item_1.m_type;
    result &= (a_item_0.m_count == a_item_1.m_count);
    result &= (a_item_0.m_matLayout == a_item_1.m_matLayout);
    return result;
  }

  bool operator!=(std140ItemDeclaration const& a_item_0, std140ItemDeclaration const& a_item_1)
  {
    bool result = a_item_0.m_type == a_item_1.m_type;
    result &= (a_item_0.m_count == a_item_1.m_count);
    result &= (a_item_0.m_matLayout == a_item_1.m_matLayout);
    return !result;
  }

  //---------------------------------------------------------------------------------------------------
  // std140UniformBlock
  //---------------------------------------------------------------------------------------------------

  bool operator==(std140UniformBlock const & a_block_0, std140UniformBlock const & a_block_1)
  {
    if (a_block_0.m_items.size() != a_block_1.m_items.size())
      return false;

    for (uint32_t i = 0; i < uint32_t(a_block_0.m_items.size()); i++)
    {
      if (a_block_0.m_items[i] != a_block_1.m_items[i])
        return false;
    }
    return true;
  }

  //---------------------------------------------------------------------------------------------------
  // std140UniformBlockBuffer
  //---------------------------------------------------------------------------------------------------

  //---------------------------------------------------------------------------------------------------
  // ShaderUniformDeclaration
  //---------------------------------------------------------------------------------------------------
  ShaderUniformDeclaration::ShaderUniformDeclaration(ShaderDataType a_type, 
                                                     std::string a_name,
                                                     bool a_isArray,
                                                     uint32_t a_count)
    : m_type(a_type)
    , m_name(a_name)
    , m_count(a_count)
    , m_isArray(a_isArray)
    , m_dataOffset(0)
  {
    BSR_ASSERT(a_type != ShaderDataType::STRUCT);
  }
  
  void ShaderUniformDeclaration::Log() const
  {
    LOG_DEBUG("UNIFORM - name: {}, count: {}, type: {}, IsVS: {}, IsFS: {}, IsGS: {}",
      m_name.c_str(), m_count, ShaderDataTypeToString(m_type).c_str(), 
      m_domains.IsDomain(ShaderDomain::Vertex), 
      m_domains.IsDomain(ShaderDomain::Fragment), 
      m_domains.IsDomain(ShaderDomain::Geometry));
  }

  std::string ShaderUniformDeclaration::GetName() const
  {
    return m_name;
  }

  uint32_t ShaderUniformDeclaration::GetCount() const
  {
    return m_count;
  }

  ShaderDomains& ShaderUniformDeclaration::GetDomains()
  {
    return m_domains;
  }

  ShaderDataType ShaderUniformDeclaration::GetType() const
  {
    return m_type;
  }

  bool ShaderUniformDeclaration::IsArray() const
  {
    return m_count > 1;
  }

  uint32_t ShaderUniformDeclaration::GetDataSize() const
  {
    return SizeOfShaderDataType(m_type) * m_count + impl_UniformBufferElementHeader::INTSIZE;
  }

  void ShaderUniformDeclaration::SetDataOffset(uint32_t a_offset)
  {
    m_dataOffset = a_offset;
  }

  uint32_t ShaderUniformDeclaration::GetDataOffset() const
  {
    return m_dataOffset;
  }

  bool operator==(ShaderUniformDeclaration const& a_uniform_0, ShaderUniformDeclaration const& a_uniform_1)
  {
    bool result = a_uniform_0.m_name == a_uniform_1.m_name;
    result = result && (a_uniform_0.m_type == a_uniform_1.m_type);
    result = result && (a_uniform_0.m_count == a_uniform_1.m_count);
    return result;
  }

  //---------------------------------------------------------------------------------------------------
  // Binding point
  //---------------------------------------------------------------------------------------------------
  BindingPoint::BindingPoint()
  {
  
  }

  Ref<BindingPoint> BindingPoint::Create(StorageBlockType a_type, ShaderDomain a_domain)
  {
    BindingPoint * pBP = new BindingPoint();
    Ref<BindingPoint> ref(pBP);
    pBP->Init(a_type, a_domain);
    return ref;
  }

  void BindingPoint::Init(StorageBlockType a_type, ShaderDomain a_domain)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BindingPointCreate);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID(), sbtype = a_type, domain = a_domain]()
    {
      ::Engine::RT_BindingPoint bp;
      if (!bp.Capture(sbtype, domain))
        LOG_WARN("BindingPoint::Init(): Failed to capture binding index!");
      ::Engine::RenderThreadData::Instance()->bindingPoints.insert(resID, bp);
    });
  }

  BindingPoint::~BindingPoint()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BindingPointDelete);

    RENDER_SUBMIT(state, [resID = GetRefID().GetID()]()
    {
      ::Engine::RT_BindingPoint * pbp = ::Engine::RenderThreadData::Instance()->bindingPoints.at(resID);
      if (pbp == nullptr)
      {
        LOG_WARN("BindingPoint::~BindingPoint(): RefID '{}' does not exist!", resID);
        return;
      }

      pbp->Release();
      ::Engine::RenderThreadData::Instance()->bindingPoints.erase(resID);
    });
  }

  //--------------------------------------------------------------------------------------------------
  // Parsing helper functions
  //--------------------------------------------------------------------------------------------------

  struct varDecl
  {
    std::string type;
    std::string name;
    bool        isArray;
    uint32_t    count;
  };

  typedef Dg::DynamicArray<varDecl> varDeclList;

  static varDeclList __FindDecls(std::string const& a_str, char const* a_regex)
  {
    varDeclList result;
    std::string subject = a_str;

    std::smatch match;
    std::regex r(a_regex);
    while (regex_search(subject, match, r))
    {
      uint32_t count(1);
      bool isArray = false;
      if (match.str(3) != "")
      {
        BSR_ASSERT(Dg::StringToNumber<uint32_t>(count, match.str(3), std::dec), "");
        isArray = true;
      }
      result.push_back(varDecl{match.str(1), match.str(2), isArray, count});
      subject = match.suffix().str();
    }
    return result;
  }

  static varDeclList FindDecls(std::string const& a_str)
  {
    return __FindDecls(a_str, VAR_EXPRESSION);
  }

  static varDeclList FindUniformDecls(std::string const& a_str)
  {
    return __FindDecls(a_str, UNIFORM_VAR_EXPRESSION);
  }

  static bool IsTypeStringTexture(const std::string& type)
  {
    if (type == "sampler2D")		return true;
    if (type == "samplerCube")		return true;
    if (type == "sampler2DShadow")	return true;
    return false;
  }

  //---------------------------------------------------------------------------------------------------
  // ShaderData
  //---------------------------------------------------------------------------------------------------
  
  Ref<ShaderData> ShaderData::Create(std::initializer_list<ShaderSourceElement> const& a_src)
  {
    return Ref<ShaderData>(new ShaderData(a_src));
  }
  
  ShaderData::ShaderData()
    : m_dataSize(0)
  {

  }

  ShaderData::ShaderData(std::initializer_list<ShaderSourceElement> const& a_data)
    : m_dataSize(0)
  {
    Init(a_data);
  }

  void ShaderData::Init(std::initializer_list<ShaderSourceElement> const& a_data)
  {
    Clear();
    m_source.Init(a_data);
    Parse();
    PostProcess();
    Log();
  }

  void ShaderData::Log()
  {
    for (auto& un : m_uniforms)
      un.Log();
  }

  void ShaderData::Clear()
  {
    m_uniforms.clear();
    //m_textures.clear();
  }

  void ShaderData::Parse()
  {
    for (int i = 0; i < ShaderDomain_COUNT; i++)
    {
      ShaderStructList structList;
      ExtractStructs(ShaderDomain(i), structList);
      ExtractUniforms(ShaderDomain(i), structList);
      //ExtractUniformBlocks(ShaderDomain(i));
    }
  }

  void ShaderData::PostProcess()
  {
    uint32_t offset = 0;
    for (auto & uniform : m_uniforms)
    {
      uniform.SetDataOffset(offset);
      offset += uniform.GetDataSize();
    }
    m_dataSize = offset;
  }

  void ShaderData::ExtractStructs(ShaderDomain a_domain, ShaderStructList & a_out)
  {
    std::string subject = m_source.Get(a_domain);

    std::smatch match;
    std::regex r(STRUCT_EXPRESSION);
    while (regex_search(subject, match, r))
    {
      ShaderStruct newStruct(match.str(1));
      varDeclList vars = FindDecls(match.str(2));

      for (auto const& var : vars)
      {
        ShaderDataType dataType = StringToShaderDataType(var.type);
        if (dataType == ShaderDataType::NONE) //might be a previously defined struct
        {
          size_t structIndex = FindStruct(var.type, a_out);
          if (structIndex != INVALID_INDEX)
          {
            ShaderStruct * pStruct = &a_out.data[structIndex];
            for (size_t i = 0; i < pStruct->GetFields().size(); i++)
            {
              ShaderUniformDeclaration const * pUniform = &pStruct->GetFields()[i];
              std::string name = var.name + "." + pUniform->GetName();
              newStruct.AddField(ShaderUniformDeclaration(pUniform->GetType(), name, var.isArray, var.count));
            }
          }
          else
          {
            LOG_WARN("Unrecognised field '{}' in struct '{}' while parsing glsl struct.", var.type.c_str(), match.str(1).c_str());
            continue;
          }
        }
        else
          newStruct.AddField(ShaderUniformDeclaration(dataType, var.name, var.isArray, var.count));
      }
      a_out.data.push_back(newStruct);
      subject = match.suffix().str();
    }
  }

  void ShaderData::ExtractUniforms(ShaderDomain a_domain, ShaderStructList const & a_structs)
  {
    std::string subject = m_source.Get(a_domain);
    varDeclList vars = FindUniformDecls(subject);

    for (auto const& var : vars)
    {
      ShaderDataType t = StringToShaderDataType(var.type);

      if (t == ShaderDataType::NONE)
      {
        size_t structIndex = FindStruct(var.type, a_structs);
        BSR_ASSERT(structIndex != INVALID_INDEX, "Undefined struct!");
        ShaderStruct const * pStruct = &a_structs.data[structIndex];
        for (size_t i = 0; i < pStruct->GetFields().size(); i++)
        {
          ShaderUniformDeclaration const* pUniform = &pStruct->GetFields()[i];
          std::string name = var.name + "." + pUniform->GetName();
          ShaderUniformDeclaration decl(pUniform->GetType(), name, pUniform->IsArray(), pUniform->GetCount());
          decl.GetDomains().AddDomain(a_domain);
          PushUniform(decl);
        }
      }
      else
      {
        ShaderUniformDeclaration decl(t, var.name, var.isArray, var.count);
        decl.GetDomains().AddDomain(a_domain);
        PushUniform(decl);
      }
    }
  }

  size_t ShaderData::FindStruct(std::string const& a_name, ShaderStructList const& a_structs)
  {
    for (size_t i = 0; i < a_structs.data.size(); i++)
    {
      if (a_name == a_structs.data[i].GetName())
        return i;
    }
    return INVALID_INDEX;
  }

  void ShaderData::PushUniform(ShaderUniformDeclaration a_decl)
  {
    for (ShaderUniformDeclaration& decl: m_uniforms)
    {
      if (a_decl == decl)
      {
        decl.GetDomains().AddDomains(a_decl.GetDomains());
        return;
      }
    }
    m_uniforms.push_back(a_decl);
  }

  ShaderUniformDeclaration* ShaderData::FindUniform(std::string const& a_name)
  {
    for (ShaderUniformDeclaration& uniform : m_uniforms)
    {
      if (uniform.GetName() == a_name)
        return &uniform;
    }
    return nullptr;
  }

  uint32_t ShaderData::FindUniformIndex(std::string const& a_name)
  {
    for (uint32_t i = 0; i < uint32_t(m_uniforms.size()); i++)
    {
      if (m_uniforms[i].GetName() == a_name)
        return i;
    }
    return INVALID_INDEX;
  }

  uint32_t ShaderData::GetUniformDataSize() const
  {
    return m_dataSize;
  }

  ShaderSource const& ShaderData::GetShaderSource() const
  {
    return m_source;
  }

  ShaderUniformList const& ShaderData::GetUniforms() const
  {
    return m_uniforms;
  }

  ShaderUniformList& ShaderData::GetUniforms()
  {
    return m_uniforms;
  }
}