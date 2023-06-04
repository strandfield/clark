// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "derivedclasseswidget.h"

#include <indexing/indexer.h>

#include <QComboBox>
#include <QTreeWidget>

#include <QVBoxLayout>

DerivedClassesWidget::DerivedClassesWidget(QWidget* parent) : DerivedClassesWidget(nullptr, parent)
{

}

DerivedClassesWidget::DerivedClassesWidget(TranslationUnitIndexing* idx, QWidget* parent) : QWidget(parent)
{
  m_classes_combobox = new QComboBox;
  m_tree_widget = new QTreeWidget;

  {
    m_tree_widget->setHeaderHidden(true);
  }

  auto* layout = new QVBoxLayout;
  {
    layout->addWidget(m_classes_combobox);
    layout->addWidget(m_tree_widget);
  }
  setLayout(layout);

  connect(m_classes_combobox, qOverload<int>(&QComboBox::currentIndexChanged), this, &DerivedClassesWidget::fillTree);

  setIndexing(idx);
}

TranslationUnitIndexing* DerivedClassesWidget::indexing() const
{
  return m_indexing;
}

void DerivedClassesWidget::setIndexing(TranslationUnitIndexing* idx)
{
  if (m_indexing == idx)
    return;

  m_derivation_table.clear();
  m_bases.clear();
  m_classes.clear();

  m_indexing = idx;

  if (m_indexing)
  {
    init(idx->indexingResult());
    connect(m_indexing, &QObject::destroyed, this, &DerivedClassesWidget::clear);
  }

  fillCombobox();
  fillTree();
}

void DerivedClassesWidget::clear()
{
  setIndexing(nullptr);
}

void DerivedClassesWidget::init(const clark::IndexingResult& idx)
{
  m_bases = idx.bases;

  std::sort(m_bases.begin(), m_bases.end(), [](const clark::BaseClass& lhs, const clark::BaseClass& rhs) {
    return lhs.base < rhs.base;
    });

  m_derivation_table.clear();

  auto it = m_bases.cbegin();
  auto find_next = [this](std::vector<clark::BaseClass>::const_iterator iterator) {
    return std::find_if(iterator, m_bases.cend(), [iterator](const clark::BaseClass& bcla) {
      return bcla.base != iterator->base;
      });
  };

  while (it != m_bases.cend())
  {
    auto end = find_next(it);
    DerivationInfo info;
    info.offset = std::distance(m_bases.cbegin(), it);
    info.count = std::distance(it, end);
    assert(info.count > 0);
    m_derivation_table[it->base] = info;
    it = end;
  }

  m_classes.clear();
  m_classes.reserve(m_derivation_table.size());

  for (const auto& p : m_derivation_table)
  {
    m_classes.push_back(p.first);
  }

  std::sort(m_classes.begin(), m_classes.end(), [](const clark::Entity* lhs, const clark::Entity* rhs) {
    return lhs->display_name < rhs->display_name;
    });
}

void DerivedClassesWidget::fillCombobox()
{
  m_classes_combobox->clear();

  for (const clark::Entity* ent : m_classes)
  {
    m_classes_combobox->addItem(QString::fromStdString(ent->display_name));
  }
}

void DerivedClassesWidget::fillTree()
{
  m_tree_widget->clear();

  if (m_classes_combobox->currentIndex() < 0 || m_classes_combobox->currentIndex() >= (int)m_classes.size())
    return;

  const clark::Entity* base = m_classes.at(m_classes_combobox->currentIndex());
  const DerivationInfo& info = m_derivation_table[base];

  for (size_t i(0); i < info.count; ++i)
  {
    const clark::BaseClass& derived = m_bases.at(info.offset + i);
    QTreeWidgetItem* item = createItem(derived.derived);
    m_tree_widget->addTopLevelItem(item);
  }
}

QTreeWidgetItem* DerivedClassesWidget::createItem(const clark::Entity* ent) const
{
  auto* result = new QTreeWidgetItem;
  result->setText(0, QString::fromStdString(ent->display_name));

  auto it = m_derivation_table.find(ent);

  if (it != m_derivation_table.end())
  {
    const DerivationInfo& info = it->second;

    for (size_t i(0); i < info.count; ++i)
    {
      const clark::BaseClass& derived = m_bases.at(info.offset + i);
      QTreeWidgetItem* item = createItem(derived.derived);
      result->addChild(item);
    }
  }

  return result;
}
