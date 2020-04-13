#include <iostream>

#include "TestHarness.h"
#include "Memory.h"
#include "Resource.h"

static int gVal = 0;

class TestClass
{
public:

  ~TestClass()
  {
    gVal++;
  }
};

TEST(Stack_ResourceNoID, creation_ResourceNoID)
{
  gVal = 0;
  {
    Engine::Ref<TestClass> refTC(new TestClass());
  }
  CHECK(gVal == 1);
}

TEST(Stack_User, creation_User)
{
  CHECK(Engine::RegisterResource(42, new TestClass()));
  {
    Engine::Ref<TestClass> refTCWithID(42);
    CHECK(!refTCWithID.IsNull());
  }

  gVal = 0;
  Engine::DestroyResource(42);
  CHECK(gVal == 1);
}