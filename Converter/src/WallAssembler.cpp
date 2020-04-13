//#include "WallAssembler.h"
//
//WallAssembler::WallAssembler(Dg::HyperArray<bool, 2> const & a_mask)
//  : m_rMask(a_mask)
//{
//
//}
//
//Dg::DynamicArray<Wall> const & WallAssembler::GetWalls() const
//{
//  return m_walls;
//}
//
//Dg::DynamicArray<Corner> const & WallAssembler::GetCorners() const
//{
//  return m_corners;
//}
//
//void WallAssembler::ScanAndInsert(int a_xyFixed,
//                                  int a_rowCol,
//                                  bool a_above,
//                                  int a_start,
//                                  int a_end)
//{
//  //TODO add checks
//  float foffset = a_above ? 1.0f : 0.0f;
//  int ioffset = a_above ? 1 : -1;
//
//  int xyMoving = (a_xyFixed + 1) % 2;
//
//  int xy[2];
//  xy[xyMoving] = a_rowCol;
//
//  int i = a_start;
//  while (i < a_end)
//  {
//    xy[xyMoving] = i;
//
//    //No wall
//    if (!m_rMask(xy[0], xy[1]))
//    {
//      //Wall on the offset
//      int xyWall[2] = {xy[0], xy[1]};
//      xyWall[a_xyFixed] = a_rowCol + ioffset;
//      if (m_rMask(xyWall[0], xyWall[1]))
//      {
//        Wall wall;
//        wall.origin[a_xyFixed] = float(xy[a_xyFixed]);
//        wall.origin[xyMoving] = float(xy[xyMoving]) + foffset;
//
//        while (!m_rMask(xy[0], xy[1]) 
//             && m_rMask(xyWall[0], xyWall[1]) 
//             && i < size_t(a_end))
//        {
//          i++;
//          xy[xyMoving] = i;
//          xyWall[xyMoving] = i;
//        }
//
//        wall.length = float(xyWall[xyMoving]) - wall.origin[xyMoving];
//        
//        if (a_xyFixed == 0)
//        {
//          if (a_above)
//            wall.direction = VEC_NORTH;
//          else
//            wall.direction = VEC_SOUTH;
//        }
//        else
//        {
//          if (a_above)
//            wall.direction = VEC_EAST;
//          else
//            wall.direction = VEC_WEST;
//        }
//
//        m_walls.push_back(wall);
//      }
//    }
//    i++;
//  }
//}
//
//void WallAssembler::Run()
//{
//  m_walls.clear();
//  m_corners.clear();
//
//  //Add in lines
//  for (size_t y = 1; y < m_rMask.length(1) - 1; y++)
//  {
//    ScanAndInsert(0, (int)y, true, 1, (int)m_rMask.length(0));
//    ScanAndInsert(0, (int)y, false, 1, (int)m_rMask.length(0));
//  }
//
//  for (size_t x = 1; x < m_rMask.length(0) - 1; x++)
//  {
//    ScanAndInsert(1, (int)x, true, 1, (int)m_rMask.length(1));
//    ScanAndInsert(1, (int)x, false, 1, (int)m_rMask.length(1));
//  }
//
//  //Add in points
//  for (size_t y = 1; y < m_rMask.length(1) - 1; y++)
//  {
//    for (size_t x = 1; x < m_rMask.length(0) - 1; x++)
//    {
//      if (m_rMask(x, y))
//      {
//        bool freePt = m_rMask(x - 1, y);
//        freePt = freePt && m_rMask(x - 1, y + 1);
//        freePt = freePt && m_rMask(x, y + 1);
//        if (freePt)
//        {
//          Corner cnr;
//          cnr.point[0] = float(x);
//          cnr.point[1] = float(y + 1);
//          cnr.arcQuadrant = Corner::II;
//          m_corners.push_back(cnr);
//        }
//
//        freePt = m_rMask(x, y + 1);
//        freePt = freePt && m_rMask(x + 1, y + 1);
//        freePt = freePt && m_rMask(x + 1, y);
//        if (freePt)
//        {
//          Corner cnr;
//          cnr.point[0] = float(x + 1);
//          cnr.point[1] = float(y + 1);
//          cnr.arcQuadrant = Corner::I;
//          m_corners.push_back(cnr);
//        }
//
//        freePt = m_rMask(x + 1, y);
//        freePt = freePt && m_rMask(x + 1, y - 1);
//        freePt = freePt && m_rMask(x, y - 1);
//        if (freePt)
//        {
//          Corner cnr;
//          cnr.point[0] = float(x + 1);
//          cnr.point[1] = float(y);
//          cnr.arcQuadrant = Corner::IV;
//          m_corners.push_back(cnr);
//        }
//
//        freePt = m_rMask(x, y - 1);
//        freePt = freePt && m_rMask(x - 1, y - 1);
//        freePt = freePt && m_rMask(x - 1, y);
//        if (freePt)
//        {
//          Corner cnr;
//          cnr.point[0] = float(x);
//          cnr.point[1] = float(y);
//          cnr.arcQuadrant = Corner::III;
//          m_corners.push_back(cnr);
//        }
//      }
//    }
//  }
//}