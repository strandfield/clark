// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "program/translationunit.h"

#include "indexing/indexer.h"

#include <QAbstractItemModel>

#include <memory>
#include <unordered_map>

class IconCache;

class EntityModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  EntityModel(TranslationUnitIndexing* idx, QObject* parent = nullptr);
  ~EntityModel();

  TranslationUnitIndexing* translationUnitIndexing() const;

  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  const QIcon& iconForEntity(const clark::Entity* entity) const;

  struct Node
  {
    clark::Entity* entity = nullptr;
    size_t parent_offset = std::numeric_limits<size_t>::max();
    size_t first_child_offset = std::numeric_limits<size_t>::max();
    size_t child_count = 0;
  };

  Node* convert(const QModelIndex& index) const;

  class Tree
  {
  private:
    std::vector<Node> m_nodes;

  public:
    Tree();
    Tree(const Tree&) = default;
    ~Tree() = default;

    explicit Tree(const std::map<clark::USR, std::unique_ptr<clark::Entity>>& entities);
    
    Node* root();
    const std::vector<Node>& nodes() const;
    Node* node(size_t index);
  };

  using TreeSharedPtr = std::shared_ptr<Tree>;

protected:
  void setTree(std::unique_ptr<Tree> t);
  Tree* tree() const;

protected Q_SLOTS:
  void computeTree();
  void onTreeReady();
  void resetTree();

private:
  TranslationUnitIndexing* m_indexing;
  std::unique_ptr<IconCache> m_icons;
  std::unique_ptr<Tree> m_tree;
};
