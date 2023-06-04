// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "astmodel.h"

#include "resource/iconcache.h"

#include <libclang-utils/clang-translation-unit.h>

#include <QDebug>

void AstModel::CursorDB::clear()
{
  m_nodes.clear();
}

AstModel::Node* AstModel::CursorDB::getPtr(const libclang::Cursor& c, Node* parent)
{
  std::unique_ptr<AstModel::Node>& ptr = m_nodes[c];
  
  if (!ptr)
    ptr = std::make_unique<AstModel::Node>(c, parent);

  return ptr.get();
}

AstModel::AstModel(TranslationUnit* tunit, QObject* parent) : QAbstractItemModel(parent),
  m_db(std::make_unique<CursorDB>()),
  m_icons(std::make_unique<IconCache>())
{
  setTranslationUnit(tunit);
}

AstModel::~AstModel()
{

}

TranslationUnit* AstModel::translationUnit() const
{
  return m_tunit_handle.translationUnit();
}

void AstModel::setTranslationUnit(TranslationUnit* tunit)
{
  if (tunit == translationUnit())
    return;

  beginResetModel();

  m_db->clear();

  m_tunit_handle.reset(tunit);

  if (tunit)
  {
    connect(tunit, &TranslationUnit::aboutToBeDestroyed, this, [this, tunit]() {
      if (translationUnit() == tunit) {
        setTranslationUnit(nullptr);
      }
      });
  }

  endResetModel();
}

libclang::TranslationUnit& AstModel::clangTranslationUnit() const
{
  return m_tunit_handle.clangTranslationunit();
}

int AstModel::columnCount(const QModelIndex& /* parent */) const
{
  return 2;
}

int AstModel::rowCount(const QModelIndex& parent) const
{
  if (!translationUnit())
    return 0;

  if (parent == QModelIndex())
  {
    return int(clangTranslationUnit().getCursor().childCount());
  }
  else
  {
    Node* n = convert(parent);
    return n ? int(n->child_count) : 0;
  }
}

QModelIndex AstModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!translationUnit())
    return QModelIndex();

  if (parent == QModelIndex())
  {
    libclang::Cursor c = clangTranslationUnit().getCursor().childAt(row);
    return createIndex(row, column, m_db->getPtr(c));
  }
  else
  {
    Node* parent_node = convert(parent);

    if (!parent_node)
      return QModelIndex();

    libclang::Cursor c = parent_node->cursor.childAt(row);
    return createIndex(row, column, m_db->getPtr(c, parent_node));
  }
}

QModelIndex AstModel::parent(const QModelIndex& index) const
{
  if (index == QModelIndex() || !translationUnit())
    return QModelIndex();

  Node* n = convert(index);

  if (!n)
    return QModelIndex();

  // We cannot rely on 
  //   n->cursor.getLexicalParent()
  // to get the parent as it currently does not seem to work for statements or expression.
  // Hence the need to use a custom Node struct that stores a pointer to the parent.

  libclang::Cursor parent = n->parent ? n->parent->cursor : clangTranslationUnit().getCursor();

  if (parent == clangTranslationUnit().getCursor())
    return QModelIndex();

  libclang::Cursor grandparent = n->parent->parent ? n->parent->parent->cursor : clangTranslationUnit().getCursor();

  int row = grandparent.indexOfChild(parent);

  if (row == -1)
  {
    qDebug() << "huge problem";
  }

  return createIndex(row, 0, m_db->getPtr(parent));
}

QVariant AstModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole)
  {
    if (section == 0)
      return QString("CursorKind");
    else if (section == 1)
      return QString("DisplayName");
  }

  return QVariant();
}

QVariant AstModel::data(const QModelIndex& index, int role) const
{
  Node* n = convert(index);
 
  if (!n)
    return QVariant();

  const libclang::Cursor& c = n->cursor;

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    if (index.column() == 0)
    {
      return QString::fromStdString(c.getCursorKindSpelling());
    }
    else if (index.column() == 1)
    {
      std::string display_name = c.getDisplayName();
      return QString::fromStdString(display_name);
    }
  }
  else if (index.column() == 0 && role == Qt::DecorationRole)
  {
    return iconForCursor(c);
  }

  return QVariant();
}

AstModel::Node* AstModel::convert(const QModelIndex& index) const
{
  if (index == QModelIndex())
    return nullptr;
  else
    return reinterpret_cast<AstModel::Node*>(index.internalPointer());
}

const QIcon& AstModel::iconForCursor(const libclang::Cursor& c) const
{
  switch (c.kind())
  {
  case CXCursor_ClassDecl:
    return m_icons->icon("symbol-class");
  case CXCursor_StructDecl:
    return m_icons->icon("symbol-struct");
  case CXCursor_FunctionDecl:
  case CXCursor_CXXMethod:
    return m_icons->icon("symbol-method");
  case CXCursor_Namespace:
    return m_icons->icon("symbol-namespace");
  case CXCursor_VarDecl:
    return m_icons->icon("symbol-variable");
  case CXCursor_FieldDecl:
    return m_icons->icon("symbol-field");
  case CXCursor_ParmDecl:
    return m_icons->icon("symbol-parameter");
  case CXCursor_CompoundStmt:
    return m_icons->icon("symbol-namespace");
  default:
    break;
  }

  return m_icons->icon("symbol-misc");
}
