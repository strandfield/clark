// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_TUFROMSLN_H
#define CLARK_TUFROMSLN_H

#include <vcxproj/project.h>

#include <memory>

class TranslationUnit;

namespace program
{
struct CompileOptions;
} // namespace program

namespace clark
{

const vcxproj::ItemDefinitionGroup* defgroup4conf(const vcxproj::Project& project, const vcxproj::ProjectConfiguration& conf);

program::CompileOptions idg2copts(const vcxproj::ItemDefinitionGroup& idg);

std::unique_ptr<TranslationUnit> sln2tu(const vcxproj::Project& project, const vcxproj::ProjectConfiguration& conf, const std::string& compile);

} // namespace clark

#endif // CLARK_TUFROMSLN_H
