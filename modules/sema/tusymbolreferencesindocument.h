// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TUSYMBOLREFERENCESINDOCUMENT_H
#define CLARK_TUSYMBOLREFERENCESINDOCUMENT_H

#include "tusymbol.h"

#include <libclang-utils/clang-file.h>

#include <QFuture>

class TranslationUnitSymbolReferencesInDocument : public SymbolReferencesInDocument
{
  Q_OBJECT
public:
  explicit TranslationUnitSymbolReferencesInDocument(TranslationUnitSymbolObject& sym, const QString& filePath, const libclang::File& file, QObject* parent = nullptr);
  ~TranslationUnitSymbolReferencesInDocument();

  TranslationUnitSymbolObject* symbol() const;
  const libclang::File& file() const;

protected Q_SLOTS:
  void onFutureFinished();

private:
  libclang::File m_file;
  QFuture<std::vector<Position>> m_find_references_future;
};

#endif // CLARK_TUSYMBOLREFERENCESINDOCUMENT_H
