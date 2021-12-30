//@group Scene

#include "Camera.h"

namespace DgE
{
  Camera::Camera()
    : m_projectionType(ProjectionType::Perspective)
    , m_projectionMatrix{}
    , m_viewMatrix{}
    , m_viewProjectionMatrix{}
    , m_position(0.f, 0.f, 0.f)
    , m_rotation(0.f, 0.f, 0.f, 1.f)
    , m_FOV(1.5f)
    , m_near(0.001f)
    , m_far(1000.f)
    , m_aspectRatio(16.f / 9.f)
  {
    Reproject();
  }
}