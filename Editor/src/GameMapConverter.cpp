#include <exception>
#include <fstream>

//DEBUG
#include <iostream>
//#include <map>
//End debug

#include "MapData.h"
#include "GameMapConverter.h"
#include "DgDynamicArray.h"
#include "DgHyperArray.h"
#include "BlockPartition.h"
#include "BSPTreeBuilder.h"
#include "WallAssembler.h"

//All defines should be in a common file
#define MAPHEADER_DIR "../Resources/game_data/MAPHEAD.BS6"
#define MAPTEMP_DIR "../Resources/game_data/MAPTEMP.BS6"

#define MAP_OUTPUT_FILE "../Resources/BSR_data/GAMEMAPS.BSR"

#define AUDIO_OUTPUT_FILE "../Resources/BSR_data/AUDIO.BSR"
#define IMAGE_OUTPUT_FILE "../Resources/BSR_data/IMAGES.BSR"
#define LIGHT_OUTPUT_FILE "../Resources/BSR_data/LIGHTING.BSR"

#define MAX_MAPS 100

#define PUSH_WALL 98
#define CEILINGFLOOR 64256

#define READ16(pData) *((uint16_t const*)pData); pData++; pData++;

class GameMapConverter::PIMPL
{
private:

  struct MapFileType
  {
    uint16_t	                  RLEWtag;
    Dg::DynamicArray<uint32_t>	headeroffsets;
  };

  struct BSRGameMapHeader
  {
    uint32_t const version = BSR_VERSION;
  };

public:

  PIMPL();
  ~PIMPL();

  void SetInputFiles(std::string const & mapHeader,
                     std::string const & gameData);

  void SetOutputFile(std::string const & mapHeader);

  void Convert();

private:

  PIMPL(PIMPL const &);
  PIMPL & operator=(PIMPL const &);

private:

  void BeginFile(BSRGameMapHeader const & );
  void WriteMetaData(MapFileType const &);
  MapFileType GetHeaderData();
  void OutputMap(MapFileData const &, uint32_t);
  MapFileData ConvertMap(uint32_t offset);
  Dg::HyperArray<uint16_t, 2> UncompressPlane(char const *, uint16_t size, uint16_t w, uint16_t h);
  bool IsWall(uint16_t) const;

private:

  std::string m_inputHeaderFile;
  std::string m_inputGameDataFile;
  std::string m_outputFile;
  uint16_t m_RLEWtag;
};

GameMapConverter::GameMapConverter()
  : m_pimpl(new PIMPL)
{

}

GameMapConverter::~GameMapConverter()
{
  delete m_pimpl;
}

void GameMapConverter::SetInputFiles(std::string const & a_header,
                                     std::string const & a_data)
{
  m_pimpl->SetInputFiles(a_header, a_data);
}

void GameMapConverter::SetOutputFile(std::string const & a_outFile)
{
  m_pimpl->SetOutputFile(a_outFile);
}

void GameMapConverter::Convert()
{
  m_pimpl->Convert();
}

GameMapConverter::PIMPL::PIMPL()
  : m_inputHeaderFile(MAPHEADER_DIR)
  , m_inputGameDataFile(MAPTEMP_DIR)
  , m_outputFile(MAP_OUTPUT_FILE)
  , m_RLEWtag(0xABCD)
{

}

GameMapConverter::PIMPL::~PIMPL()
{

}

void GameMapConverter::PIMPL::SetInputFiles(std::string const & a_header,
                                            std::string const & a_data)
{
  m_inputHeaderFile = a_header;
  m_inputGameDataFile = a_data;
}

void GameMapConverter::PIMPL::SetOutputFile(std::string const & a_outFIle)
{
  m_outputFile = a_outFIle;
}

GameMapConverter::PIMPL::MapFileType GameMapConverter::PIMPL::GetHeaderData()
{
  std::ifstream fHeader(m_inputHeaderFile, std::ios::binary);

  if (!fHeader.good())
  {
    throw std::exception((std::string("Failed to open ") + m_inputHeaderFile).c_str());
  }

  MapFileType mapHeaderData;
  long mapOffsets[MAX_MAPS];

  fHeader.read((char*)&mapHeaderData.RLEWtag, sizeof(MapFileType::RLEWtag));
  fHeader.read((char*)&mapOffsets, sizeof(long)*MAX_MAPS);

  for (long i = 0; i < MAX_MAPS; i++)
  {
    if (mapOffsets[i] > 0)
    {
      mapHeaderData.headeroffsets.push_back(mapOffsets[i]);
    }
  }

  return mapHeaderData;
}

void GameMapConverter::PIMPL::OutputMap(MapFileData const &, uint32_t a_index)
{

}

Dg::HyperArray<uint16_t, 2> GameMapConverter::PIMPL::UncompressPlane(char const * a_pData, 
                                                                     uint16_t a_size,
                                                                     uint16_t a_w,
                                                                     uint16_t a_h)
{
  Dg::DynamicArray<uint16_t> raw;

  uint16_t pos = 0;

  //There seems to be an extra word at the start of each map, the same value each time (0x2000).
  //Not sure what this is, so discard.
  //TODO find out what this value is.
  uint16_t word = READ16(a_pData); 
  pos += 2;

  while (pos < a_size)
  {
    uint16_t word = READ16(a_pData); 
    pos += 2;

    if (word == m_RLEWtag)
    {
      uint16_t count = READ16(a_pData);
      pos += 2;
      uint16_t value = READ16(a_pData);
      pos += 2;

      for (uint16_t i = 0; i < count; i++)
      {
        raw.push_back(value);
      }
    }
    else
    {
      raw.push_back(word);
    }
  }

  if (raw.size() != a_w * a_h)
  {
    throw std::exception("Incorrect map size!");
  }

  Dg::HyperArray<uint16_t, 2> result({size_t(a_w), size_t(a_h)});

  for (size_t y = 0; y < a_h; y++)
  {
    for (size_t x = 0; x < a_w; x++)
    {
      result(x, y) = raw[(y * a_w) + x];
    }
  }

  return result;
}

bool GameMapConverter::PIMPL::IsWall(uint16_t a_val) const
{
  return (a_val > 0) && (a_val <= 63);
}

class Counter
{
public:

  Counter(): val(0){}

  uint32_t val;
};

//std::map<uint16_t, std::map<int, Counter>> g_structural;
//std::map<uint16_t, std::map<int, Counter>> g_objects;
//int g_Map = -1;

MapFileData GameMapConverter::PIMPL::ConvertMap(uint32_t a_offset)
{
  //g_Map++;
  MapFileData result;

  //------------------------------------------------------------------------------------------
  // Read the map header
  //------------------------------------------------------------------------------------------
  struct GameMapHeader
  {
    uint32_t   planeoffset_0;
    uint32_t   planeoffset_1;
    uint32_t   planeoffset_2;
    uint16_t   planeSize_0;
    uint16_t   planeSize_1;
    uint16_t   planeSize_2;
    uint16_t   mapLength;
    uint16_t   mapWidth;
    char       mapName[16];
  };

  std::ifstream fMapData(m_inputGameDataFile, std::ios::binary);

  if (!fMapData.good())
  {
    throw std::exception((std::string("Failed to open ") + m_inputGameDataFile).c_str());
  }

  fMapData.seekg(a_offset);

  GameMapHeader gameMapHeader;
  fMapData.read((char*)&gameMapHeader.planeoffset_0, sizeof(GameMapHeader::planeoffset_0));
  fMapData.read((char*)&gameMapHeader.planeoffset_1, sizeof(GameMapHeader::planeoffset_1));
  fMapData.read((char*)&gameMapHeader.planeoffset_2, sizeof(GameMapHeader::planeoffset_2));
  fMapData.read((char*)&gameMapHeader.planeSize_0, sizeof(GameMapHeader::planeSize_0));
  fMapData.read((char*)&gameMapHeader.planeSize_1, sizeof(GameMapHeader::planeSize_1));
  fMapData.read((char*)&gameMapHeader.planeSize_2, sizeof(GameMapHeader::planeSize_2));
  fMapData.read((char*)&gameMapHeader.mapLength, sizeof(GameMapHeader::mapLength));
  fMapData.read((char*)&gameMapHeader.mapWidth, sizeof(GameMapHeader::mapWidth));
  fMapData.read((char*)&gameMapHeader.mapName, sizeof(GameMapHeader::mapName));

  //------------------------------------------------------------------------------------------
  // Read the map data
  //------------------------------------------------------------------------------------------
  char * plane_0_data = new char[gameMapHeader.planeSize_0]();
  char * plane_1_data = new char[gameMapHeader.planeSize_1]();

  fMapData.seekg(gameMapHeader.planeoffset_0);
  fMapData.read(plane_0_data, gameMapHeader.planeSize_0);

  fMapData.seekg(gameMapHeader.planeoffset_1);
  fMapData.read(plane_1_data, gameMapHeader.planeSize_1);

  //------------------------------------------------------------------------------------------
  // Uncompress map data
  //------------------------------------------------------------------------------------------
  Dg::HyperArray<uint16_t, 2> plane_0 = UncompressPlane(plane_0_data, gameMapHeader.planeSize_0, gameMapHeader.mapLength, gameMapHeader.mapWidth);
  Dg::HyperArray<uint16_t, 2> plane_1 = UncompressPlane(plane_1_data, gameMapHeader.planeSize_1, gameMapHeader.mapLength, gameMapHeader.mapWidth);

  /*bool cfFoundTxt = false;
  bool cfFoundClr = false;
  for (uint16_t y = 0; y < gameMapHeader.mapWidth; y++)
  {
    for (uint16_t x = 0; x < gameMapHeader.mapLength; x++)
    {
      uint16_t p0Val = plane_0(size_t(x), size_t(y));
      uint16_t p1Val = plane_1(size_t(x), size_t(y));

      g_structural[p0Val][g_Map].val++;

      if (!cfFoundTxt && !cfFoundClr)
      {
        g_objects[p1Val][g_Map].val++;
      }

      cfFoundTxt = p1Val == CEILINGFLOOR;
      cfFoundClr = p1Val == 65024;
    }
  }*/

  delete[] plane_0_data;
  delete[] plane_1_data;

  //------------------------------------------------------------------------------------------
  // Generate the floor/wall mask
  //------------------------------------------------------------------------------------------
  //Do not include push walls in the creation of blocks. We'll consider these objects.
  Dg::HyperArray<bool, 2> mask({size_t(gameMapHeader.mapLength), size_t(gameMapHeader.mapWidth)});
  for (uint16_t y = 0; y < gameMapHeader.mapWidth; y++)
  {
    for (uint16_t x = 0; x < gameMapHeader.mapLength; x++)
    {
      bool isWall = IsWall(plane_0(size_t(x), size_t(y)));
      bool isPushWall = plane_1(size_t(x), size_t(y)) == PUSH_WALL;
      mask(x, y) = (isWall) && !(isPushWall);
    }
  }

  //------------------------------------------------------------------------------------------
  // Generate block partition
  //------------------------------------------------------------------------------------------
  BlockPartition blockPart(mask);
  result.blocks = blockPart.Run();

  //------------------------------------------------------------------------------------------
  // Generate Wall Geometry
  //------------------------------------------------------------------------------------------
  WallAssembler wallAss(mask);
  wallAss.Run();
  result.wallGeometryLines = wallAss.GetWalls();
  result.wallGeometryPoints = wallAss.GetCorners();

  //------------------------------------------------------------------------------------------
  // Generate BSP Tree
  //------------------------------------------------------------------------------------------
  BSPTreeBuilder bsp(result.blocks);
  bsp.Run();
  result.bspTree = bsp.GetResults();

  //------------------------------------------------------------------------------------------
  // Get ceiling/floor tiles
  //------------------------------------------------------------------------------------------
  for (size_t y = 0; y < plane_1.length(1); y++)
  {
    for (size_t x = 0; x < plane_1.length(0); x++)
    {
      if (plane_1(x, y) == CEILINGFLOOR)
      {
        uint16_t val = plane_1(x + 1, y);

        result.ceilingTile = (val >> 8);
        result.floorTile = val & 0xFF;
        goto end_ceilingfloor;
      }
    }
  }
end_ceilingfloor:

  //------------------------------------------------------------------------------------------
  // Get all structural objects
  //------------------------------------------------------------------------------------------


  return result;
}

void GameMapConverter::PIMPL::BeginFile(BSRGameMapHeader const & a_headerData)
{
  std::ofstream fGameMaps(m_outputFile, std::ios::binary);

  if (!fGameMaps.good())
  {
    throw std::exception((std::string("Failed to open ") + m_outputFile).c_str());
  }

  fGameMaps.write((char*)&a_headerData.version, sizeof(BSRGameMapHeader::version));
}

void GameMapConverter::PIMPL::WriteMetaData(MapFileType const & a_data)
{
  std::ofstream fGameMaps(m_outputFile, std::ios::binary | std::ofstream::app);

  if (!fGameMaps.good())
  {
    throw std::exception((std::string("Failed to open ") + m_outputFile).c_str());
  }

  uint32_t nMaps = uint32_t(a_data.headeroffsets.size());
  fGameMaps.write((char*)&nMaps, sizeof(nMaps));

  size_t bufSze = a_data.headeroffsets.size() * sizeof(uint32_t);
  char * dummyBuf = new char[bufSze]();
  fGameMaps.write(dummyBuf, bufSze);
  delete[] dummyBuf;
}

void GameMapConverter::PIMPL::Convert()
{
  //Start the file and output the header
  BSRGameMapHeader BSRHeader;
  BeginFile(BSRHeader);

  //Write the map meta data
  MapFileType headerData = GetHeaderData();
  WriteMetaData(headerData);

  for (uint32_t i = 0; i < uint32_t(headerData.headeroffsets.size()); i++)
  {
    MapFileData data = ConvertMap(headerData.headeroffsets[i]);
    std::cout << i << " of " << headerData.headeroffsets.size() << " Complete\n";
    //OutputMap(data, i);
  }

  /*std::ofstream fs("CodeCount.txt");
  fs << "Structural:\n";
  for (auto kv: g_structural)
  {
    fs << "\n\n" << kv.first << ": ";
    for (auto kv2 : kv.second)
    {
      fs << "[" << kv2.first << ": " << kv2.second.val << "], ";
    }
  }

  fs << "\n\nObjects:\n\n";
  for (auto kv: g_objects)
  {
    fs << "\n\n" << kv.first << ": ";
    for (auto kv2 : kv.second)
    {
      fs << "[" << kv2.first << ": " << kv2.second.val << "], ";
    }
  }*/
}