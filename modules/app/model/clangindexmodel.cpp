// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "clangindexmodel.h"

#include <QFont>
#include <QSize>

ClangIndexModel::ClangIndexModel(ClangIndex* index, QObject* parent) : QAbstractItemModel(parent)
{
  setClangIndex(index);
}

ClangIndexModel::~ClangIndexModel()
{

}

ClangIndex* ClangIndexModel::clangIndex() const
{
  return m_clang_index;
}

void ClangIndexModel::setClangIndex(ClangIndex* index)
{
  if (index == m_clang_index)
    return;

  if (m_clang_index)
    disconnect(m_clang_index, nullptr, this, nullptr);

  beginResetModel();

  m_clang_index = index;

  if (m_clang_index)
  {
    connect(m_clang_index, &ClangIndex::translationUnitsAdded, this, &ClangIndexModel::onTranslationUnitsAdded);

    connect(m_clang_index, &QObject::destroyed, this, [this]() {
      setClangIndex(nullptr);
      });
 
    listen(m_clang_index->translationUnits().begin(), m_clang_index->translationUnits().end());
  }

  endResetModel();
}

int ClangIndexModel::columnCount(const QModelIndex& /* parent */) const
{
  return 2;
}

int ClangIndexModel::rowCount(const QModelIndex& parent) const
{
  return (parent == QModelIndex() && clangIndex()) ? (int)clangIndex()->translationUnits().size() : 0;
}

QModelIndex ClangIndexModel::index(int row, int column, const QModelIndex& parent) const
{
  if (parent != QModelIndex())
    return QModelIndex();

  return createIndex(row, column, clangIndex()->translationUnits().at(row));
}

QModelIndex ClangIndexModel::parent(const QModelIndex& index) const
{
  return QModelIndex();
}

QVariant ClangIndexModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::SizeHintRole)
  {
    return QSize(50, 20);
  }
  else if (role == Qt::FontRole)
  {
    return QFont();
  }
  else if (role == Qt::DisplayRole)
  {
    if (section == 0)
    {
      return QString("Path");
    }
    else if (section == 1)
    {
      return QString("State");
    }
    else
    {
      return QVariant();
    }
  }
  else
  {
    return QVariant();
  }
}

QVariant ClangIndexModel::data(const QModelIndex& index, int role) const
{
  TranslationUnit* tu = convert(index);
  int col = index.column();

  if (!tu)
    return QVariant();

  if (role == Qt::SizeHintRole)
  {
    return QSize(50, 20);
  }
  else if (role == Qt::FontRole)
  {
    return QFont();
  }

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    if (col == 0)
    {
      return tu->filePath();
    }
    else if (col == 1)
    {
      return toString(tu->state());
    }
  }

  return QVariant();
}

QString ClangIndexModel::toString(TranslationUnit::State s)
{
  switch (s)
  {
  case TranslationUnit::State::AwaitingParsing:
    return "Awaiting parsing";
  case TranslationUnit::State::Loaded:
    return "Loaded";
  case TranslationUnit::State::Parsing:
    return "Parsing";
  case TranslationUnit::State::Suspended:
    return "Suspended";
  default:
    return {};
  }
}

TranslationUnit* ClangIndexModel::convert(const QModelIndex& index) const
{
  if (index == QModelIndex())
    return nullptr;
  else
    return reinterpret_cast<TranslationUnit*>(index.internalPointer());
}

const std::vector<TranslationUnit*>& ClangIndexModel::translationUnits() const
{
  return m_clang_index->translationUnits();
}

void ClangIndexModel::onTranslationUnitsAdded(int n)
{
  beginInsertRows(QModelIndex(), (int)m_clang_index->translationUnits().size() - n, (int)m_clang_index->translationUnits().size() - 1);
  listen(m_clang_index->translationUnits().end() - n, m_clang_index->translationUnits().end());
  endInsertRows();
}

void ClangIndexModel::onTranslationUnitStateChanged()
{
  auto* tu = qobject_cast<TranslationUnit*>(sender());

  if (!tu)
    return;

  auto it = std::find(translationUnits().begin(), translationUnits().end(), tu);

  if (it == translationUnits().end())
    return;

  int n = std::distance(translationUnits().begin(), it);

  QModelIndex index{ createIndex(n, 1, tu) };
  Q_EMIT dataChanged(index, index);
}

void ClangIndexModel::listen(std::vector<TranslationUnit*>::const_iterator begin, std::vector<TranslationUnit*>::const_iterator end)
{
  for (auto it = begin; it != end; ++it)
  {
    TranslationUnit* tu = *it;
    connect(tu, &TranslationUnit::stateChanged, this, &ClangIndexModel::onTranslationUnitStateChanged);
  }
}
