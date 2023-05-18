// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TUSYMBOLINFOPROVIDER_H
#define CLARK_TUSYMBOLINFOPROVIDER_H

#include <codeviewer/symbolinfoprovider.h>

#include <program/translationunit.h>

#include <libclang-utils/clang-file.h>

class QTextDocument;

class TranslationUnitSymbolInfoProvider : public SymbolInfoProvider
{
  Q_OBJECT
public:
  // $todo: should we pass the document, or rather the document path directly
  TranslationUnitSymbolInfoProvider(TranslationUnitHandle handle, const QTextDocument& document);
  ~TranslationUnitSymbolInfoProvider();

  Features features() const override;

  SymbolObject* getSymbol(const TokenInfo& tokinfo) override;
  SymbolReferencesInDocument* getReferencesInDocument(SymbolObject* symbol, const QString& filePath) override;
  ::IncludesInFile* getIncludesInFile(const QString& filePath) override;

private:
  TranslationUnitHandle m_handle;
  std::unique_ptr<libclang::File> m_file;
};

#endif // CLARK_TUSYMBOLINFOPROVIDER_H
