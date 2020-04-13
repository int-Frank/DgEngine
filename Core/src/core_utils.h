#ifndef CORE_H
#define CORE_H

#include <stdint.h>
#include <string>

#include "DgR2Vector.h"
#include "DgR3Vector.h"
#include "DgR3Matrix.h"
#include "DgR2Matrix.h"
#include "DgBit.h"

#define BIT(x) (1 << x)
#define SIZEOF32(x) static_cast<uint32_t>(sizeof(x))

typedef unsigned char byte;

typedef Dg::R2::Vector<float>           vec3;
typedef Dg::R2::Vector_cartesian<float> vec2;
typedef Dg::R3::Vector<float>           vec4;
typedef Dg::R2::Matrix<float>           mat3;
typedef Dg::R3::Matrix<float>           mat4;

class RGBA
{
public:
  
  typedef uint32_t DataType;

  RGBA() :data(0xFF) {}
  RGBA(uint32_t val) :data(val) {}

  uint32_t r() const {return Dg::GetSubInt<uint32_t, 0,  8>(data);}
  uint32_t g() const {return Dg::GetSubInt<uint32_t, 8,  8>(data);}
  uint32_t b() const {return Dg::GetSubInt<uint32_t, 16, 8>(data);}
  uint32_t a() const {return Dg::GetSubInt<uint32_t, 24, 8>(data);}
  
  void r(uint32_t val) {data = Dg::SetSubInt<uint32_t, 0,  8>(data, val);}
  void g(uint32_t val) {data = Dg::SetSubInt<uint32_t, 8,  8>(data, val);}
  void b(uint32_t val) {data = Dg::SetSubInt<uint32_t, 16, 8>(data, val);}
  void a(uint32_t val) {data = Dg::SetSubInt<uint32_t, 24, 8>(data, val);}

  uint32_t data;
};

//--- Types ------------------------------------------------------------------

// 2 steps to initialise: 1) define a new NEW_COUNTER name (this is just used internally)
//                        2) Initialise the counter
//example:
// #undef NEW_COUNTER
// #define NEW_COUNTER MyCounter
// INIT_COUNTER(1)
// ..will start a new counter, beginning at 1
// int val[3] = {COUNTER, COUNTER, COUNTER} // = {1, 2, 3}
#define CONCAT2(a, b) a##b
#define CONCAT(a, b) CONCAT2(a, b)
#define NEW_COUNTER TEMP
#define INIT_COUNTER(val) namespace CONCAT(IMPL_COUNTER_, NEW_COUNTER) {int const COUNTER_BASE = __COUNTER__ - val + 1;}
#define COUNTER (__COUNTER__ - CONCAT(IMPL_COUNTER_, NEW_COUNTER)::COUNTER_BASE)

namespace Core
{
  //Advance a void pointer a number of bytes
  void* AdvancePtr(void*, size_t);
  void const * AdvancePtr(void const *, size_t);

  std::string ImportTextFile(std::string const & path);

  //Compare a std::string to a serialized string
  bool AreEqual(std::string const&, void const*);

  //Compare two serialized strings
  bool AreEqual(void const *, void const *);
}

#endif