// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_LIBCLANG_H
#define CLARK_LIBCLANG_H

#include <libclang-utils/libclang.h>

#include <QObject>

class LibClang : public QObject
{
  Q_OBJECT

  Q_PROPERTY(bool libclangAvailable READ libclangAvailable NOTIFY libclangAvailableChanged)
public:
  explicit LibClang(QObject* parent = nullptr);
  ~LibClang();

  std::shared_ptr<libclang::LibClang> libclang() const;
  bool libclangAvailable() const;

  bool tryLoadLibclang();

Q_SIGNALS:
  void libclangAvailableChanged();

private:
  std::shared_ptr<libclang::LibClang> m_libclang_library;
};

#endif // CLARK_LIBCLANG_H
