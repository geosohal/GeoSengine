#ifndef CAMERA3D_H
#define CAMERA3D_H

#include "transform3d.h"
#include "transform.h"
#include "Quaternion.h"

class Camera3D
{
public:

  // Constants
  static const vec3 LocalForward;
  static const vec3 LocalUp;
  static const vec3 LocalRight;

  // Constructors
  Camera3D();

  // Transform By (Add/Scale)
  void translate(const vec3 &dt);
  void translate(float dx, float dy, float dz);
  void rotate(const Quaternion &dr);
  void rotate(float angle, const vec3 &axis);
  void rotate(float angle, float ax, float ay, float az);

  // Transform To (Setters)
  void setTranslation(const vec3 &t);
  void setTranslation(float x, float y, float z);
  void setRotation(const Quaternion &r);
  void setRotation(float angle, const vec3 &axis);
  void setRotation(float angle, float ax, float ay, float az);

  // Accessors
  const vec3& translation() const;
  const Quaternion& rotation() const;
  const MAT4& getViewMatrix();

  // Queries
  vec3 forward() const;
  vec3 right() const;
  vec3 up() const;

  float front;  // near plane
  float back;   // far plane
  float ry;

private:
  bool m_dirty;
  vec3 m_translation;
  Quaternion m_rotation;
  MAT4 m_world;

#ifndef QT_NO_DATASTREAM
  friend QDataStream &operator<<(QDataStream &out, const Camera3D &transform);
  friend QDataStream &operator>>(QDataStream &in, Camera3D &transform);
#endif
};

Q_DECLARE_TYPEINFO(Camera3D, Q_MOVABLE_TYPE);

inline Camera3D::Camera3D() : m_dirty(true), front(0), back(2000.0f), ry(.4f) {}

// Transform By (Add/Scale)
inline void Camera3D::translate(float dx, float dy,float dz) { translate(vec3(dx, dy, dz)); }
inline void Camera3D::rotate(float angle, const vec3 &axis) { rotate(Quaternion::FromAngleAxis(angle, axis)); }
inline void Camera3D::rotate(float angle, float ax, float ay,float az) { rotate(Quaternion::FromAngleAxis(angle, vec3(ax, ay, az))); }

// Transform To (Setters)
inline void Camera3D::setTranslation(float x, float y, float z) { setTranslation(vec3(x, y, z)); }
inline void Camera3D::setRotation(float angle, const vec3 &axis) { setRotation(Quaternion::FromAngleAxis(angle, axis)); }
inline void Camera3D::setRotation(float angle, float ax, float ay, float az) { setRotation(Quaternion::FromAngleAxis(angle, vec3(ax, ay, az))); }

// Accessors
inline const vec3& Camera3D::translation() const { return m_translation; }
inline const Quaternion& Camera3D::rotation() const { return m_rotation; }

// Qt Streams
#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug dbg, const Camera3D &transform);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream &operator<<(QDataStream &out, const Camera3D &transform);
QDataStream &operator>>(QDataStream &in, Camera3D &transform);
#endif


#endif // CAMERA3D_H
