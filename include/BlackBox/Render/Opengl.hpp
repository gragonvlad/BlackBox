#pragma once
#ifdef GLAD_LOADER
//#define _WINDOWS_
#include <glad/glad.h>
#else
#ifdef GLEW_LOADER
#include <GL/glew.h>
#else
#error OPENGL LOADER NOT SETTED
#endif
#endif

#include <BlackBox/Render/OpenglDebug.hpp>

bool OpenGLLoader();
