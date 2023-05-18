// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_OPENSLNDIALOG_WIDGETS_H
#define CLARK_OPENSLNDIALOG_WIDGETS_H

#include <vcxproj/solution.h>

#include <QListWidget>

#include <optional>

class SlnProjectListWidget : public QListWidget
{
  Q_OBJECT
public:
  explicit SlnProjectListWidget(const vcxproj::Solution& s, QWidget* parent = nullptr);

  std::optional<std::string> selection() const;

Q_SIGNALS:
  void selectionChanged();

private:
  void buildItems(const vcxproj::Solution& solution);

private:
  const vcxproj::Solution& m_solution;
};

class SlnProjectConfigurationListWidget : public QListWidget
{
  Q_OBJECT
public:
  explicit SlnProjectConfigurationListWidget(QWidget* parent = nullptr);

  std::optional<std::string> selection() const;

  void setProject(const vcxproj::Project* project);

Q_SIGNALS:
  void selectionChanged();

private:
  void buildItems(const vcxproj::Project& project);
};

class SlnCompileListWidget : public QListWidget
{
  Q_OBJECT
public:
  explicit SlnCompileListWidget(const vcxproj::Solution& s, QWidget* parent = nullptr);

  const QString& projectFilter() const;
  void setProjectFilter(const QString& f);

  const QString& projectConfigurationFilter() const;
  void setProjectConfigurationFilter(const QString& f);

  struct Item
  {
    QString project;
    QString projectConfiguration;
    QString compile; // the file
  };

  std::optional<Item> selection() const;

Q_SIGNALS:
  void selectionChanged();

private:
  void buildItems(const vcxproj::Solution& solution);
  static QListWidgetItem* createListItem(const Item& item);
  static Item toItem(QListWidgetItem* listitem);
  void updateFilters();

private:
  QString m_project_filter;
  QString m_projectconfiguration_filter;
};

#endif // CLARK_OPENSLNDIALOG_WIDGETS_H
