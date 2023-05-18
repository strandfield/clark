// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "aboutdialog.h"

#include "version.h"

#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QTabWidget>

#include <QVBoxLayout>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QSettings>

AboutDialog::AboutDialog(QWidget* parent) : QDialog(parent)
{
  setWindowTitle("About");

  auto* description_display = new QLabel(
    "Clark " + clark::version().toString()
  );

  auto* thirdparties_display = new QLabel(
    "This software uses the following dependencies: "
  );

  m_licenses_widget = new QTabWidget;

  QDirIterator thirdparties{ ":/thirdparty", QDir::Dirs };

  while(thirdparties.hasNext())
  {
    QDir dir{ thirdparties.next() };
    createLicenseNoticeWidget(dir);
  }

  m_ok_button = new QPushButton("Ok");
  m_ok_button->setDefault(true);

  {
    auto* layout = new QVBoxLayout;

    layout->addWidget(description_display);
    layout->addWidget(thirdparties_display);
    layout->addWidget(m_licenses_widget);
    layout->addWidget(m_ok_button, 0, Qt::AlignHCenter);

    setLayout(layout);
  }

  {
    connect(m_ok_button, &QPushButton::clicked, this, &QDialog::accept);
  }
}

void AboutDialog::createLicenseNoticeWidget(const QDir& dir)
{
  QSettings settings{ dir.absoluteFilePath("info.ini"), QSettings::IniFormat };
  QString name = settings.value("name").toString();
  QString url = settings.value("url").toString();

  QFile license{ dir.absoluteFilePath("LICENSE") };

  if (!license.open(QIODevice::ReadOnly))
    return;

  auto* textedit = new QPlainTextEdit;
  textedit->setReadOnly(true);
  QFont font;
  font.setFamily("Courier");
  font.setPointSize(6);
  textedit->document()->setDefaultFont(font);
  
  QTextCursor cursor{ textedit->document() };
  cursor.insertText(url + "\n\n");
  cursor.insertText(QString::fromUtf8(license.readAll()));
  textedit->moveCursor(QTextCursor::Start);

  m_licenses_widget->addTab(textedit, name);
}
