#ifndef OPENGLMARKERSCOPE_H
#define OPENGLMARKERSCOPE_H OpenGLMarkerScoped

#include <string>
#include <QString>
#include "openglprofiler.h"

class OpenGLMarkerScoped
{
public:
  OpenGLMarkerScoped(const char *name);
  OpenGLMarkerScoped(const std::string &name);
  OpenGLMarkerScoped(const QString &name);
  ~OpenGLMarkerScoped();
};

inline OpenGLMarkerScoped::OpenGLMarkerScoped(const char *name) { OpenGLProfiler::PushGpuMarker(name); }
inline OpenGLMarkerScoped::OpenGLMarkerScoped(const QString &name) { OpenGLProfiler::PushGpuMarker(qPrintable(name)); }
inline OpenGLMarkerScoped::OpenGLMarkerScoped(const std::string &name) { OpenGLProfiler::PushGpuMarker(name.c_str()); }
inline OpenGLMarkerScoped::~OpenGLMarkerScoped() { OpenGLProfiler::PopGpuMarker(); }

#endif // OPENGLMARKERSCOPE_H

