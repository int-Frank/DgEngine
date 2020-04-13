#ifndef GC_MAP_BS
#define GC_MAP_BS

#include <string>
#include <stdint.h>

#include "gc_Constants.h"
#include "DgHyperArray.h"

namespace GC
{
  class Map_BS
  {
  public:

    static bool Init(std::string const & a_bsPath);
    bool LoadMap(int);

    char name[16];
    Dg::HyperArray<uint16_t, 2> planes[2];

  private:

    void Clean();
    bool UncompressPlane(int plane,
                         char const * data,
                         uint32_t dataSize,
                         uint16_t width,
                         uint16_t height);

  private:

    struct MapFileType
    {
      uint16_t	RLEWtag;
      uint32_t	headeroffsets[MAX_MAPS];
    };

    static MapFileType s_header;
    static std::string s_mapTempPath;
  };
}

#endif