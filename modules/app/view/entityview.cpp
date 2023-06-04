// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "entityview.h"

EntityView::EntityView(TranslationUnitIndexing* idx, QWidget* parent) : QTreeView(parent)
{
  setHeaderHidden(true);

  setSelectionMode(QAbstractItemView::SingleSelection);

  setUniformRowHeights(true);

  setModel(new EntityModel(idx, this));

  connect(this, &QAbstractItemView::doubleClicked, this, &EntityView::onDoubleClicked);
}

EntityModel* EntityView::model() const
{
  return static_cast<EntityModel*>(QTreeView::model());
}

void EntityView::setModel(EntityModel* model)
{
  QTreeView::setModel(model);
}

void EntityView::onDoubleClicked(const QModelIndex& index)
{
  EntityModel::Node* n = model()->convert(index);

  if (n)
  {
    Q_EMIT entityDoubleClicked(n->entity);
  }
}
