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

CppSyntaxHighlighter::Format TranslationUnitNameResolver::resolve(const QTextDocument& document, int line, int col, const cpptok::Token& tok)
{
  const libclang::TranslationUnit& tu = m_handle.clangTranslationunit();
  libclang::SourceLocation loc = tu.getLocation(*m_file, line, col);
  libclang::Cursor c = tu.getCursor(loc);

  if (c.isNull())
    return CppSyntaxHighlighter::Default;

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
    return CppSyntaxHighlighter::Format::Typename;
  case CXCursor_Namespace:
    return CppSyntaxHighlighter::Format::NamespaceName;
  case CXCursor_VarDecl:
    return CppSyntaxHighlighter::Format::MemberName;
  case CXCursor_FunctionDecl:
  case CXCursor_CXXMethod:
  case CXCursor_Constructor:
  case CXCursor_Destructor:
    return CppSyntaxHighlighter::Format::Function;
  case CXCursor_MemberRefExpr:
    return CppSyntaxHighlighter::Format::Function;
  default:
    //qDebug() << c.getCursorKindSpelling().c_str() << ": " << c.getDisplayName().c_str();
    return CppSyntaxHighlighter::Format::Default;
  }
}
