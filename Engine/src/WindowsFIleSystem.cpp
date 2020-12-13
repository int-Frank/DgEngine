#include <filesystem>

#include "IFileSystem.h"
#include "Framework.h"

namespace Engine
{
  class WindowsFileSystem : public IFileSystem
  {
  public:

    ~WindowsFileSystem();
    std::string GetAbsolutePath(std::string const & path) override;

  };

  Dg::ErrorCode Framework::InitFileSystem()
  {
    SetFileSystem(new WindowsFileSystem());
    return Dg::ErrorCode::None;
  }

  WindowsFileSystem::~WindowsFileSystem()
  {
  
  }

  std::string WindowsFileSystem::GetAbsolutePath(std::string const & a_path)
  {
    return std::filesystem::canonical(a_path).string();
  }
}