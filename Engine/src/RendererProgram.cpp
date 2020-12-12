//@group Renderer

#include "RendererProgram.h"
#include "RT_RendererProgram.h"
#include "Renderer.h"
#include  "Log.h"
#include "Serialize.h"
#include "RenderThreadData.h"
#include "ResourceManager.h"

namespace Engine
{
  void RendererProgram::Init(ResourceID a_shaderSourceID)
  {
    m_shaderDataID = a_shaderSourceID;

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramCreate);

    RENDER_SUBMIT(state, [resID = m_id, shaderDataID = m_shaderDataID]()
    {
      if (RenderThreadData::Instance()->rendererPrograms.at(resID) != nullptr)
      {
        LOG_WARN("RendererProgram::RendererProgram: RefID '{}' already exists!", resID);
        return;
      }
      RenderThreadData::Instance()->rendererPrograms.insert(resID, RT_RendererProgram::Create(shaderDataID));
    });
  }

  RendererProgram::RendererProgram()
  {
  
  }

  Ref<RendererProgram> RendererProgram::Create(ResourceID a_shaderSourceID)
  {
    RendererProgram* pRP = new RendererProgram();
    Ref<RendererProgram> ref(pRP);
    pRP->Init(a_shaderSourceID);
    return ref;
  }

  RendererProgram::~RendererProgram()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramDelete);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      RT_RendererProgram ** ppRP = RenderThreadData::Instance()->rendererPrograms.at(resID);
      if (ppRP == nullptr)
      {
        LOG_WARN("RendererProgram::~RendererProgram: RefID '{}' does not exist!", resID);
        return;
      }
      delete (*ppRP);
      *ppRP = nullptr;
      ::Engine::RenderThreadData::Instance()->rendererPrograms.erase(resID);
    });
  }

  uint32_t RendererProgram::UniformBufferSize() const
  {
    ShaderData * ptr = ResourceManager::Instance()->GetResource<ShaderData>(m_shaderDataID);
    BSR_ASSERT(ptr != nullptr);

    return ptr->GetUniformDataSize();
  }

  /*void RendererProgram::Init(ShaderSource const & a_src)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramInit);
    ShaderSource * ptr = new ShaderSource(a_src);

    RENDER_SUBMIT(state, [resID = m_id, ptr = ptr]()
    {
      RT_RendererProgram* pRP = RenderThreadData::Instance()->rendererPrograms.at(resID);
      if (pRP == nullptr)
      {
        LOG_WARN("RendererProgram::Init: RefID '{}' does not exist!", resID);
        return;
      }
      if (!pRP->Init(*ptr))
        LOG_WARN("RendererProgram::Init: Failed! RefID: {}", resID);
      delete ptr;
    });
  }*/

  void RendererProgram::Bind()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramBind);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      RT_RendererProgram ** ppRP = RenderThreadData::Instance()->rendererPrograms.at(resID);
      if (ppRP == nullptr)
      {
        LOG_WARN("RendererProgram::Bind: RefID '{}' does not exist!", resID);
        return;
      }
      (*ppRP)->Bind();
    });
  }

  void RendererProgram::Unbind()
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramUnbind);

    RENDER_SUBMIT(state, [resID = m_id]()
    {
      RT_RendererProgram ** ppRP = RenderThreadData::Instance()->rendererPrograms.at(resID);
      if (ppRP == nullptr)
      {
        LOG_WARN("RendererProgram::Unbind: RefID '{}' does not exist!", resID);
        return;
      }
      (*ppRP)->Unbind();
    });
  }

  void RendererProgram::UploadUniformBuffer(byte const * a_buf)
  {
    //TODO Store pointer to avoid look-up each call to UploadUniformBuffer()
    ShaderData * pShaderData = ResourceManager::Instance()->GetResource<ShaderData>(m_shaderDataID);
    BSR_ASSERT(pShaderData != nullptr);

    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramUploadUniform);

    byte * buf_data = (byte*)RENDER_ALLOCATE(pShaderData->GetUniformDataSize());
    memcpy(buf_data, a_buf, pShaderData->GetUniformDataSize());

    RENDER_SUBMIT(state, [resID = m_id, buf = buf_data]()
    {
      RT_RendererProgram ** ppRP = RenderThreadData::Instance()->rendererPrograms.at(resID);
      if (ppRP == nullptr)
      {
        LOG_WARN("RendererProgram::UploadUniformBuffer: RefID '{}' does not exist!", resID);
        return;
      }
      (*ppRP)->UploadUniformBuffer(buf);
    });
  }

  ShaderUniformDeclaration const* RendererProgram::FindUniformDeclaration(std::string const& a_name) const
  {
    ShaderData * pShaderData = ResourceManager::Instance()->GetResource<ShaderData>(m_shaderDataID);
    BSR_ASSERT(pShaderData != nullptr);

    for (size_t i = 0; i < pShaderData->GetUniforms().size(); i++)
    {
      ShaderUniformDeclaration const * pdecl = &pShaderData->GetUniforms()[i];
      if (pdecl->GetName() == a_name)
        return pdecl;
    }
    return nullptr;
  }

  void RendererProgram::UploadUniform(std::string const& a_name, void const* a_buf, uint32_t a_size)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramUploadUniform);

    uint32_t sze_name = SerializedSize(a_name);
    void* buf_name = RENDER_ALLOCATE(sze_name);
    Serialize(buf_name, &a_name);

    void* buf_data = RENDER_ALLOCATE(a_size);
    memcpy(buf_data, a_buf, a_size);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, buf_name = buf_name, buf_data = buf_data]()
    {
      RT_RendererProgram ** ppRP = RenderThreadData::Instance()->rendererPrograms.at(resID);
      if (ppRP == nullptr)
      {
        LOG_WARN("RendererProgram::Bind: RefID '{}' does not exist!", resID);
        return;
      }
      std::string name;

      Deserialize(buf_name, &name, 1);
      (*ppRP)->UploadUniform(name, buf_data, size);
    });
  }

  /*void RendererProgram::UploadUniformNoCopy(std::string const& a_name, void const* a_buf, uint32_t a_size)
  {
    RenderState state = RenderState::Create();
    state.Set<RenderState::Attr::Type>(RenderState::Type::Command);
    state.Set<RenderState::Attr::Command>(RenderState::Command::RendererProgramUploadUniform);

    uint32_t sze_name = SerializedSize(a_name);
    void* buf_name = RENDER_ALLOCATE(sze_name);
    Serialize(buf_name, &a_name);

    RENDER_SUBMIT(state, [resID = m_id, size = a_size, buf_name = buf_name, buf_data = a_buf]()
    {
      RT_RendererProgram* pRP = RenderThreadData::Instance()->rendererPrograms.at(resID);
      if (pRP == nullptr)
      {
        LOG_WARN("RendererProgram::Bind: RefID '{}' does not exist!", resID);
        return;
      }
      std::string name;

      Deserialize(buf_name, &name, 1);
      pRP->UploadUniform(name, buf_data, size);
    });
  }*/
}