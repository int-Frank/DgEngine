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
#include "ResourceManager.h"

#include "Log.h"
#include "DgMath.h"
#include "Utils.h"
#include "Serialize.h"
#include "Options.h"
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

namespace DgE
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
    return ::DgE::Serialize(a_buf, &m_data);
  }

  void const * UniformBufferElementHeader::Deserialize(void const * a_buf)
  {
    return ::DgE::Deserialize(a_buf, &m_data);
  }

  void UniformBufferElementHeader::SetSize(uint32_t a_count)
  {
    DG_ASSERT(a_count <= uint32_t(Dg::Mask<uint32_t,
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
    DG_ASSERT(a_type != ShaderDataType::STRUCT);
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

  Ref<BindingPoint> BindingPoint::Create(StorageBlockType a_type, ShaderDomain a_domain)
  {
    BindingPoint * pBP = new BindingPoint(a_type, a_domain);
    return Ref<BindingPoint>(pBP);
  }

  BindingPoint::BindingPoint(StorageBlockType a_type, ShaderDomain a_domain)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BindingPointCreate);

    RENDER_SUBMIT(state, [resID = m_id, sbtype = a_type, domain = a_domain]()
    {
      RT_BindingPoint * pBP = new RT_BindingPoint;
      if (!pBP->Capture(sbtype, domain))
        LOG_WARN("BindingPoint::Capture(): Failed to capture binding index!");
      else
        RenderThreadData::Instance()->bindingPoints.insert(resID, pBP);
    });
  }

  BindingPoint::~BindingPoint()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::BindingPointDelete);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      RT_BindingPoint ** ppBP = RenderThreadData::Instance()->bindingPoints.at(resID);
      if (ppBP == nullptr)
      {
        LOG_WARN("BindingPoint::~BindingPoint(): RefID '{}' does not exist!", resID);
        return;
      }
    
      delete *ppBP;
      *ppBP = nullptr;
      RenderThreadData::Instance()->bindingPoints.erase(resID);
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
        DG_ASSERT(Dg::StringToNumber<uint32_t>(count, match.str(3), std::dec), "");
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

  Ref<ShaderData> ShaderData::Create(std::initializer_list<ShaderSourceElement> const & a_src)
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
        DG_ASSERT(structIndex != INVALID_INDEX, "Undefined struct!");
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

  uint32_t ShaderData::FindUniformIndex(std::string const& a_name) const
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