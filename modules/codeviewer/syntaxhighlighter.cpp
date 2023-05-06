// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "syntaxhighlighter.h"

#include <QTextDocument>

#include <set>

SyntaxHighlighter::SyntaxHighlighter(QTextDocument* document) : QSyntaxHighlighter(document)
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

  setNameResolver(new SyntaxHighlighterNameResolver(this));
}

SyntaxHighlighter::~SyntaxHighlighter()
{

}

SyntaxHighlighterNameResolver& SyntaxHighlighter::nameResolver() const
{
  return *m_name_resolver;
}

void SyntaxHighlighter::setNameResolver(SyntaxHighlighterNameResolver* nameresolver)
{
  if (nameresolver)
  {
    if(m_name_resolver)
      m_name_resolver->deleteLater();

    m_name_resolver = nameresolver;
    m_name_resolver->setParent(this);
    connect(nameresolver, &SyntaxHighlighterNameResolver::update, this, &QSyntaxHighlighter::rehighlight);
    connect(nameresolver, &SyntaxHighlighterNameResolver::updateBlock, this, &QSyntaxHighlighter::rehighlightBlock);
    rehighlight();
  }
}

void SyntaxHighlighter::highlightBlock(const QString& text)
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
      Format f = nameResolver().resolve(*document(), line, col, tok);
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

void SyntaxHighlighter::initFormat(Format fmt, const QTextCharFormat& value)
{
  m_formats[static_cast<size_t>(fmt)] = value;
}

void SyntaxHighlighter::setFormat(int start, int count, Format fmt)
{
  QSyntaxHighlighter::setFormat(start, count, m_formats.at(fmt));
}

const std::vector<QTextCharFormat>& SyntaxHighlighter::formats() const
{
  return m_formats;
}

QStringLineTokenizer& SyntaxHighlighter::tokenizer()
{
  return m_tokenizer;
}

SyntaxHighlighterNameResolver::SyntaxHighlighterNameResolver(QObject* parent) : QObject(parent)
{

}

SyntaxHighlighterNameResolver::~SyntaxHighlighterNameResolver()
{

}

SyntaxHighlighter::Format SyntaxHighlighterNameResolver::resolve(const QTextDocument& document, int line, int col, const cpptok::Token& tok)
{
  static const std::set<std::string> known_namespaces = {
    "std", "Eigen", "Poco", "Qt"
  };

  static const std::set<std::string> known_types = {

  };

  {
    auto it = known_namespaces.find(std::string(tok.text()));

    if (it != known_namespaces.end())
      return SyntaxHighlighter::Format::NamespaceName;
  }

  {
    auto it = known_types.find(std::string(tok.text()));

    if (it != known_types.end())
      return SyntaxHighlighter::Format::Typename;
  }

  // Check if token is immediately followed by "::"
  if (std::strncmp(tok.text().data() + tok.text().length(), "::", 2) == 0)
  {
    return SyntaxHighlighter::Format::Typename;
  }

  return SyntaxHighlighter::Format::Default;
}
