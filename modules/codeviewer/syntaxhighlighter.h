// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_SYNTAXHIGHLIGHTER_H
#define CLARK_SYNTAXHIGHLIGHTER_H

#include "tokenizer.h"

#include <QSyntaxHighlighter>

#include <vector>


class CppSyntaxHighlighter : public QSyntaxHighlighter
{
  Q_OBJECT
public:
  explicit CppSyntaxHighlighter(QTextDocument* document);
  ~CppSyntaxHighlighter();

  enum Format
  {
    Default = 0,
    Keyword,
    Literal,
    Preproc,
    String,
    Function,
    Macro,
    Include,
    Typename,
    MemberName,
    NamespaceName,
    Comment,
  };

  enum State
  {
    ST_Comment = 1,
  };

protected:
  void initFormat(Format fmt, const QTextCharFormat& value);
  void setFormat(int start, int count, Format fmt);

protected:
  const std::vector<QTextCharFormat>& formats() const;

private:
  std::vector<QTextCharFormat> m_formats;
};

class SyntaxHighlighterNameResolver;

class CpptokSyntaxHighlighter : public CppSyntaxHighlighter
{
  Q_OBJECT
public:
  explicit CpptokSyntaxHighlighter(QTextDocument* document);

  SyntaxHighlighterNameResolver& nameResolver() const;
  void setNameResolver(SyntaxHighlighterNameResolver* nameresolver);

protected:
  void highlightBlock(const QString& text) override;

protected:
  QStringLineTokenizer& tokenizer();

private:
  QStringLineTokenizer m_tokenizer;
  SyntaxHighlighterNameResolver* m_name_resolver = nullptr;
};

class SyntaxHighlighterNameResolver : public QObject
{
  Q_OBJECT
public:
  explicit SyntaxHighlighterNameResolver(QObject* parent = nullptr);
  ~SyntaxHighlighterNameResolver();

  virtual CppSyntaxHighlighter::Format resolve(const QTextDocument& document, int line, int col, const cpptok::Token& tok);

Q_SIGNALS:
  void update();
  void updateBlock(const QTextBlock& b);
};

#endif // CLARK_SYNTAXHIGHLIGHTER_H
