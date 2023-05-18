// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_INCLUDE_H
#define CLARK_INCLUDE_H

struct File;

namespace clark
{

struct Include
{
  File* file = nullptr;
  File* included_file = nullptr;
  int line = -1;
};

} // namespace clark

#endif // CLARK_INCLUDE_H
