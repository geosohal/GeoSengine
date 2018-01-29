#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLWindow>
#include <QMatrix4x4>
#include "transform3d.h"
#include "camera3d.h"
#include "mesh.h"
#include "openglfunctions.h"
#include "shader.h"
#include "gbuffer.h"
#include "aoBuffer.h"
// OpenGL Classes
class OpenGLError;
class OpenGLShaderProgram;
#include "openglbuffer.h"
#include "openglvertexarrayobject.h"
#include "texture.h"

class QOpenGLShaderProgram;

class Window : public QOpenGLWindow,
               protected OpenGLFunctions
{
  Q_OBJECT

// OpenGL Events
public:
  Window();
  ~Window();
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();
protected slots:
  void teardownGL();
  void update();

protected:
  bool event(QEvent *event);
  void errorEventGL(OpenGLError *event);
  void keyPressEvent(QKeyEvent *event);
  void keyReleaseEvent(QKeyEvent *event);
  void mousePressEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

private:
  // OpenGL State Information
//  QOpenGLBuffer m_vertex;
//  QOpenGLVertexArrayObject m_object;
//  QOpenGLShaderProgram *m_program;

//  // Shader Information
//  int u_modelToWorld;
//  int u_worldToCamera;
//  int u_cameraToView;
  QMatrix4x4 m_projection;
  Camera3D m_camera;
  Transform3D m_transform;
//  MAT4 worldProj;
  GBuffer gbuffer;
  DirLightProgram* dirLightShader;
  ShaderProgram* lightShader;
  ShaderProgram* aoProgram;
  IBLProgram* iblShader;
  AoFbo aoFbo;
  Texture* envMap;
  Texture* irradMap;
  MAT4 Identity;

  // Private Helpers
  void printVersionInformation();

  // Scene items
  BasicMesh* dragonMesh;
  BasicMesh* quad; // used for full screen quad on light pass
};

#endif // WINDOW_H
