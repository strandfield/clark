// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "openslndialog.h"
#include "openslndialog.widgets.h"

#include "program/translationunit.h"
#include "utils/tufromsln.h"

#include <QLabel>
#include <QPushButton>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <algorithm>

/* Widgets */

SlnProjectListWidget::SlnProjectListWidget(const vcxproj::Solution& s, QWidget* parent) : QListWidget(parent),
  m_solution(s)
{
  setSelectionMode(QAbstractItemView::SingleSelection);

  buildItems(m_solution);

  connect(this, &QListWidget::itemSelectionChanged, this, &SlnProjectListWidget::selectionChanged);
}

std::optional<std::string> SlnProjectListWidget::selection() const
{
  QList<QListWidgetItem*> items = selectedItems();

  if (items.isEmpty())
    return {};

  assert(items.size() == 1);

  auto* item = items.front();
  return item->text().toStdString();
}

void SlnProjectListWidget::buildItems(const vcxproj::Solution& solution)
{
  for (const vcxproj::Project& p : solution.projects)
  {
    auto* item = new QListWidgetItem(QString::fromStdString(p.name));
    addItem(item);
  }
}

SlnProjectConfigurationListWidget::SlnProjectConfigurationListWidget(QWidget* parent) : QListWidget(parent)
{
  setSelectionMode(QAbstractItemView::SingleSelection);

  connect(this, &QListWidget::itemSelectionChanged, this, &SlnProjectConfigurationListWidget::selectionChanged);
}

std::optional<std::string> SlnProjectConfigurationListWidget::selection() const
{
  QList<QListWidgetItem*> items = selectedItems();

  if (items.isEmpty())
    return {};

  assert(items.size() == 1);

  auto* item = items.front();
  return item->text().toStdString();
}

void SlnProjectConfigurationListWidget::setProject(const vcxproj::Project* project)
{
  clear();

  if (project)
    buildItems(*project);
}

void SlnProjectConfigurationListWidget::buildItems(const vcxproj::Project& project)
{
  for (const vcxproj::ProjectConfiguration& conf : project.projectConfigurationList)
  {
    auto* item = new QListWidgetItem(QString::fromStdString(conf.name));
    addItem(item);
  }
}

SlnCompileListWidget::SlnCompileListWidget(const vcxproj::Solution& s, QWidget* parent) : QListWidget(parent)
{
  setSelectionMode(QAbstractItemView::SingleSelection);

  buildItems(s);

  connect(this, &QListWidget::itemSelectionChanged, this, &SlnCompileListWidget::selectionChanged);
}

const QString& SlnCompileListWidget::projectFilter() const
{
  return m_project_filter;
}

void SlnCompileListWidget::setProjectFilter(const QString& f)
{
  if (m_project_filter != f)
  {
    m_project_filter = f;
    updateFilters();
  }
}


const QString& SlnCompileListWidget::projectConfigurationFilter() const
{
  return m_projectconfiguration_filter;
}

void SlnCompileListWidget::setProjectConfigurationFilter(const QString& f)
{
  if (m_projectconfiguration_filter != f)
  {
    m_projectconfiguration_filter = f;
    updateFilters();
  }
}

std::optional<SlnCompileListWidget::Item> SlnCompileListWidget::selection() const
{
  QList<QListWidgetItem*> items = selectedItems();

  if (items.isEmpty())
    return {};

  assert(items.size() == 1);

  auto* item = items.front();
  return toItem(item);
}

void SlnCompileListWidget::buildItems(const vcxproj::Solution& solution)
{
  for (const vcxproj::Project& p : solution.projects)
  {
    QString projectname = QString::fromStdString(p.name);

    QStringList configs;

    for (const vcxproj::ProjectConfiguration& pconf : p.projectConfigurationList)
    {
      configs.append(QString::fromStdString(pconf.name));
    }

    for (const std::string& filepath : p.compileList)
    {
      Item item;
      item.project = projectname;
      item.compile = QString::fromStdString(filepath);
      
      for (QString conf : configs)
      {
        item.projectConfiguration = conf;
       
        QListWidgetItem* listitem = createListItem(item);
        addItem(listitem);
      }
    }
  }
}

QListWidgetItem* SlnCompileListWidget::createListItem(const Item& item)
{
  auto* listitem = new QListWidgetItem();
  listitem->setData(Qt::UserRole + 1, item.project);
  listitem->setData(Qt::UserRole + 2, item.projectConfiguration);
  listitem->setData(Qt::UserRole + 3, item.compile);
  listitem->setText(QString("%1 | %2 | %3").arg(item.compile, item.project, item.projectConfiguration));
  return listitem;
}

SlnCompileListWidget::Item SlnCompileListWidget::toItem(QListWidgetItem* listitem)
{
  Item result;
  result.project = listitem->data(Qt::UserRole + 1).toString();
  result.projectConfiguration = listitem->data(Qt::UserRole + 2).toString();
  result.compile = listitem->data(Qt::UserRole + 3).toString();
  return result;
}

void SlnCompileListWidget::updateFilters()
{
  for (int i(0); i < count(); ++i)
  {
    QListWidgetItem* listitem = item(i);
    Item itemdata = toItem(listitem);

    bool visible = (projectFilter().isEmpty() || itemdata.project == projectFilter())
      && (projectConfigurationFilter().isEmpty() || itemdata.projectConfiguration == projectConfigurationFilter());

    listitem->setHidden(!visible);
  }
}

/* Dialog */

OpenSlnDialog::OpenSlnDialog(vcxproj::Solution s, QWidget* parent) : QDialog(parent),
  m_solution(std::move(s))
{
  setWindowTitle("Open Visual Studio Solution");
  setModal(true);

  auto* description_display = new QLabel(
    "Visual Studio Solution: " + QString::fromStdString(m_solution.filepath.string())
  );

  m_projects_widget = new SlnProjectListWidget(m_solution);
  m_configs_widget = new SlnProjectConfigurationListWidget();
  m_compiles_widget = new SlnCompileListWidget(m_solution);

  m_ok_button = new QPushButton("Ok");
  auto* cancel_button = new QPushButton("Cancel");

  {
    m_ok_button->setEnabled(false);
    m_ok_button->setDefault(true);
  }

  {
    auto* layout = new QVBoxLayout;

    layout->addWidget(description_display);
    
    {
      auto* sublayout = new QHBoxLayout;
      sublayout->addWidget(m_projects_widget);
      sublayout->addWidget(m_configs_widget);

      layout->addLayout(sublayout);
    }

    layout->addWidget(m_compiles_widget);

    {
      auto* buttonslayout = new QHBoxLayout;
      buttonslayout->addStretch();
      buttonslayout->addWidget(m_ok_button);
      buttonslayout->addWidget(cancel_button);
      buttonslayout->addStretch();

      layout->addLayout(buttonslayout);
    }

    setLayout(layout);
  }

  {
    connect(m_projects_widget, &SlnProjectListWidget::selectionChanged, this, [this]() {
      auto sel = m_projects_widget->selection();
      m_configs_widget->setProject(findProject(sel.value_or(std::string())));
      m_compiles_widget->setProjectFilter(QString::fromStdString(sel.value_or(std::string())));
      });

    connect(m_configs_widget, &SlnProjectConfigurationListWidget::selectionChanged, this, [this]() {
      auto sel = m_configs_widget->selection();
      m_compiles_widget->setProjectConfigurationFilter(QString::fromStdString(sel.value_or(std::string())));
      });

    connect(m_compiles_widget, &SlnCompileListWidget::selectionChanged, this, [this]() {
      m_ok_button->setEnabled(m_compiles_widget->selection().has_value());
      });

    connect(m_ok_button, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancel_button, &QPushButton::clicked, this, &QDialog::reject);
  }
}

int OpenSlnDialog::resultCode() const
{
  return QDialog::result();
}

std::unique_ptr<TranslationUnit> OpenSlnDialog::result() const
{ 
  std::optional<SlnCompileListWidget::Item> selection = m_compiles_widget->selection();

  if (!selection)
    return nullptr;

  const SlnCompileListWidget::Item& item = selection.value();

  const vcxproj::Project* project = findProject(item.project.toStdString());

  if (!project)
    return nullptr;

  const auto& confs = project->projectConfigurationList;

  auto it = std::find_if(confs.begin(), confs.end(), [&item](const vcxproj::ProjectConfiguration& c) {
    return c.name.c_str() == item.projectConfiguration;
    });

  if (it == confs.end())
    return nullptr;

  return clark::sln2tu(*project, *it, item.compile.toStdString());
}

const vcxproj::Project* OpenSlnDialog::findProject(const std::string& name) const
{
  auto it = std::find_if(m_solution.projects.begin(), m_solution.projects.end(), [&name](const vcxproj::Project& p) {
    return p.name == name;
    });

  return it != m_solution.projects.end() ? &(*it) : nullptr;
}
