// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "model/astmodel.h"

#include <QTreeView>

class AstView : public QTreeView
{
  Q_OBJECT
public:
  explicit AstView(QWidget* parent = nullptr);
  explicit AstView(TranslationUnit& tunit, QWidget* parent = nullptr);

  AstModel* model() const;
  void setModel(AstModel* model);

Q_SIGNALS:
  void clangCursorDoubleClicked(const libclang::Cursor& c);

protected Q_SLOTS:
  void onDoubleClicked(const QModelIndex& index);
};
