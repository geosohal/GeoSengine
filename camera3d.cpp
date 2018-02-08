#include "camera3d.h"
#include <QDebug>


const vec3 Camera3D::LocalForward(0.0f, 0.0f, -1.0f);
const vec3 Camera3D::LocalUp(0.0f, 1.0f, 0.0f);
const vec3 Camera3D::LocalRight(1.0f, 0.0f, 0.0f);

// Transform By (Add/Scale)
void Camera3D::translate(const vec3 &dt)
{
  m_dirty = true;
  m_translation += dt;
}

void Camera3D::rotate(const Quaternion &dr)
{
  m_dirty = true;
  m_rotation = dr * m_rotation;
}

// Transform To (Setters)
void Camera3D::setTranslation(const vec3 &t)
{
  m_dirty = true;
  m_translation = t;
}

void Camera3D::setRotation(const Quaternion &r)
{
  m_dirty = true;
  m_rotation = r;
}

// Accessors
const MAT4 &Camera3D::getViewMatrix()
{
  if (m_dirty)
  {
    m_dirty = false;
    m_world = MAT4();
    m_world = m_rotation.ToMat4() * Translate(-m_translation);
  }
  return m_world;
}

// Queries
vec3 Camera3D::forward() const
{
  return m_rotation.Rotate(LocalForward);
}

vec3 Camera3D::right() const
{
  return m_rotation.Rotate(LocalRight);
}

vec3 Camera3D::up() const
{
  return m_rotation.Rotate(LocalUp);
}

// Qt Streams
QDebug operator<<(QDebug dbg, const Camera3D &transform)
{
  dbg << "Camera3D\n{\n";
  dbg << "Position: <" << transform.translation().x << ", " << transform.translation().y << ", " << transform.translation().z << ">\n";
  dbg << "Rotation: <" << transform.rotation().v.x << ", " << transform.rotation().v.y << ", " << transform.rotation().v.z << " | " << transform.rotation().s << ">\n}";
  return dbg;
}

// needs implementing
QDataStream &operator<<(QDataStream &out, const Camera3D &transform)
{
//  out << transform.m_translation;
//  out << transform.m_rotation;
  return out;
}

// needs implementing
QDataStream &operator>>(QDataStream &in, Camera3D &transform)
{
//  in >> transform.m_translation;
//  in >> transform.m_rotation;
  transform.m_dirty = true;
  return in;
}
