#pragma once
#include <memory>
#include <BlackBox/Texture.hpp>
#include <glm/glm.hpp>

class CShader;
class CShaderProgram;
class Object;
class CCamera;

struct Material
{
  Texture
  *diffuse,
  *specular,
  *bump,
  *normal,
  *mask;
  glm::vec3 diffuseColor;

  CShaderProgram *program;
  std::shared_ptr<std::string> name;
  bool hasTexture = false;
  Material() : diffuse(nullptr), specular(nullptr), bump(nullptr), normal(nullptr), mask(nullptr)
  {

  }
  void apply(Object *object, CCamera *camera);
  void setTexture(Texture *texture, const char *type);
private:
  void activeTexture(uint32_t block, const char *uniform, Texture *texture);
};
