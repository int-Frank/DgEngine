#ifndef GC_BOOLEANMASK2D_H
#define GC_BOOLEANMASK2D_H

#include <stdint.h>
#include "DgAssert.h"

namespace GC
{
  template<int H, int W>
  class BooleanMask2D
  {
    static_assert(H > 0 && W > 0, "Invalid BooleanMask2D dimension");

    template<int T>
    struct Attr;

    template<>
    struct Attr<1>
    {
      typedef uint8_t intType;
      static intType const shift = 3;
      static intType const mask = 7;
      static intType const nBits = CHAR_BIT * sizeof(intType);
    };

    template<>
    struct Attr<2>
    {
      typedef uint16_t intType;
      static intType const shift = 4;
      static intType const mask = 15;
      static intType const nBits = CHAR_BIT * sizeof(intType);
    };

    template<>
    struct Attr<4>
    {
      typedef uint32_t intType;
      static intType const shift = 5;
      static intType const mask = 31;
      static intType const nBits = CHAR_BIT * sizeof(intType);
    };

    template<>
    struct Attr<8>
    {
      typedef uint64_t intType;
      static intType const shift = 6;
      static intType const mask = 63;
      static intType const nBits = CHAR_BIT * sizeof(intType);
      static intType const nInts = (W * H + (sizeof(SizeType) * CHAR_BIT) - 1) / (sizeof(SizeType) * CHAR_BIT);
    };

    typedef Attr<8> TypeTraits;

  public:

    BooleanMask2D();
    ~BooleanMask2D();

    BooleanMask2D(BooleanMask2D const &);
    BooleanMask2D & operator=(BooleanMask2D const &);

    BooleanMask2D(BooleanMask2D &&) noexcept;
    BooleanMask2D & operator=(BooleanMask2D &&) noexcept;

    bool Get(size_t y, size_t x) const;
    void Set(size_t y, size_t x, bool);
    void Toggle(size_t y, size_t x);

  private:
    TypeTraits::intType * m_pData;
  };

  template<int H, int W>
  BooleanMask2D<H, W>::BooleanMask2D()
    : m_pData(new SizeType[nInts]())
  {

  }

  template<int H, int W>
  BooleanMask2D<H, W>::~BooleanMask2D()
  {
    delete[] m_pData; 
  }

  template<int H, int W>
  BooleanMask2D<H, W>::BooleanMask2D(BooleanMask2D const & a_other)
    : m_pData(new SizeType[nInts]())
  {
    for (size_t i = 0; i < nInts; i++)
      m_pData[i] = a_other.m_pData[i];
  }

  template<int H, int W>
  BooleanMask2D<H, W> & BooleanMask2D<H, W>::operator=(BooleanMask2D const & a_other)
  {
    for (size_t i = 0; i < nInts; i++)
      m_pData[i] = a_other.m_pData[i];
  }

  template<int H, int W>
  BooleanMask2D<H, W>::BooleanMask2D(BooleanMask2D && a_other)
    : m_pData(a_other.m_pData)
  {
    a_other.m_pData = nullptr;
  }

  template<int H, int W>
  BooleanMask2D<H, W> & BooleanMask2D<H, W>::operator=(BooleanMask2D && a_other)
  {
    if (this != &a_other)
    {
      m_pData = a_other.m_pData;

    }
    for (size_t i = 0; i < nInts; i++)
      m_pData[i] = a_other.m_pData[i];
  }

  template<int H, int W>
  bool BooleanMask2D<H, W>::Get(size_t a_y, size_t a_x) const
  {
    Dg::Assert(a_y <= H && a_x < W, "Invalid index value.");

    TypeTraits::intType bucket(i >> TypeTraits::shift);
    TypeTraits::intType n = i & TypeTraits::mask;

    return ((m_pBuckets[bucket] >> n) & TypeTraits::intType(1)) != 0;
  }

  template<int H, int W>
  void BooleanMask2D<H, W>::Set(size_t a_y, size_t a_x, bool a_val) 
  {
    Dg::Assert(a_y <= H && a_x < W, "Invalid index value.");

    TypeTraits::intType bucket(i >> TypeTraits::shift);
    TypeTraits::intType n = i & TypeTraits::mask;
    TypeTraits::intType x = (a_val) ? 1 : 0;

    m_pBuckets[bucket] = (m_pBuckets[bucket] & ~(TypeTraits::intType(1) << n)) | (x << n);
  }

  template<int H, int W>
  void BooleanMask2D<H, W>::Toggle(size_t a_y, size_t a_x) 
  {
    Dg::Assert(a_y <= H && a_x < W, "Invalid index value.");

    TypeTraits::intType bucket(i >> TypeTraits::shift);
    TypeTraits::intType n = i & TypeTraits::mask;

    m_pBuckets[bucket] ^= TypeTraits::intType(1) << n;
  }
}

#endif