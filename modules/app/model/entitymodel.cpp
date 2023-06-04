// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "entitymodel.h"

#include "resource/iconcache.h"

#include <libclang-utils/clang-translation-unit.h>

#include <QtConcurrent>

#include <QDebug>


static std::shared_ptr<EntityModel::Tree> build_entitymodel_tree(const clark::IndexingResult* index)
{
  return std::make_shared<EntityModel::Tree>(index->symbols);
}


EntityModel::Tree::Tree()
{
  m_nodes.emplace_back();
  m_nodes.back().child_count = 0;
}

EntityModel::Tree::Tree(const std::map<clark::USR, std::unique_ptr<clark::Entity>>& entities)
{
  m_nodes.reserve(entities.size() + 1);
  m_nodes.emplace_back();

  for (const auto& p : entities)
  {
    m_nodes.emplace_back();
    m_nodes.back().entity = p.second.get();
  }

  std::sort(m_nodes.begin() + 1, m_nodes.end(), [](const Node& lhs, const Node& rhs) {
    return lhs.entity->parent < rhs.entity->parent;
    });

  std::map<clark::Entity*, size_t> entpos;

  for (size_t i(0); i < m_nodes.size(); ++i)
  {
    entpos[m_nodes.at(i).entity] = i;
  }

  for (size_t i(1); i < m_nodes.size(); ++i)
  {
    Node& n = m_nodes[i];

    auto it = entpos.find(n.entity->parent);
    assert(it != entpos.end());

    n.parent_offset = it->second;

    Node& parent = m_nodes[n.parent_offset];

    if (parent.first_child_offset == std::numeric_limits<size_t>::max())
    {
      parent.first_child_offset = i;

      while (i + parent.child_count < m_nodes.size() && m_nodes.at(i + parent.child_count).entity->parent == parent.entity)
        parent.child_count += 1;
    }
  }
}

EntityModel::Node* EntityModel::Tree::root()
{
  return node(0);
}

const std::vector<EntityModel::Node>& EntityModel::Tree::nodes() const
{
  return m_nodes;
}

EntityModel::Node* EntityModel::Tree::node(size_t index) 
{
  return &(m_nodes[index]);
}

EntityModel::EntityModel(TranslationUnitIndexing* idx, QObject* parent) : QAbstractItemModel(parent),
  m_indexing(idx),
  m_icons(std::make_unique<IconCache>())
{
  if (idx)
  {
    connect(idx, &TranslationUnitIndexing::destroyed, this, &EntityModel::resetTree);

    if (!idx->isReady())
    {
      connect(idx, &TranslationUnitIndexing::ready, this, &EntityModel::computeTree);
    }
    else
    {
      computeTree();
    }
  }
}

EntityModel::~EntityModel()
{

}

TranslationUnitIndexing* EntityModel::translationUnitIndexing() const
{
  return m_indexing;
}

int EntityModel::columnCount(const QModelIndex& /* parent */) const
{
  return 1;
}

int EntityModel::rowCount(const QModelIndex& parent) const
{
  if (!tree())
    return 0;

  if (parent == QModelIndex())
  {
    return int(tree()->root()->child_count);
  }
  else
  {
    Node* n = convert(parent);
    return n ? int(n->child_count) : 0;
  }
}

QModelIndex EntityModel::index(int row, int column, const QModelIndex& parent) const
{
  if (!tree())
    return QModelIndex();

  const Node* parent_node = parent == QModelIndex() ? tree()->root() : convert(parent);

  assert(parent_node); if (!parent_node)
    return QModelIndex();

  return createIndex(row, column, tree()->node(parent_node->first_child_offset + row));
}

QModelIndex EntityModel::parent(const QModelIndex& index) const
{
  if (index == QModelIndex() || !tree())
    return QModelIndex();

  Node* n = convert(index);

  if (!n)
    return QModelIndex();

  Node* parent_node = tree()->node(n->parent_offset);

  if (parent_node == tree()->root())
    return QModelIndex();

  Node* ancestor_node = tree()->node(parent_node->parent_offset);

  auto it = std::find_if(
    tree()->nodes().begin() + ancestor_node->first_child_offset,
    tree()->nodes().begin() + ancestor_node->first_child_offset + ancestor_node->child_count,
    [parent_node](const Node& treenode) {
      return treenode.entity == parent_node->entity;
    }
  );

  size_t offset = std::distance(tree()->nodes().begin() + ancestor_node->first_child_offset, it);

  return createIndex(int(offset), 0, parent_node);
}

QVariant EntityModel::data(const QModelIndex& index, int role) const
{
  Node* n = convert(index);
 
  if (!n)
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    return QString::fromStdString(n->entity->display_name);
  }
  else if (index.column() == 0 && role == Qt::DecorationRole)
  {
    return iconForEntity(n->entity);
  }

  return QVariant();
}

EntityModel::Node* EntityModel::convert(const QModelIndex& index) const
{
  if (index == QModelIndex())
    return nullptr;
  else
    return reinterpret_cast<EntityModel::Node*>(index.internalPointer());
}

const QIcon& EntityModel::iconForEntity(const clark::Entity* entity) const
{
  switch (entity->kind)
  {
  case clark::Whatsit::Unexposed:
    break;
  case clark::Whatsit::Typedef:
    break;
  case clark::Whatsit::Function:
    return m_icons->icon("symbol-method");
  case clark::Whatsit::Variable:
    return m_icons->icon("symbol-variable");
  case clark::Whatsit::Field:
    return m_icons->icon("symbol-field");
  case clark::Whatsit::EnumConstant:
    break;
  case clark::Whatsit::ObjCClass:
    return m_icons->icon("symbol-class");
  case clark::Whatsit::ObjCProtocol:
  case clark::Whatsit::ObjCCategory:
    break;
  case clark::Whatsit::ObjCInstanceMethod:
  case clark::Whatsit::ObjCClassMethod:
    return m_icons->icon("symbol-method");
  case clark::Whatsit::ObjCProperty:
  case clark::Whatsit::ObjCIvar:
    break;
  case clark::Whatsit::Enum:
  case clark::Whatsit::Struct:
  case clark::Whatsit::Union:
    return m_icons->icon("symbol-struct");
  case clark::Whatsit::CXXClass:
    return m_icons->icon("symbol-class");
  case clark::Whatsit::CXXNamespace:
  case clark::Whatsit::CXXNamespaceAlias:
    return m_icons->icon("symbol-namespace");
  case clark::Whatsit::CXXStaticVariable:
    return m_icons->icon("symbol-variable");
  case clark::Whatsit::CXXStaticMethod:
  case clark::Whatsit::CXXInstanceMethod:
    return m_icons->icon("symbol-method");
  case clark::Whatsit::CXXConstructor:
  case clark::Whatsit::CXXDestructor:
  case clark::Whatsit::CXXConversionFunction:
    return m_icons->icon("symbol-method");
  case clark::Whatsit::CXXTypeAlias:
  case clark::Whatsit::CXXInterface:
    break;
  default:
    break;
  }

  return m_icons->icon("symbol-misc");
}

void EntityModel::setTree(std::unique_ptr<Tree> t)
{
  beginResetModel();

  m_tree = std::move(t);

  endResetModel();
}

EntityModel::Tree* EntityModel::tree() const
{
  return m_tree.get();
}

void EntityModel::computeTree()
{
  QFuture<TreeSharedPtr> future_tree = QtConcurrent::run(build_entitymodel_tree, &translationUnitIndexing()->indexingResult());
  auto watcher = new QFutureWatcher<TreeSharedPtr>(this);
  connect(watcher, &QFutureWatcher<TreeSharedPtr>::finished, this, &EntityModel::onTreeReady);
  watcher->setFuture(future_tree);
}

void EntityModel::onTreeReady()
{
  auto* watcherbase = qobject_cast<QFutureWatcherBase*>(sender());
  auto* watcher = dynamic_cast<QFutureWatcher<TreeSharedPtr>*>(watcherbase);

  if (!watcher) return;

  TreeSharedPtr result = watcher->result();

  if (!result) return;

  setTree(std::make_unique<Tree>(*result));

  watcher->deleteLater();
}

void EntityModel::resetTree()
{
  setTree(nullptr);
}
