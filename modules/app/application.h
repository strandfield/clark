// Copyright (C) 2023 Vincent Chambrin
// This file is part of the 'clark' project.
// For conditions of distribution and use, see copyright notice in LICENSE.

#ifndef CLARK_APPLICATION_H
#define CLARK_APPLICATION_H

#include <QApplication>

#include <utility>

class Settings;

class Application : public QApplication
{
  Q_OBJECT
public:
  Application(int& argc, char** argv);

  static Application& instance();

  Settings& settings() const;

  template<typename T, typename...Args>
  T& init(Args&&... args);

  template<typename T>
  T* find() const;

  template<typename T>
  T& get() const;

  template<typename T>
  T& dependency();

  template<typename T>
  void deinit();
};

#define ClarkApp Application::instance()

template<typename T, typename...Args>
inline T& Application::init(Args&&... args)
{
  T* obj = new T(std::forward<Args>(args)..., this);
  return *obj;
}

template<typename T>
inline T* Application::find() const
{
  return findChild<T*>();
}

template<typename T>
inline T& Application::get() const
{
  return *find<T>();
}

template<typename T>
inline T& Application::dependency()
{
  T* dep = find<T>();
  return dep ? *dep : init<T>();
}

template<typename T>
inline void Application::deinit() 
{
  auto* child = findChild<T*>();
  child->setParent(nullptr);
  child->deleteLater();
}

#endif // CLARK_APPLICATION_H
