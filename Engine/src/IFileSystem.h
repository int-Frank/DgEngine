//@group Interface

#ifndef IFILESYSTEM_H
#define IFILESYSTEM_H

#include <string>

namespace Engine
{
  class IFileSystem
  {
  public:

    virtual ~IFileSystem() { }

    virtual std::string GetAbsolutePath(std::string const & path) = 0;
  };
}

#endif