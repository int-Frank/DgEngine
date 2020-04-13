
#include <exception>

#include "BSPTreeBuilder.h"
#include "DgDynamicArray.h"
#include "BSR_Assert.h"

#define SCORE_BY_AREA

BSPTreeBuilder::BSPTreeBuilder(Dg::DynamicArray<Block> const & a_blocks)
  : m_rBlocks(a_blocks)
{

}

BSPTreeBuilder::~BSPTreeBuilder()
{

}

void BSPTreeBuilder::Run()
{
  m_result.clear();

  if (m_rBlocks.size()  > 0)
  {
    Dg::DynamicArray<BlockID> nodeBlocks;

    //Our first node will contain every polygon.
    for (BlockID i = 0; i < BlockID(m_rBlocks.size()); i++)
    {
      nodeBlocks.push_back(i);
    }

    if (nodeBlocks.size() == 1)
    {
      uint32_t ind = GetLeafIndex(nodeBlocks[0]);
    }
    else
    {
      m_result.push_back(Node());
      Split(0, nodeBlocks);
    }
  }
}

void  BSPTreeBuilder::Split(uint32_t a_nodeIndex,
                            Dg::DynamicArray<BlockID> const & a_nodeBlocks)
{
  uint32_t bestScore = FLT_MAX;
  int bestElement = -1;
  float bestOffset = 0.0f;
  Dg::DynamicArray<BlockID> aboveBlocks, belowBlocks;

  Dg::DynamicArray<vec2> candidates = GetCandidatePoints(a_nodeBlocks);

  for (size_t c = 0; c < candidates.size(); c++)
  {
    for (int i = 0; i < 2; i++)
    {
      Dg::DynamicArray<BlockID> newAbovePolys, newBelowPolys;

      float newScore = MakeSplit(candidates[c][i], i, a_nodeBlocks, newAbovePolys, newBelowPolys);

      if (newScore < bestScore)
      {
        bestElement = i;
        bestOffset = candidates[c][i];
        bestScore = newScore;
        aboveBlocks = newAbovePolys;
        belowBlocks = newBelowPolys;
      }
    }
  }

  BSR_ASSERT(bestScore < FLT_MAX, "BSPTreeBuilder::Split() No valid split found!");
  
  m_result[a_nodeIndex].SetType(Node::E_Branch);

  bestOffset = round(bestOffset);

  BSR_ASSERT(bestOffset >= 0.0f);
  BSR_ASSERT(bestOffset <= 255.0f);

  m_result[a_nodeIndex].SetElement(bestElement);
  m_result[a_nodeIndex].SetOffset(bestOffset);

  if (aboveBlocks.size() == 1)
  {
    uint32_t ind = GetLeafIndex(aboveBlocks[0]);
    m_result[a_nodeIndex].SetChildAboveInd(ind);
  }
  else
  {
    m_result.push_back(Node());
    uint32_t ind = (uint32_t)m_result.size() - 1;
    m_result[a_nodeIndex].SetChildAboveInd(ind);
    Split(ind, aboveBlocks);
  }

  if (belowBlocks.size() == 1)
  {
    uint32_t ind = GetLeafIndex(belowBlocks[0]);
    m_result[a_nodeIndex].SetChildBelowInd(ind);
  }
  else
  {
    m_result.push_back(Node());
    uint32_t ind = (uint32_t)m_result.size() - 1;
    m_result[a_nodeIndex].SetChildBelowInd(ind);
    Split(ind, belowBlocks);
  }
}


uint32_t BSPTreeBuilder::GetLeafIndex(BlockID a_id)
{
  for (size_t i = 0; i < m_result.size(); i++)
  {
    if (m_result[i].GetType() == Node::E_Leaf)
    {
      if (m_result[i].GetBlockID() == a_id)
      {
        return (uint32_t)i;
      }
    }
  }

  m_result.push_back(Node());
  uint32_t ind = (uint32_t)m_result.size() - 1;
  m_result[ind].SetType(Node::E_Leaf);
  m_result[ind].SetBlockID(a_id);
  return ind;
}

Dg::DynamicArray<vec2> BSPTreeBuilder::GetCandidatePoints(Dg::DynamicArray<BlockID> const & a_blocks) const
{
  Dg::DynamicArray<vec2> result;

  for (size_t i = 0; i < a_blocks.size(); i++)
  {
    BlockID id = a_blocks[i];
    Block block = m_rBlocks[id];
    vec2 p0, p1;

    p0[0] = float(block.lowerLeft[Block::X]);
    p0[1] = float(block.lowerLeft[Block::Y]);

    p1[0] = float(block.lowerLeft[Block::X]) + block.dimensions[Block::W];
    p1[1] = float(block.lowerLeft[Block::Y]) + block.dimensions[Block::H];

    bool p0_exists = false;
    bool p1_exists = false;
    for (size_t b = 0; b < result.size(); b++)
    {
      if (result[b] == p0)
        p0_exists = true;
      if (result[b] == p1)
        p1_exists = true;

      if (p0_exists && p1_exists)
        break;
    }

    if (!p0_exists)
      result.push_back(p0);
    if (!p1_exists)
      result.push_back(p1);
  }
  return result;
}

float BSPTreeBuilder::MakeSplit(float a_offset, int a_ele, 
                                Dg::DynamicArray<BlockID> const & a_nodeBlocks,
                                Dg::DynamicArray<BlockID> & a_belowBlocks,
                                Dg::DynamicArray<BlockID> & a_aboveBlocks) const
{
  //page 362, float time collision detection
  //score = K * nStraddling + (1 - K) * abs(nAbove - nBelow)

  float K = 0.5f;
  float epsilon = 0.0001f;
  Dg::DynamicArray<BlockID> straddlingBlocks;

  for (size_t i = 0; i < a_nodeBlocks.size(); i++)
  {
    BlockID id = a_nodeBlocks[i];

    //flag values:
    // 1 : polygon sits completely to the LEFT of the line
    // 2 : polygon sits completely to the RIGHT of the line
    // 3 : polygon straddles line
    // 0 : Something weird is happening. Could be polygon has no vertices
    //     or the polygon is ultra thin, is aligned with the line and 
    //     straddles it.
    int flag = 0;
    float p0, p1;
    p0 = m_rBlocks[id].lowerLeft[a_ele];
    p1 = p0 + m_rBlocks[id].dimensions[a_ele];

    if (p0 >= a_offset)
    {
      a_aboveBlocks.push_back(id);
    }
    else if (p1 <= a_offset)
    {
      a_belowBlocks.push_back(id);
    }
    else
    {
      straddlingBlocks.push_back(id);
    }
  }

  //Invalid splits
  bool badSplit = (a_belowBlocks.size() == 0) && (a_aboveBlocks.size() == 0); //Splitting line intersects all polygons

#ifdef SCORE_BY_AREA
  float areaBelow = TotalArea(a_belowBlocks);
  float areaAbove = TotalArea(a_aboveBlocks);
  float areaStraddling = TotalArea(straddlingBlocks);
  float score = K * areaStraddling + (1.0f - K) * abs(areaBelow - areaAbove);
#else
  float nBelow = (float)a_belowBlocks.size();
  float nAbove = (float)a_aboveBlocks.size();
  float nStraddling = (float)straddlingBlocks.size();
  float score = K * nStraddling + (1.0 - K) * abs(nBelow - nAbove);
#endif

  for (size_t i = 0; i < straddlingBlocks.size(); i++)
  {
    a_belowBlocks.push_back(straddlingBlocks[i]);
    a_aboveBlocks.push_back(straddlingBlocks[i]);
  }

  badSplit = badSplit || (a_aboveBlocks.size() == a_nodeBlocks.size()); //Infinite recursion
  badSplit = badSplit || (a_belowBlocks.size() == a_nodeBlocks.size()); //Infinite recursion

  if (badSplit)
  {
    return FLT_MAX;
  }

  return score;
}

float BSPTreeBuilder::TotalArea(Dg::DynamicArray<BlockID> const & a_blocks) const
{
  float result = 0.0f;
  for (size_t i = 0; i < a_blocks.size(); i++)
  {
    Block block = m_rBlocks[a_blocks[i]];
    result += block.dimensions[0] * block.dimensions[1];
  }
  return result;
}

Dg::DynamicArray<Node> const & BSPTreeBuilder::GetResults() const
{
  return m_result;
}