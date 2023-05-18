// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "tufromsln.h"

#include "program/translationunit.h"

namespace clark
{

const vcxproj::ItemDefinitionGroup* defgroup4conf(const vcxproj::Project& project, const vcxproj::ProjectConfiguration& conf)
{
  for (const vcxproj::ItemDefinitionGroup& idg : project.itemDefinitionGroupList)
  {
    if (idg.condition.find(conf.name) != std::string::npos) // $todo: technically not correct, 
      return &idg; // 'condition' should be properly evaluated
  }

  return nullptr;
}

program::CompileOptions idg2copts(const vcxproj::ItemDefinitionGroup& idg)
{
  program::CompileOptions params;

  QStringList incdirs = QString::fromStdString(idg.additionalIncludeDirectories).split(';');

  for (const QString& incdir : incdirs)
    params.includedirs.insert(incdir.toStdString());

  QStringList defines = QString::fromStdString(idg.preprocessorDefinitions).split(';');

  for (const QString& def : defines)
  {
    int p = def.indexOf('=');

    if (p != -1)
    {
      std::string name = def.mid(0, p).toStdString();
      std::string value = def.mid(p + 1).toStdString();
      params.defines[name] = value;
    }
    else
    {
      params.defines[def.toStdString()] = "";
    }
  }

  return params;
}

std::unique_ptr<TranslationUnit> sln2tu(const vcxproj::Project& project, const vcxproj::ProjectConfiguration& conf, const std::string& compile)
{
  const vcxproj::ItemDefinitionGroup* idg = defgroup4conf(project, conf);

  program::CompileOptions params = idg2copts(*idg);

  auto tu = std::make_unique<TranslationUnit>(QString::fromStdString(compile));
  tu->setCompileOptions(params);

  return tu;
}

} // namespace clark
