#ifndef WINDOW_H
#define WINDOW_H

#include <QOpenGLWindow>
#include <QMatrix4x4>
#include <QElapsedTimer>
#include <QString>
#include "transform3d.h"
#include "camera3d.h"
#include "mesh.h"
#include "openglfunctions.h"
#include "shader.h"
#include "gbuffer.h"
#include "aoBuffer.h"
#include "AnimatedModel.h"
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

    enum SHADERTYPE
    {
        SSAO, IBL, OTHER
    };

  Window();
  ~Window();
  void initializeGL();
  void resizeGL(int width, int height);
  void paintGL();


  void updateShaderUniform(SHADERTYPE shaderType, const QString& uniform, float value);
  void updateShaderUniform(SHADERTYPE shaderType, const QString& uniform, int value);
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
  MAT4 m_projection;
  Camera3D m_camera;
  Transform3D m_transform;
//  MAT4 worldProj;
  GBuffer gbuffer;
  ShaderProgram* simpleShader;
  DirLightProgram* dirLightShader;
  ShaderProgram* geomShader; // for deferred shading
  ShaderProgram* aoProgram;
  IBLProgram* iblShader;
  SkinProgram* skinningShader;  // for character skin
  ShaderProgram* bilateralBlur; // compute shaders for blurring SSAO map
  ShaderProgram* bilateralBlurV;
  AoFbo aoFbo;
  Texture* envMap;
  Texture* irradMap;
  MAT4 Identity;
  float near;   // near/far planes
  float far;

  // Private Helpers
  void printVersionInformation();

  // Scene items
  BasicMesh* dragonMesh;
  BasicMesh* floor;
  BasicMesh* spider;
  BasicMesh* quad; // used for full screen quad on light pass
  BasicMesh* skyDome;
  AnimatedModel* skeleton;  // character

  QElapsedTimer timer;
};

#endif // WINDOW_H
