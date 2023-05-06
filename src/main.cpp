// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "app/application.h"
#include "app/window.h"

#include <QDebug>


void cleanup(Application& app)
{

}

int main(int argc, char *argv[])
{
  Q_INIT_RESOURCE(media);
  Q_INIT_RESOURCE(thirdparty);

  Application app{ argc, argv };

  app.setQuitOnLastWindowClosed(false);

  auto* window = new Window;
  window->show();

  QObject::connect(&app, &QGuiApplication::lastWindowClosed, &app, [&]() {
    cleanup(app);
    app.quit();
    }, Qt::QueuedConnection);

  return app.exec();
}
