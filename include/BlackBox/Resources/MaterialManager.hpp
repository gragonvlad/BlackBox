#pragma once

#include <BlackBox/Material.hpp>
#include <BlackBox/IEngine.hpp>
#include <BlackBox/ILog.hpp>
#include <BlackBox/Render/CShader.hpp>
#include <BlackBox/IConsole.hpp>

#include <map>
#include <string>
#include <tinyxml2.h>

extern Material *defaultMaterial;

class MaterialManager
{
	struct ShaderDesc 
	{
		std::string type;
		std::string name;
		ShaderDesc() {}
		ShaderDesc(std::string type, std::string name) : type(type), name(name){}
	};
	struct ProgramDesc
	{
		std::string name;
		std::string vs;
		std::string fs;
	};
  static MaterialManager *manager;
  std::map<std::string, Material*> cache;
  ILog *m_pLog;
	bool alpha_shakaled = false;
	bool isSkyBox = false;
	ICVar* root_path = nullptr;
	std::map<std::string, std::shared_ptr<CShaderProgram>> shaders_map;
public:
  static MaterialManager *instance();
  std::shared_ptr<CShaderProgram> getProgram(std::string name);
  Material *getMaterial(std::string name);
  static bool init(std::string materialLib);
	bool reloadShaders();
	bool reloadShaders(std::vector<std::string> names);

	void reloadShader(MaterialManager::ProgramDesc& pd);

private:
  MaterialManager();
  bool loadLib(std::string name);
	void getShaderAttributes(tinyxml2::XMLElement* shader, MaterialManager::ProgramDesc& pd);
  bool loadMaterial(tinyxml2::XMLElement *material);
  bool loadProgram(ProgramDesc &desc, bool isReload);
  BaseTexture *loadTexture(tinyxml2::XMLElement *texture);
  tinyxml2::XMLElement *saveTexture(tinyxml2::XMLDocument &xmlDoc, Texture *texture);
  std::shared_ptr<CShader> loadShader(ShaderDesc &sd, bool isReload);
  tinyxml2::XMLElement *saveShader(tinyxml2::XMLDocument &xmlDoc, CShader *shader);
};