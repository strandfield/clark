// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#pragma once

#include <program/clangindex.h>

#include <QAbstractListModel>

class ClangIndexModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  explicit ClangIndexModel(ClangIndex* index, QObject* parent = nullptr);
  ~ClangIndexModel();

  ClangIndex* clangIndex() const;
  void setClangIndex(ClangIndex* index);

  int columnCount(const QModelIndex& parent = QModelIndex()) const override;
  int rowCount(const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex& index) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

  static QString toString(TranslationUnit::State s);

  TranslationUnit* convert(const QModelIndex& index) const;

protected:
  const std::vector<TranslationUnit*>& translationUnits() const;

private Q_SLOTS:
  void onTranslationUnitsAdded(int n);
  void onTranslationUnitStateChanged();

private:
  void listen(std::vector<TranslationUnit*>::const_iterator begin, std::vector<TranslationUnit*>::const_iterator end);

private:
  ClangIndex* m_clang_index = nullptr;
};
