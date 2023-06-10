// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_CLANGSYMBOLREFERENCESINDOCUMENT_H
#define CLARK_CLANGSYMBOLREFERENCESINDOCUMENT_H

#include "clangsymbol.h"

#include <libclang-utils/clang-file.h>

#include <QFuture>

/**
 * \brief list the references of a symbol in a document using libclang
 *
 * This class uses clang_findReferencesInFile() to asynchronously list the references
 * of a symbol in a file.
 */
class ClangSymbolReferencesInDocument : public SymbolReferencesInDocument
{
  Q_OBJECT
public:
  explicit ClangSymbolReferencesInDocument(ClangSymbolObject& sym, const QString& filePath, const libclang::File& file, QObject* parent = nullptr);
  ~ClangSymbolReferencesInDocument();

  ClangSymbolObject* symbol() const;
  const libclang::File& file() const;

protected Q_SLOTS:
  void onFutureFinished();

private:
  libclang::File m_file;
  QFuture<std::vector<Position>> m_find_references_future;
};

#endif // CLARK_CLANGSYMBOLREFERENCESINDOCUMENT_H
