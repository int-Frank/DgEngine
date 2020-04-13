#include "TestHarness.h"
#include "ResourceID.h"

typedef Engine::impl::ResourceID64 ResourceID64;

TEST(Stack_ResourceID, creation_ResourceID)
{
  ResourceID64 id;
  CHECK(id.IsNull());

  id.SetFlag(ResourceID64::Flag::Persistant, true);
  id.SetID(0xABCDEF12);
  id.SetType(0x9876);
  CHECK(id.GetID() == 0x9876ABCDEF12);
  CHECK(id.Is(ResourceID64::Flag::Persistant));
  CHECK(!id.IsNull());
  id.SetFlag(ResourceID64::Flag::Persistant, false);
  CHECK(!id.Is(ResourceID64::Flag::Persistant));
}