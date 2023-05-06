// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "indexingresult.h"

#include <algorithm>

namespace clark
{

const Entity* find_entity(const IndexingResult& idx, const USR& usr)
{
  auto it = idx.symbols.find(usr);
  return it != idx.symbols.end() ? it->second.get() : nullptr;
}

const EntityReference* find_definition(const std::vector<EntityReference>& refs, const Entity& e)
{
  auto it = std::find_if(refs.begin(), refs.end(), [&e](const EntityReference& r) {
    return r.symbol == &e && (r.flags & EntityReference::Definition);
    });

  return it != refs.end() ? &(*it) : nullptr;
}

} // namespace clark
