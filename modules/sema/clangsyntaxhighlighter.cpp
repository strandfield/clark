// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "clangsyntaxhighlighter.h"
 
#include <libclang-utils/annotatetokens.h>
#include <libclang-utils/clang-cursor.h>
#include <libclang-utils/clang-source-location.h>
#include <libclang-utils/clang-translation-unit.h>

#include <QTextDocument>

#include <QDebug>

#include <cstring>

ClangSyntaxHighlighter::ClangSyntaxHighlighter(TranslationUnitHandle thandle, const libclang::File& file, QTextDocument* document) 
  : CppSyntaxHighlighter(document),
  m_thandle(thandle),
  m_file(file)
{

}

CppSyntaxHighlighter::Format format4DeclRefExpr(const libclang::Cursor& cursor)
{
  libclang::Cursor c = cursor;

  c = c.getDefinition();

  switch (c.kind())
  {
  case CXCursor_ClassDecl:
  case CXCursor_StructDecl:
  case CXCursor_TypedefDecl:
  case CXCursor_TypeAliasDecl:
  case CXCursor_EnumDecl:
  case CXCursor_EnumConstantDecl:
  case CXCursor_TemplateRef:
  case CXCursor_TypeRef:
    return CppSyntaxHighlighter::Format::Typename;
  case CXCursor_Namespace:
  case CXCursor_NamespaceRef:
    return CppSyntaxHighlighter::Format::NamespaceName;
  case CXCursor_VarDecl:
  case CXCursor_FieldDecl:
    return CppSyntaxHighlighter::Format::MemberName;
  case CXCursor_FunctionDecl:
  case CXCursor_CXXMethod:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
    return CppSyntaxHighlighter::Format::Function;
  default:
    return CppSyntaxHighlighter::Format::Default;
  }
}

CppSyntaxHighlighter::Format ClangSyntaxHighlighter::format4cursor(const libclang::Cursor& cursor)
{
  if (cursor.isNull())
    return CppSyntaxHighlighter::Default;

  libclang::Cursor c = cursor;

  switch (c.kind())
  {
  case CXCursor_PreprocessingDirective:
  case CXCursor_MacroDefinition:
  case CXCursor_MacroExpansion:
  case CXCursor_InclusionDirective:
    return CppSyntaxHighlighter::Format::Preproc;
  case CXCursor_ClassDecl:
  case CXCursor_StructDecl:
  case CXCursor_TypedefDecl:
  case CXCursor_TypeAliasDecl:
  case CXCursor_EnumDecl:
  case CXCursor_EnumConstantDecl:
  case CXCursor_TemplateRef:
  case CXCursor_TypeRef:
    return CppSyntaxHighlighter::Format::Typename;
  case CXCursor_Namespace:
  case CXCursor_NamespaceRef:
    return CppSyntaxHighlighter::Format::NamespaceName;
  case CXCursor_VarDecl:
  case CXCursor_FieldDecl:
  case CXCursor_MemberRef:
    return CppSyntaxHighlighter::Format::MemberName;
  case CXCursor_FunctionDecl:
  case CXCursor_CXXMethod:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
    return CppSyntaxHighlighter::Format::Function;
  case CXCursor_MemberRefExpr:
    return CppSyntaxHighlighter::Format::Function;
  case CXCursor_DeclRefExpr:
    return format4DeclRefExpr(cursor);
  default:
    //qDebug() << c.getCursorKindSpelling().c_str() << ": " << c.getDisplayName().c_str();
    return CppSyntaxHighlighter::Format::Default;
  }
}

static bool has_any_identifier(const libclang::TokenSet& tokens)
{
  for (size_t i(0); i < tokens.size(); ++i)
  {
    if (tokens.at(i).getKind() == CXToken_Identifier)
      return true;
  }

  return false;
}

CppSyntaxHighlighter::Format ClangSyntaxHighlighter::format4token(const libclang::Token& t)
{
  switch (t.getKind())
  {
  case CXToken_Keyword:
    return Format::Keyword;
  case CXToken_Literal:
    return Format::Literal;
  case CXToken_Comment:
    return Format::Comment;
  default:
    return Format::Default;
  }
}

void ClangSyntaxHighlighter::highlightBlock(const QString& text)
{
  int prevstate = previousBlockState();

  int col = 1;

  if (prevstate == ST_Comment)
  {
    int i = text.indexOf("*/");

    if (i == -1)
    {
      setFormat(0, text.size(), Format::Comment);
      setCurrentBlockState(ST_Comment);
      return;
    }
    else
    {
      setFormat(0, i+2, Format::Comment);
      col = i + 2 + 1;

      if (col >= text.size())
      {
        setCurrentBlockState(ST_Default);
        return;
      }
    }
  }

  int line = currentBlock().blockNumber() + 1;

  auto& tu = m_thandle.clangTranslationunit();
  libclang::SourceLocation start = tu.getLocation(m_file, line, col);
  libclang::SourceLocation end = tu.getLocation(m_file, line, text.size());
  libclang::SourceRange range = libclang::getRange(start, end);
  libclang::TokenSet tokens = tu.tokenize(range);

  bool has_identifier = has_any_identifier(tokens);

  if (has_identifier)
  {
    libclang::annotateTokens(tu, tokens, [&](const libclang::Token& tok, const libclang::Cursor& c) {
      int col = tok.getLocation().getSpellingLocation().col - 1;
      std::string spelling = tok.getSpelling();
      int len = spelling.size();

      if (tok.getKind() == CXToken_Identifier)
      {
        setFormat(col, len, format4cursor(c));
      }
      else
      {
        setFormat(col, len, format4token(tok));
      }
      });
  }
  else
  {
    for (size_t i(0); i < tokens.size(); ++i)
    {
      libclang::Token tok = tokens.at(i);
      setFormat(tok.getLocation().getSpellingLocation().col - 1, tok.getSpelling().size(), format4token(tok));
    }
  }

  if (!tokens.empty())
  {
    setCurrentBlockStateFromToken(tokens.at(tokens.size() - 1));
  }
}

void ClangSyntaxHighlighter::setCurrentBlockStateFromToken(const libclang::Token& t)
{
  if (t.getKind() == CXToken_Comment)
  {
    std::string spelling = t.getSpelling();
    bool is_multiline = std::strncmp(spelling.c_str(), "/*", 2) == 0;

    if (is_multiline)
    {
      bool ends_in_this_line = spelling.find("\n") == std::string::npos 
        && std::strcmp(spelling.c_str() + (spelling.size() - 2), "*/") == 0;

      if (ends_in_this_line)
      {
        /* multiline comment is in fact on a single line */
        setCurrentBlockState(ST_Default);
      }
      else
      {
        /* we are in a multiline comment that does not end 
           at this line, we leave the line in the ST_Comment state */
        setCurrentBlockState(ST_Comment);
      }
    }
    else
    {
      // the comment ends at the end of the line, 
      // so we do not leave the line in the ST_Comment state
      setCurrentBlockState(ST_Default);
    }
  }
  else
  {
    setCurrentBlockState(ST_Default);
  }
}
