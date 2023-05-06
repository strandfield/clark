// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "clangindexview.h"

#include <QHeaderView>

ClangIndexView::ClangIndexView(QWidget* parent) : QTreeView(parent)
{
  Q_ASSERT(header());
  Q_ASSERT(!isHeaderHidden());

  setModel(new ClangIndexModel(nullptr, this));

  connect(this, &QAbstractItemView::doubleClicked, this, &ClangIndexView::onDoubleClicked);
}

ClangIndexView::ClangIndexView(ClangIndex& cindex, QWidget* parent) : ClangIndexView(parent)
{
  model()->setClangIndex(&cindex);
}

ClangIndexModel* ClangIndexView::model() const
{
  return static_cast<ClangIndexModel*>(QTreeView::model());
}

void ClangIndexView::setModel(ClangIndexModel* model)
{
  QTreeView::setModel(model);
}

void ClangIndexView::onDoubleClicked(const QModelIndex& index)
{
  TranslationUnit* tu = model()->convert(index);

  if (tu)
  {
    Q_EMIT translationUnitDoubleClicked(tu);
  }
}
