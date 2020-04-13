#include <exception>
#include <sstream>

#include "BlockPartition.h"
#include "BSR_Assert.h"

int BlockPartition::X(int a_val) const
{
  return a_val % int(m_mask.length(1));
}

int BlockPartition::Y(int a_val) const
{
  return a_val / int(m_mask.length(1));
}

int BlockPartition::GetNextSeed(int a_seed)
{
  while (a_seed < m_mask.length(0) * m_mask.length(1))
  {
    int x = X(a_seed);
    int y = Y(a_seed);

    if (!m_mask.at(x, y))
      return a_seed;
    a_seed++;
  }

  return INVALID_SEED;
}

bool BlockPartition::GetNextBlock(unsigned a_blockSize, int & a_seed, Block & a_out)
{
  while (true)
  {
    a_seed = GetNextSeed(a_seed);
    if (a_seed == INVALID_SEED)
      break;

    a_out = BuildBlock(a_seed);
    if (a_out.dimensions[Block::W] >= a_blockSize && a_out.dimensions[Block::H] >= a_blockSize)
      return true;
  }
  return false;
}

Block BlockPartition::BuildBlock(int a_seed)
{
  uint32_t h = 1;
  uint32_t w = 1;
  uint32_t min_y = Y(a_seed);
  uint32_t min_x = X(a_seed);

  //CheckCoordinate(a_seed);
  BSR_ASSERT(!m_mask(min_x, min_y), "Not a proper seed");

  uint32_t flags = 0;
  while (flags != 0xF)
  {
    //Try to expand block in the -x direction
    if (!(flags & 1))
    {
      if (min_x > 0)
      {
        for (uint32_t y = min_y; y < min_y + h; y++)
        {
          if (m_mask.at(size_t(min_x - 1), size_t(y)))
          {
            flags |= 1;
            break;
          }
        }

        if (!(flags & 1))
        {
          min_x = min_x - 1;
          w = w + 1;
        }
      }
      else
        flags |= 1;
    }

    //Try to expand block in the +y direction
    if (!(flags & 2))
    {
      if (min_y + h < uint32_t(m_mask.length(1)))
      {
        for (uint32_t x = min_x; x < min_x + w; x++)
        {
          if (m_mask.at(size_t(x), size_t(min_y + h)))
          {
            flags |= 2;
            break;
          }
        }

        if (!(flags & 2))
          h = h + 1;
      }
      else
        flags |= 2;
    }

    //Try to expand block in the +x direction
    if (!(flags & 4))
    {
      if (min_x + w < uint32_t(m_mask.length(0)))
      {
        for (uint32_t y = min_y; y < min_y + h; y++)
        {
          if (m_mask.at(size_t(min_x + w), size_t(y)))
          {
            flags |= 4;
            break;
          }
        }

        if (!(flags & 4))
          w = w + 1;
      }
      else
        flags |= 4;
    }

    //Try to expand block in the -y direction
    if (!(flags & 8))
    {
      if (min_y > 0)
      {
        for (uint32_t x = min_x; x < min_x + w; x++)
        {
          if (m_mask.at(size_t(x), size_t(min_y - 1)))
          {
            flags |= 8;
            break;
          }
        }

        if (!(flags & 8))
        {
          min_y = min_y - 1;
          h = h + 1;
        }
      }
      else
        flags |= 8;
    }
  }

  Block result;

  result.lowerLeft[Block::X] = min_x;
  result.lowerLeft[Block::Y] = min_y;
  result.dimensions[Block::W] = w;
  result.dimensions[Block::H] = h;

  return result;
}

void BlockPartition::MaskOut(Block const & a_block)
{
  for (size_t x = size_t(a_block.lowerLeft[Block::X]); x < size_t(a_block.lowerLeft[Block::X] + a_block.dimensions[Block::W]); x++)
  {
    for (size_t y = size_t(a_block.lowerLeft[Block::Y]); y < size_t(a_block.lowerLeft[Block::Y] + a_block.dimensions[Block::H]); y++)
    {
      m_mask(x, y) = true;
    }
  }
}

BlockPartition::BlockPartition(Dg::HyperArray<bool, 2> const & m_mask)
  : m_mask(m_mask)
  , m_maxBlockSize(64)
{
  BSR_ASSERT(m_mask.length(0) <= 256);
  BSR_ASSERT(m_mask.length(1) <= 256);
}

void BlockPartition::SetMaxBlockSize(unsigned a_val)
{
  m_maxBlockSize = a_val;
}

Dg::DynamicArray<Block> BlockPartition::Run()
{
  Dg::DynamicArray<Block> result;
  unsigned blockSize = m_maxBlockSize;
  int seed = 0;

  while (blockSize > 0)
  {
    Block block;
    if (GetNextBlock(blockSize, seed, block))
    {
      MaskOut(block);
      result.push_back(block);
      continue;
    }

    blockSize--;
    seed = 0;
  }
  return result;
}