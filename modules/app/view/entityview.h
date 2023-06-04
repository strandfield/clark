// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "model/entitymodel.h"

#include <QTreeView>

class EntityView : public QTreeView
{
  Q_OBJECT
public:
  explicit EntityView(TranslationUnitIndexing* idx, QWidget* parent = nullptr);

  EntityModel* model() const;
  void setModel(EntityModel* model);

Q_SIGNALS:
  void entityDoubleClicked(const clark::Entity* entity);

protected Q_SLOTS:
  void onDoubleClicked(const QModelIndex& index);
};
