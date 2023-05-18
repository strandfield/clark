// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <program/translationunit.h>

#include <libclang-utils/clang-cursor.h>

#include <QAbstractItemModel>

#include <memory>
#include <unordered_map>

class IconCache;

/**
 * \brief a tree model for the (clang) ast of a translation unit
 */
class AstModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit AstModel(TranslationUnit* tunit, QObject* parent = nullptr);
  ~AstModel();

  TranslationUnit* translationUnit() const;
  void setTranslationUnit(TranslationUnit* tunit);
  libclang::TranslationUnit& clangTranslationUnit() const;

  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  /**
   * \brief a node in the ast
   * 
   * Note: we cannot fully rely on libclang::Cursor::getLexicalParent() to 
   * get the parent, so we need to define our own Node struct.
   */
  struct Node
  {
    /**
     * \brief the cursor in the clang ast
     */
    libclang::Cursor cursor;

    /**
     * \brief pointer to the parent node
     * 
     * Note: this field is required as cursor.getLexicalParent() is currently 
     * not reliable.
     */
    Node* parent = nullptr;

    /**
     * \brief number of children of the cursor
     * 
     * This field is filled automatically by the constructor and is 
     * used to avoid repetive calls to cursor.childCount().
     */
    size_t child_count;

    explicit Node(const libclang::Cursor& c, Node* p = nullptr) :
      cursor(c),
      parent(p),
      child_count(c.childCount())
    {

    }
  };

  Node* convert(const QModelIndex& index) const;

  class CursorDB
  {
  private:
    std::unordered_map<libclang::Cursor, std::unique_ptr<Node>> m_nodes;

  public:
    void clear();
    Node* getPtr(const libclang::Cursor& c, Node* parent = nullptr);
  };

  const QIcon& iconForCursor(const libclang::Cursor& c) const;

private:
  TranslationUnitHandle m_tunit_handle;
  std::unique_ptr<CursorDB> m_db;
  std::unique_ptr<IconCache> m_icons;
};
