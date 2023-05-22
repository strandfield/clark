// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_CLANGSYNTAXHIGHLIGHTER_H
#define CLARK_CLANGSYNTAXHIGHLIGHTER_H

#include <codeviewer/syntaxhighlighter.h>

#include <program/translationunit.h>

#include <libclang-utils/clang-file.h>
#include <libclang-utils/clang-token.h>

/**
 * \brief C++ syntax highlighter powered by libclang
 * 
 * This syntax highlighter uses clang_tokenize() and clang_annotateTokens() to highlight 
 * source code in a QTextDocument.
 */
class ClangSyntaxHighlighter : public CppSyntaxHighlighter
{
  Q_OBJECT
public:
  ClangSyntaxHighlighter(TranslationUnitHandle thandle, const libclang::File& file, QTextDocument* document);

  static Format format4cursor(const libclang::Cursor& c);
  static Format format4token(const libclang::Token& t);

protected:
  void highlightBlock(const QString& text) override;

private:
  void setCurrentBlockStateFromToken(const libclang::Token& t);

private:
  TranslationUnitHandle m_thandle;
  libclang::File m_file;
};

#endif // CLARK_CLANGSYNTAXHIGHLIGHTER_H
