#ifndef WALLASSEMBLER_H
#define WALLASSEMBLER_H

#include "Types.h"
#include "MapData.h"
#include "DgDynamicArray.h"
#include "DgHyperArray.h"

class WallAssembler
{
public:

  WallAssembler(Dg::HyperArray<bool, 2> const &);

  void Run();
  Dg::DynamicArray<Wall> const & GetWalls() const;
  Dg::DynamicArray<Corner> const & GetCorners() const;

private:

  void ScanAndInsert(int xyFixed, 
    int fixedValue, 
    bool above, 
    int start, 
    int one_past_end);

private:

  WallAssembler(WallAssembler const &);
  WallAssembler & operator=(WallAssembler const &);

private:

  Dg::HyperArray<bool, 2> const &   m_rMask;

  //TODO Ensure right-perpendicular of the line direction is the outward
  //     normal of the line.
  Dg::DynamicArray<Wall>    m_walls;
  Dg::DynamicArray<Corner>  m_corners;
};

#endif