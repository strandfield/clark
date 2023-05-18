// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "tunameresolver.h"
 
#include <libclang-utils/clang-cursor.h>
#include <libclang-utils/clang-source-location.h>
#include <libclang-utils/clang-translation-unit.h>

#include <QTextDocument>

#include <QDebug>

TranslationUnitNameResolver::TranslationUnitNameResolver(TranslationUnitHandle handle, const QTextDocument& document) :
  m_handle(handle)
{
  QString filepath = document.metaInformation(QTextDocument::DocumentUrl);
  const libclang::TranslationUnit& tu = m_handle.clangTranslationunit();
  m_file = std::make_unique<libclang::File>(tu.getFile(filepath.toStdString()));
}

SyntaxHighlighter::Format TranslationUnitNameResolver::resolve(const QTextDocument& document, int line, int col, const cpptok::Token& tok)
{
  const libclang::TranslationUnit& tu = m_handle.clangTranslationunit();
  libclang::SourceLocation loc = tu.getLocation(*m_file, line, col);
  libclang::Cursor c = tu.getCursor(loc);

  if (c.isNull())
    return SyntaxHighlighter::Default;

  // $todo: the following needs some improvements

  if (c.isReference())
    c = c.getReference();

  c = c.getCanonical();

  switch (c.kind())
  {
  case CXCursor_ClassDecl:
  case CXCursor_StructDecl:
  case CXCursor_TypedefDecl:
  case CXCursor_TypeAliasDecl:
  case CXCursor_EnumDecl:
  case CXCursor_EnumConstantDecl:
    return SyntaxHighlighter::Format::Typename;
  case CXCursor_Namespace:
    return SyntaxHighlighter::Format::NamespaceName;
  case CXCursor_VarDecl:
    return SyntaxHighlighter::Format::MemberName;
  case CXCursor_FunctionDecl:
  case CXCursor_CXXMethod:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
    return SyntaxHighlighter::Format::Function;
  case CXCursor_MemberRefExpr:
    return SyntaxHighlighter::Format::Function;
  default:
    //qDebug() << c.getCursorKindSpelling().c_str() << ": " << c.getDisplayName().c_str();
    return SyntaxHighlighter::Format::Default;
  }
}
