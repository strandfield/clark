// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_SYNTAXHIGHLIGHTER_H
#define CLARK_SYNTAXHIGHLIGHTER_H

#include "tokenizer.h"

#include <QSyntaxHighlighter>

#include <vector>

/**
 * \brief base class for C++ syntax highlighter
 * 
 * This class provides a list of formats that subclasses can use to
 * highlight C++ source code in a QTextDocument.
 */
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
    ST_Default = -1,
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

class SyntaxHighlighterNameHighlighter;

/**
 * \brief a C++ syntax highlighter that uses cpptok for tokenization
 * 
 * Highlighting of identifiers is delegated to SyntaxHighlighterNameHighlighter.
 */
class CpptokSyntaxHighlighter : public CppSyntaxHighlighter
{
  Q_OBJECT
public:
  explicit CpptokSyntaxHighlighter(QTextDocument* document);

  SyntaxHighlighterNameHighlighter& nameHighlighter() const;
  void setNameHighlighter(SyntaxHighlighterNameHighlighter* namehighlighter);

protected:
  void highlightBlock(const QString& text) override;

protected:
  QStringLineTokenizer& tokenizer();

private:
  QStringLineTokenizer m_tokenizer;
  SyntaxHighlighterNameHighlighter* m_name_highlighter = nullptr;
};

/**
 * \brief provides highlighting of identifiers
 */
class SyntaxHighlighterNameHighlighter : public QObject
{
  Q_OBJECT
public:
  explicit SyntaxHighlighterNameHighlighter(QObject* parent = nullptr);
  ~SyntaxHighlighterNameHighlighter();

  virtual CppSyntaxHighlighter::Format format(const QTextDocument& document, int line, int col, std::string_view text);

Q_SIGNALS:
  void update();
};

#endif // CLARK_SYNTAXHIGHLIGHTER_H
