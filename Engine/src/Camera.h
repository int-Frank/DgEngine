//@group Scene

#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>

#include "Utils.h"

namespace DgE
{
  struct Component_POSITION
  {
  
  };

  class Camera
  {
  public:

    enum class ProjectionType
    {
      Perspective = 0, Orthographic = 1
    };

    Camera();

    void SetViewportSize(uint32_t width, uint32_t height);

    void SetVerticalFOV(float);
    void SetHorizontalFOV(float);
    void SetNearClip(float);
    void SetFarClip(float);

    mat44 const & GetProjectionMatrix() const;
		mat44 const & GetViewMatrix() const;
		mat44 const & GetViewProjectionMatrix() const;

  private:

    void Reproject();

  private:

    ProjectionType m_projectionType;

    mat44 m_projectionMatrix;
    mat44 m_viewMatrix;
    mat44 m_viewProjectionMatrix;

    vec3  m_position;
    quat  m_rotation;

    float m_FOV;
    float m_near;
    float m_far;

    float m_aspectRatio;
  };
}

#endif