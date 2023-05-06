// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_INDEXINGRESULT_H
#define CLARK_INDEXINGRESULT_H

#include "entity.h"
#include "file.h"
#include "include.h"
#include "reference.h"
#include "usr.h"

#include <chrono>
#include <filesystem>
#include <map>
#include <memory>
#include <vector>

namespace clark
{

struct BaseClass;
struct Entity;

struct IndexingResult
{
  std::chrono::milliseconds indexing_time;

  std::map<std::filesystem::path, std::unique_ptr<File>> files;
  std::map<USR, std::unique_ptr<Entity>> symbols;
  std::vector<Include> ppincludes;
  std::vector<EntityReference> references;
  std::vector<BaseClass> bases;
};

const Entity* find_entity(const IndexingResult& idx, const USR& usr);

const EntityReference* find_definition(const std::vector<EntityReference>& refs, const Entity& e);

inline const EntityReference* find_definition(const IndexingResult& idx, const Entity& e)
{
  return find_definition(idx.references, e);
}

} // namespace clark

#endif // CLARK_INDEXINGRESULT_H
