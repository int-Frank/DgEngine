#ifndef BLOCKPARTITION_H
#define BLOCKPARTITION_H

#include "Types.h"
#include "DgDynamicArray.h"
#include "DgHyperArray.h"
#include "MapData.h"

//TODO  Look for large blocks first (10 x 10 for exmaple). This way we can ensure larger areas are not
//      split by smaller ones. We can the decrement and search again (9 x 9 for example) all the way to 1.
class BlockPartition
{
public:

  BlockPartition(Dg::HyperArray<bool, 2> const &);
  void SetMaxBlockSize(unsigned);
  Dg::DynamicArray<Block> Run();

private:

  int GetNextSeed(int currentSeed); //change to 'bool GetNextSeed(XY & currentSeed, int size) const;'
  
  bool GetNextBlock(unsigned blockSize, int & seed, Block &);
  Block BuildBlock(int seed);
  void MaskOut(Block const &);

private:

  static int const INVALID_SEED = -1;

  int X(int) const;
  int Y(int) const;

  Dg::HyperArray<bool, 2> m_mask;
  unsigned                m_maxBlockSize;
};

#endif