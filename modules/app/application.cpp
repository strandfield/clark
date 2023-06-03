// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "application.h"

#include "settings.h"
#include "version.h"

#include <program/libclang.h>

#include <iostream>

Application::Application(int& argc, char** argv)
  : QApplication(argc, argv)
{
  setApplicationName("clark");
  //setApplicationDisplayName("Clark");
  setApplicationVersion(clark::versionString().c_str());

  Settings& settings = init<Settings>();

  init<LibClang>(settings.value(Settings::libclangPathKey()).toString());
}

Application& Application::instance()
{
  return *qobject_cast<Application*>(qApp);
}

Settings& Application::settings() const
{
  return get<Settings>();
}
