#include "Log.h"

#include "Buffer.h"
#include "unicode.h"

#define CHECK(val) do { if (!(val)) LOG_ERROR("TEST FAILED! Line: {}", __LINE__); } while(false)

void TEST_BufferLayout()
{
  Engine::BufferLayout layout(
    {
      {Engine::ShaderDataType::FLOAT, "float", true},
      {Engine::ShaderDataType::VEC3, "vec3", false},
      {Engine::ShaderDataType::MAT3, "mat3", true},
      {Engine::ShaderDataType::IVEC2, "ivec2", false}
    });

  unsigned char * buf = new unsigned char[layout.Size()];
  layout.Serialize(buf);
  Engine::BufferLayout output;
  output.Deserialize(buf);

  CHECK(layout.GetStride() == output.GetStride());

  CHECK(layout.GetElements()[0].name == output.GetElements()[0].name);
  CHECK(layout.GetElements()[0].type == output.GetElements()[0].type);
  CHECK(layout.GetElements()[0].size == output.GetElements()[0].size);
  CHECK(layout.GetElements()[0].offset == output.GetElements()[0].offset);
  CHECK(layout.GetElements()[0].normalized == output.GetElements()[0].normalized);

  CHECK(layout.GetElements()[1].name == output.GetElements()[1].name);
  CHECK(layout.GetElements()[1].type == output.GetElements()[1].type);
  CHECK(layout.GetElements()[1].size == output.GetElements()[1].size);
  CHECK(layout.GetElements()[1].offset == output.GetElements()[1].offset);
  CHECK(layout.GetElements()[1].normalized == output.GetElements()[1].normalized);

  CHECK(layout.GetElements()[2].name == output.GetElements()[2].name);
  CHECK(layout.GetElements()[2].type == output.GetElements()[2].type);
  CHECK(layout.GetElements()[2].size == output.GetElements()[2].size);
  CHECK(layout.GetElements()[2].offset == output.GetElements()[2].offset);
  CHECK(layout.GetElements()[2].normalized == output.GetElements()[2].normalized);

  CHECK(layout.GetElements()[3].name == output.GetElements()[3].name);
  CHECK(layout.GetElements()[3].type == output.GetElements()[3].type);
  CHECK(layout.GetElements()[3].size == output.GetElements()[3].size);
  CHECK(layout.GetElements()[3].offset == output.GetElements()[3].offset);
  CHECK(layout.GetElements()[3].normalized == output.GetElements()[3].normalized);

  delete[] buf;
}

void TEST_UTF8()
{
  uint8_t const utf8Str[] = {0x61, 0x73, 0x64, 0x66, 0xF3, 0x9A, 0x8D, 0xA0, 0x00};

  Engine::UTF8Parser parser((char*)utf8Str);

  CHECK(parser.Next() == 0x61);
  CHECK(parser.Next() == 0x73);
  CHECK(parser.Next() == 0x64);
  CHECK(parser.Next() == 0x66);
  CHECK(parser.Next() == 893792);
  CHECK(parser.Done());
}

void RunTests()
{
  TEST_BufferLayout();
  TEST_UTF8();

  LOG_INFO("Finished running tests.");
}