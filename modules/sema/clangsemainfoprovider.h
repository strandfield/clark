// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_CLANGSEMAINFOPROVIDER_H
#define CLARK_CLANGSEMAINFOPROVIDER_H

#include <codeviewer/semainfoprovider.h>

#include <program/translationunit.h>

#include <libclang-utils/clang-file.h>

class QTextDocument;

/**
 * \brief provides semantic information within a translation unit using libclang
 */
class ClangSemaInfoProvider : public SemaInfoProvider
{
  Q_OBJECT
public:
  // $todo: should we pass the document, or rather the document path directly
  ClangSemaInfoProvider(TranslationUnitHandle handle, const QTextDocument& document);
  ~ClangSemaInfoProvider();

  Features features() const override;

  SymbolObject* getSymbol(const TokenInfo& tokinfo) override;
  SymbolReferencesInDocument* getReferencesInDocument(SymbolObject* symbol, const QString& filePath) override;
  ::IncludesInFile* getIncludesInFile(const QString& filePath) override;

private:
  TranslationUnitHandle m_handle;
  std::unique_ptr<libclang::File> m_file;
};

#endif // CLARK_CLANGSEMAINFOPROVIDER_H
