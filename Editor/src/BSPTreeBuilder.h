#ifndef BSPTREEBUILDER_H
#define BSPTREEBUILDER_H

#include "Types.h"
#include "MapData.h"
#include "DgDynamicArray.h"

class BSPTreeBuilder
{
  typedef uint32_t BlockID;

public:

  BSPTreeBuilder(Dg::DynamicArray<Block> const &);
  ~BSPTreeBuilder();

  void Run();
  Dg::DynamicArray<Node> const & GetResults() const;

private:

  BSPTreeBuilder(BSPTreeBuilder const &);
  BSPTreeBuilder & operator=(BSPTreeBuilder const &);

private:
  void  Split(uint32_t a_nodeIndex,
              Dg::DynamicArray<BlockID> const &);
  uint32_t GetLeafIndex(BlockID);
  Dg::DynamicArray<vec2> GetCandidatePoints(Dg::DynamicArray<BlockID> const &) const;
  float MakeSplit(float, int, 
                  Dg::DynamicArray<BlockID> const & nodeBlocks,
                  Dg::DynamicArray<BlockID> & below,
                  Dg::DynamicArray<BlockID> & above) const;
  float TotalArea(Dg::DynamicArray<BlockID> const &) const;
private:

  Dg::DynamicArray<Block> const & m_rBlocks;
  Dg::DynamicArray<Node>          m_result;
};

#endif