//@group Renderer

#include <fstream>
#include <sstream>

#include "ShaderSource.h"
#include "Serialize.h"
#include "core_Log.h"

namespace Engine
{
  //------------------------------------------------------------------------------------------------
  // Helpful functions
  //------------------------------------------------------------------------------------------------
  static std::string RemoveComments(std::string const& a_src)
  {
    std::stringstream ss(a_src);
    std::string result;
    char c;
    while (ss.get(c))
    {
      if (c == '/')
      {
        if (ss.get(c))
        {
          if (c == '/')
            ss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          else if (c == '*')
          {
            while (ss.get(c))
            {
              if (c == '*')
              {
                if (ss.get(c))
                {
                  if (c == '/')
                    break;
                }
              }
            }
          }
          else
          {
            result += '/';
            result += c;
          }
        }
        else
        {
          result += c;
          break;
        }
      }
      else
        result += c;
    }
    return result;
  }

  //------------------------------------------------------------------------------------------------
  // ShaderSourceElement
  //------------------------------------------------------------------------------------------------
  ShaderSourceElement::ShaderSourceElement(ShaderDomain a_domain, StrType a_type, std::string const& a_str)
    : domain(a_domain)
    , strType(a_type)
    , str(a_str)
  {

  }

  ShaderSourceElement::ShaderSourceElement()
    : domain(ShaderDomain::Vertex)
    , strType(StrType::Source)
  {

  }

  //------------------------------------------------------------------------------------------------
  // ShaderSource
  //------------------------------------------------------------------------------------------------

  ShaderSource::ShaderSource()
  {
  
  }

  void ShaderSource::Init(std::initializer_list<ShaderSourceElement> const& a_list)
  {
    Clear();

    for (auto const& ele : a_list)
    {
      if (ele.strType == StrType::Source)
        m_src[static_cast<uint32_t>(ele.domain)] = RemoveComments(ele.str);
      else
      {
        std::ifstream ifs(ele.str);
        if (!ifs.good())
        {
          LOG_WARN("ShaderSource::ShaderSource() failed to open file '{}'", ele.str.c_str());
          continue;
        }

        std::string content((std::istreambuf_iterator<char>(ifs)),
          (std::istreambuf_iterator<char>()));
        m_src[static_cast<uint32_t>(ele.domain)] = RemoveComments(content);
      }
    }
  }

  ShaderSource::ShaderSource(std::initializer_list<ShaderSourceElement> const& a_list)
  {
    Init(a_list);
  }

  std::string const& ShaderSource::Get(ShaderDomain a_domain) const
  {
    return m_src[static_cast<uint32_t>(a_domain)];
  }

  void ShaderSource::Clear()
  {
    for (uint32_t i = 0; i < ShaderDomain_COUNT; i++)
      m_src[i].clear();
  }
}