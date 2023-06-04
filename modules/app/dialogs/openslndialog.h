// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_OPENSLNDIALOG_H
#define CLARK_OPENSLNDIALOG_H

// QDialog needs to be included before solution.h because Qt moc seems 
// to have some issue (on Linux) when <filesystem> is included first.
// See https://stackoverflow.com/questions/69407237/qt-moc-errorusr-include-c-10-bits-fs-fwd-39-parse-error-at-std
#include <QDialog>

#include <vcxproj/solution.h>

#include <memory>

class QDir;
class QTabWidget;

class TranslationUnit;

class SlnProjectListWidget;
class SlnProjectConfigurationListWidget;
class SlnCompileListWidget;

class OpenSlnDialog : public QDialog
{
  Q_OBJECT
public:
  explicit OpenSlnDialog(vcxproj::Solution s, QWidget* parent = nullptr);

  int resultCode() const;
  std::unique_ptr<TranslationUnit> result() const;

private:
  const vcxproj::Project* findProject(const std::string& name) const;

private:
  vcxproj::Solution m_solution;
  SlnProjectListWidget* m_projects_widget = nullptr;
  SlnProjectConfigurationListWidget* m_configs_widget = nullptr;
  SlnCompileListWidget* m_compiles_widget = nullptr;
  QPushButton* m_ok_button = nullptr;
};

#endif // CLARK_OPENSLNDIALOG_H
