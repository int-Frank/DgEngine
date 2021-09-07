#include "Log.h"

#include "Serialize.h"
#include "Buffer.h"
#include "unicode.h"
#include "ShaderUniform.h"

#define CHECK(val) do { if (!(val)) LOG_ERROR("TEST FAILED! Line: {}", __LINE__); } while(false)

void TEST_UniformBuffer()
{

}

void TEST_BufferLayout()
{
  DgE::BufferLayout layout(
    {
      {DgE::ShaderDataType::FLOAT, true},
      {DgE::ShaderDataType::VEC3, false},
      {DgE::ShaderDataType::MAT3, true},
      {DgE::ShaderDataType::IVEC2, false}
    });

  unsigned char * buf = new unsigned char[layout.Size()];
  layout.Serialize(buf);
  DgE::BufferLayout output;
  output.Deserialize(buf);

  CHECK(layout.GetStride() == output.GetStride());

  CHECK(layout.GetElements()[0].type == output.GetElements()[0].type);
  CHECK(layout.GetElements()[0].size == output.GetElements()[0].size);
  CHECK(layout.GetElements()[0].offset == output.GetElements()[0].offset);
  CHECK(layout.GetElements()[0].normalized == output.GetElements()[0].normalized);

  CHECK(layout.GetElements()[1].type == output.GetElements()[1].type);
  CHECK(layout.GetElements()[1].size == output.GetElements()[1].size);
  CHECK(layout.GetElements()[1].offset == output.GetElements()[1].offset);
  CHECK(layout.GetElements()[1].normalized == output.GetElements()[1].normalized);

  CHECK(layout.GetElements()[2].type == output.GetElements()[2].type);
  CHECK(layout.GetElements()[2].size == output.GetElements()[2].size);
  CHECK(layout.GetElements()[2].offset == output.GetElements()[2].offset);
  CHECK(layout.GetElements()[2].normalized == output.GetElements()[2].normalized);

  CHECK(layout.GetElements()[3].type == output.GetElements()[3].type);
  CHECK(layout.GetElements()[3].size == output.GetElements()[3].size);
  CHECK(layout.GetElements()[3].offset == output.GetElements()[3].offset);
  CHECK(layout.GetElements()[3].normalized == output.GetElements()[3].normalized);

  delete[] buf;
}

void TEST_Serialize()
{
  std::string strList[6] = {std::string("A,B,C"),
                            std::string("The quick brown fox jumps over the lazy dog."),
                            std::string(""),
                            std::string("1"),
                            std::string(""),
                            std::string("Final string")};

  uint8_t buf[128] = {};

  DgE::Serialize(buf, strList, 6);
  std::string outList[6] = {};
  DgE::Deserialize(buf, outList, 6);

  CHECK(strList[0] == outList[0]);
  CHECK(strList[1] == outList[1]);
  CHECK(strList[2] == outList[2]);
  CHECK(strList[3] == outList[3]);
  CHECK(strList[4] == outList[4]);
  CHECK(strList[5] == outList[5]);
}

void TEST_UTF8()
{
  uint8_t const utf8Str[] = {0x61, 0x73, 0x64, 0x66, 0xF3, 0x9A, 0x8D, 0xA0, 0x00};

  DgE::UTF8Parser parser((char*)utf8Str);

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
  TEST_Serialize();
  TEST_UTF8();

  LOG_INFO("Finished running tests.");
}