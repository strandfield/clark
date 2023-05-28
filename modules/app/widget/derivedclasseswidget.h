// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include "indexing/indexingresult.h"

#include <QWidget>

#include <map>
#include <vector>

class QComboBox;
class QTreeWidget;
class QTreeWidgetItem;

class TranslationUnitIndexing;

class DerivedClassesWidget : public QWidget
{
  Q_OBJECT
public:
  explicit DerivedClassesWidget(QWidget* parent = nullptr);
  DerivedClassesWidget(TranslationUnitIndexing* idx, QWidget* parent = nullptr);

  TranslationUnitIndexing* indexing() const;
  void setIndexing(TranslationUnitIndexing* idx);

  void clear();

protected:
  void init(const clark::IndexingResult& idx);
  void fillCombobox();
  void fillTree();
  QTreeWidgetItem* createItem(const clark::Entity* ent) const;

private:
  TranslationUnitIndexing* m_indexing = nullptr;
  std::vector<clark::BaseClass> m_bases;
  struct DerivationInfo { size_t offset = 0; size_t count = 0; };
  std::map<const clark::Entity*, DerivationInfo> m_derivation_table;
  std::vector<const clark::Entity*> m_classes;
  QComboBox* m_classes_combobox = nullptr;
  QTreeWidget* m_tree_widget = nullptr;
};
