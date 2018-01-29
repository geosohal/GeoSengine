#include "openglfunctions.h"
#include <QRect>
#include <QStack>

OpenGLFunctions *GL::m_functions;
QRect sg_currViewport;
QStack<QRect> sg_viewportStack;

OpenGLFunctions *GL::getInstance()
{
  return GL::m_functions;
}

void GL::setInstance(OpenGLFunctions *f)
{
  GL::m_functions = f;
}

int GL::getInteger(GLenum property)
{
  GLint value;
  GL::glGetIntegerv(property, &value);
  return static_cast<int>(value);
}

void GL::pushViewport()
{
  sg_viewportStack.push(sg_currViewport);
}

void GL::popViewport()
{
  sg_currViewport = sg_viewportStack.pop();
  GL::getInstance()->glViewport (sg_currViewport.x(), sg_currViewport.y(), sg_currViewport.width(), sg_currViewport.height());
}

void GL::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
  sg_currViewport = QRect(x, y, width, height);
  GL::getInstance()->glViewport (x, y, width, height);
}
