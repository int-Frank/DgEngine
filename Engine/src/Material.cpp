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


#include "Material.h"
#include "Message.h"
#include "Serialize.h"

namespace Engine
{
  //-----------------------------------------------------------------------------------------------
  // MaterialData
  //-----------------------------------------------------------------------------------------------
  namespace impl
  {
    MaterialData::MaterialData(Ref<RendererProgram> a_prog)
      : m_prog(a_prog)
    {

    }

    MaterialData::~MaterialData()
    {

    }

    Ref<MaterialData> MaterialData::Create(Ref<RendererProgram> a_prog)
    {
      return Ref<MaterialData>(new MaterialData(a_prog));
    }

    //-----------------------------------------------------------------------------------------------
    // MaterialBase
    //-----------------------------------------------------------------------------------------------
    MaterialBase::MaterialBase(Ref<impl::MaterialData> a_materialData)
      : m_pBuf(nullptr)
      , m_bufSize(0)
      , m_materialData(a_materialData)
    {
      BSR_ASSERT(!m_materialData.IsNull());
      BSR_ASSERT(!m_materialData->m_prog.IsNull());

      m_bufSize = m_materialData->m_prog->UniformBufferSize();
      m_pBuf = new byte[m_bufSize]{};
    }

    MaterialBase::~MaterialBase()
    {
       delete[] m_pBuf;
    }

    void MaterialBase::Bind()
    {
      BSR_ASSERT(!m_materialData.IsNull());
      BSR_ASSERT(!m_materialData->m_prog.IsNull());

      m_materialData->m_prog->Bind();
      m_materialData->m_prog->UploadUniformBuffer(m_pBuf);
    }

    ShaderUniformDeclaration const* MaterialBase::FindUniform(std::string const& a_name)
    {
      BSR_ASSERT(!m_materialData.IsNull());
      BSR_ASSERT(!m_materialData->m_prog.IsNull());

      ShaderUniformDeclaration const* pdecl = m_materialData->m_prog->FindUniformDeclaration(a_name);
      BSR_ASSERT(pdecl);
      
      return pdecl;
    }

    UniformBufferElementHeader MaterialBase::CreateHeader(ShaderUniformDeclaration const * a_pdecl, uint32_t a_size)
    {
      uint32_t count = a_size / SizeOfShaderDataType(a_pdecl->GetType());
      BSR_ASSERT(a_pdecl->GetCount() >= count);
      UniformBufferElementHeader header;
      header.SetSize(a_size);
      return header;
    }

    void MaterialBase::WriteToBuffer(uint32_t a_offset, UniformBufferElementHeader a_header, void const * a_pBuf)
    {
      void* buf = (void*)(m_pBuf + a_offset);
      buf = a_header.Serialize(buf);
      memcpy(buf, a_pBuf, a_header.GetSize());
    }
  }

  //-----------------------------------------------------------------------------------------------
  // Material
  //-----------------------------------------------------------------------------------------------

  Material::Material(Ref<impl::MaterialData> a_data)
    : impl::MaterialBase(a_data)
    , m_renderFlags(0)
  {

  }

  Material::~Material()
  {

  }

  Ref<Material> Material::Create(Ref<RendererProgram> a_prog)
  {
    BSR_ASSERT(!a_prog.IsNull());
    return Ref<Material>(new Material(impl::MaterialData::Create(a_prog)));
  }

  Ref<MaterialInstance> Material::SpawnInstance()
  {
    BSR_ASSERT(!m_materialData.IsNull());
    BSR_ASSERT(!m_materialData->m_prog.IsNull());

    Ref<MaterialInstance> inst(new MaterialInstance(m_materialData));
    inst->InitBuffer(m_pBuf);
    m_materialInstances.push_back(inst);
    return inst;
  }

  void Material::SetUniform(std::string const & a_name, void const * a_pBuf, uint32_t a_size)
  {
    ShaderUniformDeclaration const * pdecl = FindUniform(a_name);
    UniformBufferElementHeader header = CreateHeader(pdecl, a_size);

    uint32_t offset = pdecl->GetDataOffset();
    WriteToBuffer(offset, header, a_pBuf);

    for (auto pInst : m_materialInstances)
      pInst->SetUniform(offset, a_pBuf, a_size);
  }

  void Material::SetTexture(std::string const & a_name, Ref<Texture2D> const & a_texture)
  {
    ShaderUniformDeclaration const * pdecl = FindUniform(a_name);
    UniformBufferElementHeader header = CreateHeader(pdecl, sizeof(RefID));

    uint32_t offset = pdecl->GetDataOffset();
    RefID id = a_texture->GetRefID().GetID();
    WriteToBuffer(offset, header, &id);

    for (auto pInst : m_materialInstances)
      pInst->SetUniform(offset, &id, sizeof(RefID));
  }

  //-----------------------------------------------------------------------------------------------
  // MaterialInstance
  //-----------------------------------------------------------------------------------------------

  MaterialInstance::MaterialInstance(Ref<impl::MaterialData> a_materialData)
    : impl::MaterialBase(a_materialData)
  {

  }

  void MaterialInstance::InitBuffer(byte const* a_pBuf)
  {
    memcpy(m_pBuf, a_pBuf, m_bufSize);
  }

  MaterialInstance::~MaterialInstance()
  {

  }

  void MaterialInstance::SetUniform(std::string const & a_name, void const * a_pBuf, uint32_t a_size)
  {
    ShaderUniformDeclaration const * pdecl = FindUniform(a_name);
    UniformBufferElementHeader header = CreateHeader(pdecl, a_size);
    header.SetFlag(UniformBufferElementHeader::ElementLocked, true);

    uint32_t offset = pdecl->GetDataOffset();
    WriteToBuffer(offset, header, a_pBuf);
  }

  void MaterialInstance::SetTexture(std::string const & a_name, Ref<Texture2D> const & a_texture)
  {
    ShaderUniformDeclaration const * pdecl = FindUniform(a_name);
    UniformBufferElementHeader header = CreateHeader(pdecl, sizeof(RefID));
    header.SetFlag(UniformBufferElementHeader::ElementLocked, true);

    uint32_t offset = pdecl->GetDataOffset();
    RefID id = a_texture->GetRefID().GetID();
    WriteToBuffer(offset, header, &id);
  }

  void MaterialInstance::SetUniform(uint32_t a_offset, void const* a_pBuf, uint32_t a_size)
  {
    UniformBufferElementHeader header;
    header.Deserialize(a_pBuf);

    if (header.Is(UniformBufferElementHeader::ElementLocked))
      return;

    header.SetSize(a_size);
    WriteToBuffer(a_offset, header, a_pBuf);
  }
}