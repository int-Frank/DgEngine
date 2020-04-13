
#include "DgFileStream.h"
#include "DgDynamicArray.h"
#include "DgBinaryReader.h"
#include "gc_Map_BS.h"
#include "core_Log.h"

namespace GC
{
  bool Map_BS::Init(std::string const & a_path)
  {
    std::string mapHeaderPath = a_path + "/" + MAPHEADER;
    s_mapTempPath = a_path + "/" + MAPTEMP;

    Dg::FileStream fs(s_mapTempPath, Dg::StreamOpenMode::read);
    if (!fs.IsOpen())
    {
      LOG_ERROR((std::string("Failed to open ") + s_mapTempPath).c_str());
      return false;
    }

    fs.Close();
    fs.Open(mapHeaderPath, Dg::StreamOpenMode::read);

    if (!fs.IsOpen())
    {
      LOG_ERROR((std::string("Failed to open ") + mapHeaderPath).c_str());
      return false;
    }

    Dg::EndianConverter ec;
    ec.SetConversion(GC::BS_FileEndian, Dg::Endianness());

    Dg::BinaryReader reader(&fs, ec);
    
    Dg::IO::ReturnType result = reader.Read<uint16_t>(&(s_header.RLEWtag), 1);

    if (result.error != Dg::ErrorCode::None || result.value != 1)
    {
      LOG_ERROR("Failed to read RLEWtag");
      return false;
    }

    result = reader.Read<uint32_t>(s_header.headeroffsets, MAX_MAPS);

    if (result.error != Dg::ErrorCode::None || result.value != MAX_MAPS)
    {
      LOG_ERROR("Failed to read map offsets");
      return false;
    }
    return true;
  }

  bool Map_BS::LoadMap(int a_index)
  {
    Clean();

    struct GameMapHeader
    {
      uint32_t   planeOffsets[3];
      uint16_t   planeSizes[3];
      uint16_t   mapWidth;
      uint16_t   mapHeight;
      char       mapName[16];
    };

    char * plane_0_data = nullptr;
    char * plane_1_data = nullptr;
    bool isGood = true;

    do
    {
      Dg::FileStream fs(s_mapTempPath, Dg::StreamOpenMode::read);
      if (!fs.IsOpen())
      {
        LOG_ERROR((std::string("Failed to open ") + s_mapTempPath).c_str());
        isGood = false; 
        break;
      }

      Dg::EndianConverter ec;
      ec.SetConversion(GC::BS_FileEndian, Dg::Endianness());
      Dg::BinaryReader reader(&fs, ec);

      Dg::IO::ReturnType result = reader.SetPosition(s_header.headeroffsets[a_index]);
      if (result.error != Dg::ErrorCode::None)
      {
        LOG_ERROR("Failed to seek the map file: {}", Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      GameMapHeader gameMapHeader;
      result = reader.Read<uint32_t>(gameMapHeader.planeOffsets, 3);
      if (result.error != Dg::ErrorCode::None || result.value != 3)
      {
        LOG_ERROR("Failed to load plane offsets for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      result = reader.Read<uint16_t>(gameMapHeader.planeSizes, 3);
      if (result.error != Dg::ErrorCode::None || result.value != 3)
      {
        LOG_ERROR("Failed to load plane sizes for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      result = reader.Read<uint16_t>(&gameMapHeader.mapWidth, 1);
      if (result.error != Dg::ErrorCode::None || result.value != 1)
      {
        LOG_ERROR("Failed to load map width for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      result = reader.Read<uint16_t>(&gameMapHeader.mapHeight, 1);
      if (result.error != Dg::ErrorCode::None || result.value != 1)
      {
        LOG_ERROR("Failed to load map height for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      result = reader.Read<char>(name, 16);
      if (result.error != Dg::ErrorCode::None || result.value != 16)
      {
        LOG_ERROR("Failed to load map name for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      plane_0_data = new char[gameMapHeader.planeSizes[0]]();
      plane_1_data = new char[gameMapHeader.planeSizes[1]]();

      result = reader.SetPosition(gameMapHeader.planeOffsets[0]);
      if (result.error != Dg::ErrorCode::None)
      {
        LOG_ERROR("Failed to seek the map file for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      result = reader.Read<char>(plane_0_data, gameMapHeader.planeSizes[0]);
      if (result.error != Dg::ErrorCode::None || result.value != 16)
      {
        LOG_ERROR("Failed to load plane 0 data for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      result = reader.SetPosition(gameMapHeader.planeOffsets[1]);
      if (result.error != Dg::ErrorCode::None)
      {
        LOG_ERROR("Failed to seek the map file for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      result = reader.Read<char>(plane_1_data, gameMapHeader.planeSizes[1]);
      if (result.error != Dg::ErrorCode::None || result.value != 16)
      {
        LOG_ERROR("Failed to load plane 1 data for map {}: {}", a_index, Dg::ErrorCodeToString(result.error));
        isGood = false; 
        break;
      }

      bool isGood = UncompressPlane(0, plane_0_data, gameMapHeader.planeSizes[0], gameMapHeader.mapWidth, gameMapHeader.mapHeight);
      isGood = (isGood && UncompressPlane(1, plane_1_data, gameMapHeader.planeSizes[1], gameMapHeader.mapWidth, gameMapHeader.mapHeight));

      if (!isGood)
        LOG_ERROR("Failed to uncompress planes for map {}", a_index);
    } while (false);
     

    delete[] plane_0_data;
    delete[] plane_1_data;

    if (!isGood)
      Clean();

    return isGood;
  }

  void Map_BS::Clean()
  {
    memset(name, 0, 16);
    planes[0] = Dg::HyperArray<uint16_t, 2>();
    planes[1] = Dg::HyperArray<uint16_t, 2>();
  }

  bool Map_BS::UncompressPlane(int a_plane,
                               char const * a_data,
                               uint32_t a_dataSize,
                               uint16_t a_width,
                               uint16_t a_height)
  {
    Dg::DynamicArray<uint16_t> raw;

    uint16_t pos = 0;
    char const * pData = a_data;

#define READ16(pData) *((uint16_t const*)pData); pData += 2; pos += 2;

    //There seems to be an extra word at the start of each map, the same value each time (0x2000).
    //Not sure what this is, so discard.
    //TODO find out what this value is.
    uint16_t word = READ16(pData);

    while (pos < a_dataSize)
    {
      uint16_t word = READ16(pData);

      if (word == s_header.RLEWtag)
      {
        uint16_t count = READ16(pData);
        uint16_t value = READ16(pData);

        for (uint16_t i = 0; i < count; i++)
          raw.push_back(value);
      }
      else
        raw.push_back(word);
    }
    
    if (raw.size() != (a_height * a_width))
    {
      LOG_ERROR("Plane '{}' has not uncpmpressed to the correct size: '{}' vs '{}' (correct)", a_plane, raw.size(), (a_height * a_width));
      return false;
    }

    planes[a_plane].Set({size_t(a_height), size_t(a_width)});
    for (size_t y = 0; y < a_width; y++)
    {
      for (size_t x = 0; x < a_height; x++)
        planes[a_plane](x, y) = raw[(y * a_height) + x];
    }

    return true;
  }
}
