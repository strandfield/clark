// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_REFERENCE_H
#define CLARK_REFERENCE_H

namespace clark
{

struct File;
struct Entity;

struct EntityReference
{
  Entity* symbol = nullptr;
  File* file = nullptr;
  int line;
  int col;
  Entity* parent_symbol = nullptr;

  /**
   * \brief a combination of flags as described by the Flag enum
   */
  int flags = 0;

  /**
   * \brief flag values for a symbol reference
   * 
   * This enum mirrors libclang's CXSymbolRole.
   */
  enum Flag
  {
    Declaration = 1 << 0,
    Definition = 1 << 1,
    Reference = 1 << 2,
    Read = 1 << 3,
    Write = 1 << 4,
    Call = 1 << 5,
    Dynamic = 1 << 6,
    AddressOf = 1 << 7,
    Implicit = 1 << 8
  };
};

inline bool operator==(const EntityReference& lhs, const EntityReference& rhs)
{
  return lhs.symbol == rhs.symbol
    && lhs.file == rhs.file
    && lhs.line == rhs.line
    && lhs.col == rhs.col
    && lhs.parent_symbol == rhs.parent_symbol
    && lhs.flags == rhs.flags;
}

inline bool operator!=(const EntityReference& lhs, const EntityReference& rhs)
{
  return !(lhs == rhs);
}

} // namespace clark

#endif // CLARK_REFERENCE_H
