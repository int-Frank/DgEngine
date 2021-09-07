#ifndef TERRAIN_H
#define TERRAIN_H

#include "System.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "RendererProgram.h"
#include "Material.h"
#include "Texture.h"

using namespace DgE;

uint32_t const CHUNK_DIMENSION = 256;
uint32_t const MAP_DIMENSION = 4096;

template<uint32_t DIMENSION>
struct HeightMap
{
  vec3 verts[DIMENSION][DIMENSION];
};

struct Map
{
  HeightMap<MAP_DIMENSION> heightMap;

  // Rendering
  Ref<VertexBuffer> vbo;
  Ref<IndexBuffer>  ibo;
  Ref<VertexArray>  vao;
};

struct Chunk
{
  // Flag true when about to queue for work.
  // The main thread callback will set this to false once done.
  bool isLoading;

  // Rendering
  Ref<VertexBuffer> vbo;

  // lower left
  int32_t x, y;

  // Modified by the worker thread...
  float height[CHUNK_DIMENSION * CHUNK_DIMENSION];
};

class Terrain : public System
{
public:

  MAKE_SYSTEM_DECL

  void OnAttach() override;
  void HandleMessage(Message * a_pMsg) override;
  void OnDetach() override;
  void Render() override;
  void Update(float a_dt) override;

private:

  class PIMPL;
  PIMPL * m_pimpl;
};



class Camera : public System
{
public:

  MAKE_SYSTEM_DECL

  void OnAttach() override;
  void HandleMessage(Message * a_pMsg) override;
  void OnDetach() override;
  void Render() override;
  void Update(float a_dt) override;
};
#endif
