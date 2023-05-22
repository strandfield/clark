// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "syntaxhighlighter.h"

#include <QTextDocument>

#include <set>

CppSyntaxHighlighter::CppSyntaxHighlighter(QTextDocument* document) : QSyntaxHighlighter(document)
{
  m_formats.resize(Comment + 1);

  QTextCharFormat fmt;
  fmt.setFont(document->defaultFont());

  fmt.setForeground(QColor("#808000"));
  initFormat(Format::Keyword, fmt);

  fmt.setForeground(QColor("#000080"));
  initFormat(Format::Literal, fmt);

  fmt.setForeground(QColor("#000080"));
  initFormat(Format::Preproc, fmt);

  fmt.setForeground(QColor("#008000"));
  initFormat(Format::String, fmt);

  fmt.setForeground(QColor("#00677c"));
  initFormat(Format::Function, fmt);

  fmt.setForeground(QColor("#000080"));
  initFormat(Format::Macro, fmt);

  fmt.setForeground(QColor("#000080"));
  initFormat(Format::Macro, fmt);

  fmt.setForeground(QColor("#008000"));
  initFormat(Format::Include, fmt);

  fmt.setForeground(QColor("#800080"));
  initFormat(Format::Typename, fmt);

  fmt.setForeground(QColor("#860d0d"));
  initFormat(Format::MemberName, fmt);

  fmt.setForeground(QColor("#800080"));
  initFormat(Format::NamespaceName, fmt);

  fmt.setForeground(QColor("#008000"));
  initFormat(Format::Comment, fmt);
}

CppSyntaxHighlighter::~CppSyntaxHighlighter()
{

}


void CppSyntaxHighlighter::initFormat(Format fmt, const QTextCharFormat& value)
{
  m_formats[static_cast<size_t>(fmt)] = value;
}

void CppSyntaxHighlighter::setFormat(int start, int count, Format fmt)
{
  QSyntaxHighlighter::setFormat(start, count, m_formats.at(fmt));
}

const std::vector<QTextCharFormat>& CppSyntaxHighlighter::formats() const
{
  return m_formats;
}


SyntaxHighlighterNameHighlighter::SyntaxHighlighterNameHighlighter(QObject* parent) : QObject(parent)
{

}

SyntaxHighlighterNameHighlighter::~SyntaxHighlighterNameHighlighter()
{

}

CppSyntaxHighlighter::Format SyntaxHighlighterNameHighlighter::format(const QTextDocument& document, int line, int col, std::string_view text)
{
  static const std::set<std::string> known_namespaces = {
    "std", "Eigen", "Poco", "Qt"
  };

  static const std::set<std::string> known_types = {

  };

  {
    auto it = known_namespaces.find(std::string(text));

    if (it != known_namespaces.end())
      return CppSyntaxHighlighter::Format::NamespaceName;
  }

  {
    auto it = known_types.find(std::string(text));

    if (it != known_types.end())
      return CppSyntaxHighlighter::Format::Typename;
  }

  // Check if token is immediately followed by "::"
  if (std::strncmp(text.data() + text.length(), "::", 2) == 0)
  {
    return CppSyntaxHighlighter::Format::Typename;
  }

  return CppSyntaxHighlighter::Format::Default;
}

CpptokSyntaxHighlighter::CpptokSyntaxHighlighter(QTextDocument* document) : CppSyntaxHighlighter(document)
{
  setNameHighlighter(new SyntaxHighlighterNameHighlighter(this));
}

SyntaxHighlighterNameHighlighter& CpptokSyntaxHighlighter::nameHighlighter() const
{
  return *m_name_highlighter;
}

void CpptokSyntaxHighlighter::setNameHighlighter(SyntaxHighlighterNameHighlighter* namehighlighter)
{
  if (namehighlighter)
  {
    if (m_name_highlighter)
      m_name_highlighter->deleteLater();

    m_name_highlighter = namehighlighter;
    m_name_highlighter->setParent(this);
    connect(namehighlighter, &SyntaxHighlighterNameHighlighter::update, this, &QSyntaxHighlighter::rehighlight);
    rehighlight();
  }
}

void CpptokSyntaxHighlighter::highlightBlock(const QString& text)
{
  int prevstate = previousBlockState();

  int line = currentBlock().blockNumber() + 1;

  if (prevstate != -1)
    m_tokenizer.lexer.state = static_cast<cpptok::Tokenizer::State>(prevstate);

  m_tokenizer.tokenize(text);

  setCurrentBlockState(static_cast<int>(m_tokenizer.lexer.state));

  for (size_t i(0); i < m_tokenizer.lexer.output.size(); ++i)
  {
    const cpptok::Token& tok = m_tokenizer.lexer.output.at(i);

    if (tok.isLiteral())
    {
      if (tok.type() == cpptok::TokenType::StringLiteral)
        setFormat(m_tokenizer.offset(tok), m_tokenizer.length(tok), Format::String);
      else
        setFormat(m_tokenizer.offset(tok), m_tokenizer.length(tok), Format::Literal);
    }
    else if (tok.isKeyword())
    {
      setFormat(m_tokenizer.offset(tok), m_tokenizer.length(tok), Format::Keyword);
    }
    else if (tok.isIdentifier())
    {
      int col = m_tokenizer.col(tok);
      Format f = nameHighlighter().format(*document(), line, col, tok.text());
      setFormat(m_tokenizer.offset(tok), m_tokenizer.length(tok), f);
    }
    else if (tok.type() == cpptok::TokenType::SingleLineComment || tok.type() == cpptok::TokenType::MultiLineComment)
    {
      setFormat(m_tokenizer.offset(tok), m_tokenizer.length(tok), Format::Comment);
    }
    else if (tok.type() == cpptok::TokenType::Preproc)
    {
      setFormat(m_tokenizer.offset(tok), m_tokenizer.length(tok), Format::Macro);
    }
    else if (tok.type() == cpptok::TokenType::Include)
    {
      setFormat(m_tokenizer.offset(tok), m_tokenizer.length(tok), Format::String);
    }
  }
}

QStringLineTokenizer& CpptokSyntaxHighlighter::tokenizer()
{
  return m_tokenizer;
}
