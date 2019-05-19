#pragma once

#include <BlackBox/ObjectManager.hpp>
#include <BlackBox/Triangle.hpp>
#include <BlackBox/Cube.hpp>
#include <BlackBox/Tetrahedron.hpp>
#include <string>

class Primitive
{
public:
  enum Type
  {
    TRIANGLE,
    PLANE,
    CUBE,
    TETRAHEDRON
  };
private:
  Primitive(Type type);
public:
  static Object *create(Type type, CShaderProgram *program);
  ~Primitive();
};