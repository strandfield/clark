// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_ENTITY_H
#define CLARK_ENTITY_H

#include <string>

namespace clark
{

enum class AccessSpecifier
{
  Invalid,
  Public,
  Protected,
  Private,
};

enum class Whatsit
{
  Unexposed = 0,
  Typedef = 1,
  Function = 2,
  Variable = 3,
  Field = 4,
  EnumConstant = 5,
  ObjCClass = 6,
  ObjCProtocol = 7,
  ObjCCategory = 8,
  ObjCInstanceMethod = 9,
  ObjCClassMethod = 10,
  ObjCProperty = 11,
  ObjCIvar = 12,
  Enum = 13,
  Struct = 14,
  Union = 15,
  CXXClass = 16,
  CXXNamespace = 17,
  CXXNamespaceAlias = 18,
  CXXStaticVariable = 19,
  CXXStaticMethod = 20,
  CXXInstanceMethod = 21,
  CXXConstructor = 22,
  CXXDestructor = 23,
  CXXConversionFunction = 24,
  CXXTypeAlias = 25,
  CXXInterface = 26
};

struct Entity
{
  Whatsit kind = Whatsit::Unexposed;
  std::string name;
  std::string usr;
  std::string display_name;
  Entity* parent = nullptr;
  int flags = 0;

  enum Flag
  {
    // $todo: add 'mycode' flag
    Local     = 0x0001,
    Public    = 0x0002,
    Protected = 0x0004,
    Private   = 0x0006,
    Inline    = 0x0008,
    Static    = 0x0010,
    Constexpr = 0x0020,
    IsScoped  = 0x0040,
    IsStruct  = 0x0040,
    IsFinal   = 0x0080,
    Virtual   = 0x0100,
    Override  = 0x0200,
    Final     = 0x0400,
    Const     = 0x0800,
    Pure      = 0x1000,
    Noexcept  = 0x2000,
    Explicit  = 0x4000,
    Default   = 0x8000,
    Delete    = 0x10000,
  };

public:
  Entity() = default;
  Entity(Whatsit w, std::string name_, Entity* parent_ = nullptr) :
    kind(w),
    name(std::move(name_)),
    parent(parent_)
  {

  }
};

inline bool is_local(const Entity& s)
{
  return s.flags & Entity::Local;
}

inline bool test_flag(const Entity& s, Entity::Flag f)
{
  return s.flags & f;
}

inline void set_flag(Entity& s, Entity::Flag f, bool on = true)
{
  if (on)
    s.flags |= f;
  else
    s.flags &= ~((int)f);
}

inline AccessSpecifier get_access_specifier(const Entity& s)
{
  switch (s.flags & Entity::Private)
  {
  case Entity::Public:
    return AccessSpecifier::Public;
  case Entity::Protected:
    return AccessSpecifier::Protected;
  case Entity::Private:
    return AccessSpecifier::Private;
  default:
    return AccessSpecifier::Invalid;
  }
}

inline void set_access_specifier(Entity& s, AccessSpecifier a)
{
  s.flags = s.flags & ~Entity::Private;

  switch (a)
  {
  case AccessSpecifier::Public:
    s.flags |= Entity::Public;
    break;
  case AccessSpecifier::Protected:
    s.flags |= Entity::Protected;
    break;
  case AccessSpecifier::Private:
    s.flags |= Entity::Private;
    break;
  }
}

struct BaseClass
{
  AccessSpecifier access_specifier = AccessSpecifier::Invalid;
  Entity* base = nullptr;
  Entity* derived = nullptr;

  std::string toString() const;
  static BaseClass fromString(std::string_view str);
};


inline bool is_public_base(const BaseClass& base)
{
  return base.access_specifier == AccessSpecifier::Public;
}

inline bool is_protected_base(const BaseClass& base)
{
  return base.access_specifier == AccessSpecifier::Protected;
}

inline bool is_private_base(const BaseClass& base)
{
  return base.access_specifier == AccessSpecifier::Private;
}

} // namespace clark

#endif // CLARK_ENTITY_H
