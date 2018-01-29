#include "window.h"
#include <QDebug>
#include <QString>
#include <QKeyEvent>
#include "vertex.h"
#include "input.h"
#include <QQuaternion>

#include "openglerror.h"
#include "openglshaderprogram.h"



#define GBUFFDEBUG
#define CHECKERRORNOX {GLenum err = GL::glGetError(); if (err != GL_NO_ERROR) { fprintf(stderr, "OpenGL werror (at line %d):\n m:%i", __LINE__, err); } }

/*******************************************************************************
 * OpenGL Events
 ******************************************************************************/

Window::Window()
{
  m_transform.translate(0.0f, 0.0f, -5.0f);
  OpenGLError::pushErrorHandler(this);
}

Window::~Window()
{
  OpenGLError::popErrorHandler();
}

void Window::initializeGL()
{
  // Initialize OpenGL Backend
  bool test = initializeOpenGLFunctions();
  GL::setInstance(this);
  connect(context(), SIGNAL(aboutToBeDestroyed()), this, SLOT(teardownGL()), Qt::DirectConnection);
  connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));
  printVersionInformation();

  // Set global information
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  // Application-specific initialization
  {
//    // Create Shader (Do not release until VAO is created)
//    m_program = new OpenGLShaderProgram(this);
//    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/simple.vert");
//    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/simple.frag");
//    m_program->link();
//    m_program->bind();

//    // Cache Uniform Locations
//    u_modelToWorld = m_program->uniformLocation("modelToWorld");
//    u_worldToCamera = m_program->uniformLocation("worldToCamera");
//    u_cameraToView = m_program->uniformLocation("cameraToView");

//    // Create Buffer (Do not release until VAO is created)
//    m_vertex.create();
//    m_vertex.bind();
//    m_vertex.setUsagePattern(QOpenGLBuffer::StaticDraw);
//    m_vertex.allocate(sg_vertexes, sizeof(sg_vertexes));

//    // Create Vertex Array Object
//    m_object.create();
//    m_object.bind();
//    m_program->enableAttributeArray(0);
//    m_program->enableAttributeArray(1);
//    m_program->setAttributeBuffer(0, GL_FLOAT, Vertex::positionOffset(), Vertex::PositionTupleSize, Vertex::stride());
//    m_program->setAttributeBuffer(1, GL_FLOAT, Vertex::colorOffset(), Vertex::ColorTupleSize, Vertex::stride());

//    // Release (unbind) all
//    m_object.release();
//    m_vertex.release();
//    m_program->release();
}
    lightShader = new ShaderProgram();
    lightShader->AddShader("/home/geo5/GeoSengine/shaders/geometry_pass.vs", GL_VERTEX_SHADER);
    lightShader->AddShader("/home/geo5/GeoSengine/shaders/geometry_pass.fs", GL_FRAGMENT_SHADER);
    lightShader->LinkProgram();
    DirectionalLight dirLight;
    dirLight.ambientIntensity = 0.4f;
    dirLight.color = vec3(1.0f, 1.0f, 1.0f);
    dirLight.diffuseIntensity = 0.5f;
    dirLight.direction = vec3(1.0f, 0.0f, 0.0f);
    if (!gbuffer.Initialize(this->width(), this->height())) {
        printf("\nGbufferFailed to initialize");;
    }

    dirLightShader = new DirLightProgram();
    dirLightShader->Initialize();
    dirLightShader->Use();
    dirLightShader->SetUniformi("gPositionMap", GBuffer::GBUFFER_TYPE_POS);
    dirLightShader->SetUniformi("gColorMap", GBuffer::GBUFFER_TYPE_DIFFUSE);
    dirLightShader->SetUniformi("gNormalMap", GBuffer::GBUFFER_TYPE_NORMAL);
    dirLightShader->SetScreenSize(this->width(), this->height());
    dirLightShader->SetDirLight(dirLight);

    aoFbo.CreateFBO(this->width(), this->height());
    aoProgram = new ShaderProgram();
    aoProgram->AddShader("/home/geo5/GeoSengine/shaders/ambientOcclusion.vs", GL_VERTEX_SHADER);
    aoProgram->AddShader("/home/geo5/GeoSengine/shaders/ambientOcclusion.fs", GL_FRAGMENT_SHADER);
    aoProgram->LinkProgram();

    iblShader = new IBLProgram();
    iblShader->Initialize();
    envMap = new Texture("/home/geo5/GeoSengine/content/Alexs_Apt_2k.hdr");
    if (!envMap->Load(true)) {
        CHECKERRORNOX
            printf("Error loading texture env");
        delete envMap;
        envMap = NULL;
    }
    else {
        CHECKERRORNOX
            printf("Loaded texture global env \n");
    }

    irradMap = new Texture("/home/geo5/GeoSengine/content/Alexs_Apt_2k.irr.hdr");
    if (!irradMap->Load(true)) {
        CHECKERRORNOX
            printf("Error loading irrad env");
        delete irradMap;
        irradMap = NULL;
    }
    else {
        CHECKERRORNOX
            printf("Loaded texture global irrad \n");
    }

    // load meshes
    dragonMesh = new BasicMesh();
    dragonMesh->LoadMesh("/home/geo5/GeoSengine/content/Dragon.obj");
    quad = new BasicMesh();
    quad->LoadMesh("/home/geo5/GeoSengine/content/quad.obj");
//    dragonMesh->LoadMesh("/home/geo5/GeoSengine/content/box.obj");
//    dragonMesh->LoadMesh("/home/geo5/GeoSengine/content/quad_r.obj");
//    dragonMesh->LoadMesh("/home/geo5/GeoSengine/content/gh_sample_animation.fbx");




}

void Window::resizeGL(int width, int height)
{
  m_projection.setToIdentity();
  m_projection.perspective(45.0f, width / float(height), 0.0f, 2000.0f);
 // worldProj = Perspective((m_camera.ry*width) / height, m_camera.ry, m_camera.front, m_camera.back);
//  dirLightShader->SetScreenSize(width, height);
//  if (!gbuffer.Initialize(width, height)) {
//      printf("\nGbufferFailed to initialize");;
//  }
}

void Window::paintGL()
{



 // glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  lightShader->Use();
  // from DS geometry pass
  gbuffer.DrawBind();// all depth and color writes now go to gbuffer's textures
  CHECKERRORNOX
#ifndef GBUFFDEBUG
  //prevent anything but this pass from writing into the depth buffer
  // as the light pass doesnt have anythign to write into it
  glDepthMask(GL_TRUE);
#endif
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifndef GBUFFDEBUG
  // limit depth test to the geometry pass
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND); // the light passes use blending, dont need it here
#endif
  const float* VPmatrix = (m_camera.toMatrix().transposed()*m_projection.transposed()).constData();
  lightShader->SetUniform4v("gVP", VPmatrix );
  MAT4 wtrans = Translate(-2.f,-4,-4)* Scale(0.5f, 0.5f, 0.5f);
  lightShader->SetUniform4v("gWorld", wtrans );
  dragonMesh->Render();
  CHECKERRORNOX
  lightShader->Unuse();

  QVector3D eye = m_camera.translation();
  // Deferred shading light pass
#ifndef GBUFFDEBUG
  {
      glDepthMask(GL_FALSE);
      glDisable(GL_DEPTH_TEST);
      // begin light passes by setting blending so we can add the output of
      // FS (source color) to framebuffer (destination color)
      //since each light source is handled by its own FS invocation
      glEnable(GL_BLEND);
      glBlendEquation(GL_FUNC_ADD);
      glBlendFunc(GL_ONE, GL_ONE);
      gbuffer.ReadBind();
      glClear(GL_COLOR_BUFFER_BIT);


//      aoFbo.BindForWrite();	// from now on all color writes will goto aoFBO.aoTexture?
//      glClear(GL_COLOR_BUFFER_BIT);	// if we dont clear texture keeps accumulating the writes
//      aoProgram->Use();
//      aoProgram->SetUniformi("positionMap", GBuffer::GBUFFER_TYPE_POS);
//      aoProgram->SetUniformi("normalMap", GBuffer::GBUFFER_TYPE_NORMAL);
//      aoProgram->SetUniformi("depthMap", GBuffer::GBUFFER_TYPE_SPEC);	// specular is not spec, but it has
//      //depth in .z component of the vec3
//      quad->Render();
//      aoFbo.UnbindWrite();	// switch back to default framebuffer

//      // todo add bilateral blur

//      envMap->Bind(GL_TEXTURE0 + 8);
//      irradMap->Bind(GL_TEXTURE0 + 9);
//      iblShader->Use();
//      iblShader->SetEnvMapTU(8);
//      iblShader->SetIrradTU(9);
//      iblShader->SetVP(VPmatrix);
//      iblShader->SetWorldMatrix(Identity);
//      iblShader->SetAOMap(aoFbo.aoTexture);
//      aoFbo.BindForRead(GL_TEXTURE0 + aoFbo.aoTexture);
//      iblShader->SetNormalTU(GBuffer::GBUFFER_TYPE_NORMAL);
//      iblShader->SetPosTU(GBuffer::GBUFFER_TYPE_POS);
//      iblShader->SetUniformi("colorMap",  GBuffer::GBUFFER_TYPE_DIFFUSE);
//      iblShader->SetEyePos(vec3(eye.x(), eye.y(), eye.z()));
//      iblShader->SetScreenDim(this->width(), this->height());
//      quad->Render();



      // directinal light pass
      dirLightShader->Use();
      dirLightShader->SetEyeWorldPos(eye.x(),eye.y(),eye.z());
      dirLightShader->SetWVP(Identity);
      quad->Render();
      CHECKERRORNOX
      dirLightShader->Unuse();
  }
#endif
#ifdef GBUFFDEBUG
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // restore default fbo(screen) and clear it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gbuffer.ReadBind();		// bind fbo and gbuffer
    // copy gbuffer textures into screen
    gbuffer.SetReadBuffer(GBuffer::GBUFFER_TYPE_POS); //enter buffer type to test
    glBlitFramebuffer(0, 0, this->width(), this->height(),
      0, 0, this->width, this->height(), GL_COLOR_BUFFER_BIT, GL_LINEAR);
#endif
}

void Window::teardownGL()
{
  // Actually destroy our OpenGL information
//  m_object.destroy();
//  m_vertex.destroy();
//  delete m_program;
}

void Window::update()
{
  // Update input
  Input::update();

  // Camera Transformation
  if (Input::buttonPressed(Qt::RightButton))
  {
    static const float transSpeed = 0.5f;
    static const float rotSpeed   = 0.5f;

    // Handle rotations
    m_camera.rotate(-rotSpeed * Input::mouseDelta().x(), Camera3D::LocalUp);
    m_camera.rotate(-rotSpeed * Input::mouseDelta().y(), m_camera.right());

    // Handle translations
    QVector3D translation;
    if (Input::keyPressed(Qt::Key_W))
    {
      translation += m_camera.forward();
    }
    if (Input::keyPressed(Qt::Key_S))
    {
      translation -= m_camera.forward();
    }
    if (Input::keyPressed(Qt::Key_A))
    {
      translation -= m_camera.right();
    }
    if (Input::keyPressed(Qt::Key_D))
    {
      translation += m_camera.right();
    }
    if (Input::keyPressed(Qt::Key_Q))
    {
      translation -= m_camera.up();
    }
    if (Input::keyPressed(Qt::Key_E))
    {
      translation += m_camera.up();
    }
    m_camera.translate(transSpeed * translation);
  }

  // Update instance information
  m_transform.rotate(1.0f, QVector3D(0.4f, 0.3f, 0.3f));

  // Schedule a redraw
  QOpenGLWindow::update();
}

bool Window::event(QEvent *e)
{
  if (e->type() == OpenGLError::type())
  {
    errorEventGL(static_cast<OpenGLError*>(e));
    return true;
  }
  return QOpenGLWindow::event(e);
}

void Window::errorEventGL(OpenGLError *event)
{
  qFatal("%s::%s => Returned an error!", event->callerName(), event->functionName());
}

void Window::keyPressEvent(QKeyEvent *event)
{
  if (event->isAutoRepeat())
  {
    event->ignore();
  }
  else
  {
    Input::registerKeyPress(event->key());
  }
}

void Window::keyReleaseEvent(QKeyEvent *event)
{
  if (event->isAutoRepeat())
  {
    event->ignore();
  }
  else
  {
    Input::registerKeyRelease(event->key());
  }
}

void Window::mousePressEvent(QMouseEvent *event)
{
  Input::registerMousePress(event->button());
}

void Window::mouseReleaseEvent(QMouseEvent *event)
{
  Input::registerMouseRelease(event->button());
}

/*******************************************************************************
 * Private Helpers
 ******************************************************************************/

void Window::printVersionInformation()
{
  QString glType;
  QString glVersion;
  QString glProfile;

  // Get Version Information
  glType = (context()->isOpenGLES()) ? "OpenGL ES" : "OpenGL";
  glVersion = reinterpret_cast<const char*>(GL::glGetString(GL_VERSION));

  // Get Profile Information
#define CASE(c) case QSurfaceFormat::c: glProfile = #c; break
  switch (format().profile())
  {
    CASE(NoProfile);
    CASE(CoreProfile);
    CASE(CompatibilityProfile);
  }
#undef CASE

  // qPrintable() will print our QString w/o quotes around it.
  qDebug() << qPrintable(glType) << qPrintable(glVersion) << "(" << qPrintable(glProfile) << ")";
}
