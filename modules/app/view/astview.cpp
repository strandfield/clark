// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "astview.h"


AstView::AstView(QWidget* parent) : QTreeView(parent)
{
  setHeaderHidden(false);

  setSelectionMode(QAbstractItemView::SingleSelection);

  setUniformRowHeights(true);

  setModel(new AstModel(nullptr, this));

  connect(this, &QAbstractItemView::doubleClicked, this, &AstView::onDoubleClicked);
}

AstView::AstView(TranslationUnit& tunit, QWidget* parent) : AstView(parent)
{
  model()->setTranslationUnit(&tunit);
}

AstModel* AstView::model() const
{
  return static_cast<AstModel*>(QTreeView::model());
}

void AstView::setModel(AstModel* model)
{
  QTreeView::setModel(model);
}

void AstView::onDoubleClicked(const QModelIndex& index)
{
  AstModel::Node* n = model()->convert(index);

  if (n)
  {
    const libclang::Cursor& c = n->cursor;
    Q_EMIT clangCursorDoubleClicked(c);

  }
}
