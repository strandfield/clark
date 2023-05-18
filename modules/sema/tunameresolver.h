// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TUNAMERESOLVER_H
#define CLARK_TUNAMERESOLVER_H

#include <codeviewer/syntaxhighlighter.h>

#include <program/translationunit.h>

#include <libclang-utils/clang-file.h>

class TranslationUnitNameResolver : public SyntaxHighlighterNameResolver
{
  Q_OBJECT
public:
  TranslationUnitNameResolver(TranslationUnitHandle handle, const QTextDocument& document);

  SyntaxHighlighter::Format resolve(const QTextDocument& document, int line, int col, const cpptok::Token& tok) override;

private:
  TranslationUnitHandle m_handle;
  std::unique_ptr<libclang::File> m_file;
};

#endif // CLARK_TUNAMERESOLVER_H
