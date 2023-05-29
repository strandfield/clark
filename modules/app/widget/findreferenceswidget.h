// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "indexing/indexingresult.h"

#include <QWidget>

#include <QFutureWatcher>
#include <QList>

#include <map>
#include <vector>

class QComboBox;
class QTreeWidget;
class QTreeWidgetItem;

class TranslationUnitIndexing;

class FindReferencesWidget : public QWidget
{
  Q_OBJECT
public:
  explicit FindReferencesWidget(QWidget* parent = nullptr);
  FindReferencesWidget(TranslationUnitIndexing* idx, const clark::Entity* e, QWidget* parent = nullptr);
  ~FindReferencesWidget();

  TranslationUnitIndexing* indexing() const;
  void setIndexing(TranslationUnitIndexing* idx);

  const clark::Entity* entity() const;
  void setEntity(const clark::Entity* e);

  void clear();

Q_SIGNALS:
  void referenceClicked(const QString& documentPath, int line);

protected Q_SLOTS:
  void computeReferences();
  void onReferencesComputationFinished();
  void onTreeItemDoubleClicked(QTreeWidgetItem* item);

protected:
  void fillTree(const QList<clark::EntityReference>& references);
  QTreeWidgetItem* createItem(const clark::EntityReference& eref, const QString& linetext) const;

private:
  TranslationUnitIndexing* m_indexing = nullptr;
  const clark::Entity* m_entity = nullptr;
  QFutureWatcher<QList<clark::EntityReference>>* m_references_future = nullptr;
  QTreeWidget* m_tree_widget = nullptr;
};
