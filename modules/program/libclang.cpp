// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "libclang.h"

#include <QDebug>

LibClang::LibClang(QObject* parent) : LibClang(QString(), parent)
{

}

LibClang::LibClang(const QString& libclangPath, QObject* parent) : QObject(parent)
{
  tryLoadLibclang(libclangPath);
}

LibClang::~LibClang()
{

}

void LibClang::setLibrary(std::shared_ptr<libclang::LibClang> lib)
{
  m_libclang_library = lib;
  Q_EMIT libclangAvailableChanged();
}

std::shared_ptr<libclang::LibClang> LibClang::library() const
{
  return m_libclang_library;
}

bool LibClang::libclangAvailable() const
{
  return libclang() != nullptr;
}

bool LibClang::tryLoadLibclang(const QString& path)
{
  if (libclangAvailable())
    return true;

  setLibrary(tryLoad(path));

  return libclangAvailable();
}

std::shared_ptr<libclang::LibClang> LibClang::tryLoad(const QString& path)
{
  try
  {
    return std::make_shared<libclang::LibClang>(path.isEmpty() ? std::string("libclang") : path.toStdString());
  }
  catch (std::exception& ex)
  {
    qDebug() << ex.what();
  }

  return nullptr;
}
