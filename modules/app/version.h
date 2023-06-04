// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_VERSION_H
#define CLARK_VERSION_H

// Try to follow Semantic Versioning 2.0.0, see https://semver.org/.
#define CLARK_VERSION_MAJOR 0
#define CLARK_VERSION_MINOR 3
#define CLARK_VERSION_PATCH 0
#define CLARK_VERSION_SUFFIX "dev"

#include <QVersionNumber>

#include <string>

namespace clark
{

QVersionNumber version();
std::string versionString();

} // namespace clark

#endif // CLARK_VERSION_H
