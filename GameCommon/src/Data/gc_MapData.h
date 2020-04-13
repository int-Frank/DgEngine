#ifndef GC_MAPDATA_H
#define GC_MAPDATA_H

#include <string>
#include "DgDynamicArray.h"
#include "DgVariableArray2D.h"
#include "DgHyperArray.h"
#include "../gc_Types.h"
#include "../gc_Constants.h"
#include "core_Assert.h"
#include "DgBit.h"

#define BSR_VERSION 0x10000000

namespace GC
{

  //---------------------------------------------------------------------------------------
  // Vanilla map...
  //---------------------------------------------------------------------------------------



  //---------------------------------------------------------------------------------------
  // BSR formats...
  //---------------------------------------------------------------------------------------

  struct WallTile
  {
    vec3 position;
    vec3 direction;
  };

  class Wall
  {
  public:

    Wall();

    vec2 GetNormal() const;

    vec3 direction;
    vec3 origin;
    float length;
  };

  struct Arc
  {
    vec3 loa; 
    vec3 roa;
  };

  struct Corner
  {
    enum ArcQuadrant
    {
      I,
      II,
      III,
      IV
    };

    static Arc const Arcs[4];

    uint32_t  arcQuadrant;
    vec3      point;
  };

  class BSP_Node
  {
    typedef uint32_t DataType;

    struct Meta
    {
      uint32_t loc;
      uint32_t size;
    };

    static Meta const s_meta[6];

  public:

    BSP_Node();
    BSP_Node(uint64_t);

    enum Element : uint32_t
    {
      x = 0,
      y = 1
    };

    enum Type : uint32_t
    {
      Branch = 0,
      Leaf = 1
    };

    enum class Item : DataType
    {
      //(leaf/branch)
      Type,

      //leaf
      BlockIndex,

      //branch
      ChildAboveIndex,
      ChildBelowIndex,
      Offset,
      Element
    };

    template<Item item>
    uint32_t Get() const
    {
      DataType ind = static_cast<DataType>(item);
      return Dg::GetSubInt<DataType, s_meta[ind].loc, s_meta[ind].size>(m_data);
    }

    template<Item item>
    void Set(uint32_t a_val)
    {
      DataType ind = static_cast<DataType>(item);
      m_data = Dg::SetSubInt<DataType, s_meta[ind].loc, s_meta[ind].size>(m_data, a_val);
    }

  protected:
    DataType m_data;
  };

  struct AssociatedGeometry
  {
    static uint32_t const None = 0xFFFF;

    uint32_t wallsIndex   : 16;
    uint32_t cornersIndex : 16;
  };

  class Block
  {
  public:

    uint32_t GetX() const {return m_data & 0xFF;}
    uint32_t GetY() const {return (m_data >> 8) & 0xFF;}
    uint32_t GetW() const {return (m_data >> 16) & 0xFF;}
    uint32_t GetH() const {return (m_data >> 24);}

    void SetX(uint32_t);
    void SetY(uint32_t);
    void SetW(uint32_t);
    void SetH(uint32_t);

  private:
    uint32_t m_data;
  };

  class VisibilityMaskData
  {
  public:


    //uint32_t Push(Block const &, Mask const &);

  private:
    uint64_t * m_pData;
  };

  class VisibilityMask
  {
  public:

  private:
    uint32_t m_offset;
  };

  /*
  To determine visible tiles:
  1) Find the intersecting polygon between ceiling plane and view frustum.
  2) Raster polygon, checking against PVS for the particular tile the player is in.
  3) Repeat for floor plane
  4) Find convex hull of the view frustum corners as projected onto the horizontal plane.
  5) Raster to find visible wall tiles.
  */

  struct MapData
  {
    //Dg::VariableArray2D<VisibleTileBlock>  visibleTileBlocks;
    Dg::HyperArray<uint16_t, 2>            visibleTileMapping;

    Dg::VariableArray2D<uint32_t>          associatedWalls;
    Dg::VariableArray2D<uint32_t>          associatedCorners;
    Dg::HyperArray<AssociatedGeometry, 2>  asociatedGeometryMapping;

    Dg::DynamicArray<Wall>                 Walls;
    Dg::DynamicArray<Corner>               Corners;
  };
}

#endif