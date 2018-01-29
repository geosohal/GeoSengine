#ifndef OPENGLVERTEXARRAYOBJECT_H
#define OPENGLVERTEXARRAYOBJECT_H

#include "openglcommon.h"
#include <QOpenGLVertexArrayObject>

// Register to check OpenGLVertexArrayObject
class OpenGLVertexArrayObjectChecked : public QOpenGLVertexArrayObject
{
public:
  explicit OpenGLVertexArrayObjectChecked(QObject *parent = 0) : QOpenGLVertexArrayObject(parent) {}
  GL_CHECK(QOpenGLVertexArrayObject,create);
};

// Final class
class OpenGLVertexArrayObject : public OpenGLVertexArrayObjectChecked
{
public:
  explicit OpenGLVertexArrayObject(QObject *parent = 0) : OpenGLVertexArrayObjectChecked(parent) {}
};

#endif // OPENGLVERTEXARRAYOBJECT_H
