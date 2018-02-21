//#include <QGuiApplication>
#include "window.h"
#include "optionsdialog.h"
#include <QApplication>
//#include <QtQuickControls2>
//#include <QPushButton>

static bool checkVersion(QOpenGLContext &context, QSurfaceFormat &format)
{
  QSurfaceFormat currSurface = context.format();
  QPair<int,int> currVersion = currSurface.version();
  QPair<int,int> reqVersion = format.version();
  if (currVersion.first > reqVersion.first)
    return true;
  return (currVersion.first == reqVersion.first && currVersion.second >= reqVersion.second);
}


static QSurfaceFormat* getFirstSupported(std::vector<QSurfaceFormat> &formats)
{
  QOpenGLContext context;
  for (QSurfaceFormat &format : formats)
  {
    context.setFormat(format);
    if (context.create())
    {
      if (checkVersion(context, format)) return &format;
    }
  }
  return NULL;
}


int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Set OpenGL Version information
  // Note: This format must be set before show() is called.
  std::vector<QSurfaceFormat> formats;

#if !defined(QT_OPENGL_ES)
  {
    QSurfaceFormat glFormat;
    glFormat.setRenderableType(QSurfaceFormat::OpenGL);
    glFormat.setProfile(QSurfaceFormat::CoreProfile);
    glFormat.setVersion(4,3);
    formats.push_back(glFormat);
  }
#endif

#if defined(QT_OPENGL_ES)
  {
    QSurfaceFormat glesFormat;
    glesFormat.setRenderableType(QSurfaceFormat::OpenGLES);
    glesFormat.setProfile(QSurfaceFormat::CoreProfile);
    glesFormat.setVersion(3,0);
    formats.push_back(glesFormat);
  }
#endif

  // Find out which version we support
  QSurfaceFormat *format = getFirstSupported(formats);
  if (format == NULL)
  {
    //QMessageBox::critical(0, "Critical Error", "No valid supported version of OpenGL found on device!");
    exit(-1);
  }

#ifdef    GL_DEBUG
  //format->setOption(QSurfaceFormat::);
#endif // GL_DEBUG
  format->setDepthBufferSize(0);


  // Set the window up
  Window window;
  window.setFormat(*format);
  window.resize(QSize(800, 600));
  window.show();

  OptionsDialog options;
  options.setWindow(&window);
  options.show();


  return app.exec();
}
