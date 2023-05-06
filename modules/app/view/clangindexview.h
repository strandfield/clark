// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "model/clangindexmodel.h"

#include <QTreeView>

class ClangIndexView : public QTreeView
{
  Q_OBJECT
public:
  explicit ClangIndexView(QWidget* parent = nullptr);
  explicit ClangIndexView(ClangIndex& cindex, QWidget* parent = nullptr);

  ClangIndexModel* model() const;
  void setModel(ClangIndexModel* model);

Q_SIGNALS:
  void translationUnitDoubleClicked(TranslationUnit* tu);

protected Q_SLOTS:
  void onDoubleClicked(const QModelIndex& index);
};
