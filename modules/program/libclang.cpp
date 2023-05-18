// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "libclang.h"

#include <QDebug>

LibClang::LibClang(QObject* parent) : QObject(parent)
{
  tryLoadLibclang();
}

LibClang::~LibClang()
{

}

std::shared_ptr<libclang::LibClang> LibClang::libclang() const
{
  return m_libclang_library;
}

bool LibClang::libclangAvailable() const
{
  return libclang() != nullptr;
}

bool LibClang::tryLoadLibclang()
{
  if (libclangAvailable())
    return true;

  try 
  {
    m_libclang_library = std::make_shared<libclang::LibClang>();
  }
  catch (std::exception& ex) 
  {
    qDebug() << ex.what();
    return false;
  }
  
  Q_EMIT libclangAvailableChanged();

  return true;
}
