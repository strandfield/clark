// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_ABOUTDIALOG_H
#define CLARK_ABOUTDIALOG_H

#include <QDialog>

class QDir;
class QTabWidget;

class AboutDialog : public QDialog
{
  Q_OBJECT
public:
  explicit AboutDialog(QWidget* parent = nullptr);

private:
  void createLicenseNoticeWidget(const QDir& dir);

private:
  QTabWidget* m_licenses_widget;
  QPushButton* m_ok_button;
};

#endif // CLARK_ABOUTDIALOG_H
