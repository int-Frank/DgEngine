//@group Renderer

#include <fstream>
#include <sstream>

#include "ShaderSource.h"
#include "Serialize.h"
#include "Log.h"

namespace DgE
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
  ShaderSourceElement::ShaderSourceElement(ShaderDomain a_domain, std::string const& a_str)
    : domain(a_domain)
    , str(a_str)
  {

  }

  ShaderSourceElement::ShaderSourceElement()
    : domain(ShaderDomain::Vertex)
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
      m_src[static_cast<uint32_t>(ele.domain)] = RemoveComments(ele.str);
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