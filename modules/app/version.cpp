// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#include "version.h"

#include <cstring>

namespace clark
{

QVersionNumber version()
{
  return QVersionNumber(CLARK_VERSION_MAJOR, CLARK_VERSION_MINOR, CLARK_VERSION_PATCH);
}

std::string versionString()
{
  std::string base = std::to_string(CLARK_VERSION_MAJOR)
    + '.' + std::to_string(CLARK_VERSION_MINOR)
    + '.' + std::to_string(CLARK_VERSION_PATCH);

  return std::strlen(CLARK_VERSION_SUFFIX) > 0 ? base + '-' + CLARK_VERSION_SUFFIX : base;
}

} // namespace clark
