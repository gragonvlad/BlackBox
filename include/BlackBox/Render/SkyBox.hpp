#pragma once
#include <BlackBox/IDrawable.hpp>
#include <BlackBox/Render/Opengl.hpp>
#include <BlackBox/Render/Texture.hpp>
#include <BlackBox/Render/TextureCube.hpp>
#include <BlackBox/Render/VertexBuffer.hpp>
#include <BlackBox/Render/CShader.hpp>


class SkyBox : public IDrawable
{
public:
	TextureCube* texture;
	VertexArrayObject* vao;
	CBaseShaderProgram* shader;

	SkyBox(TextureCube *t)
		:
		texture(t),
		shader(new CShaderProgram(CShader::load("res/shaders/skybox.vs", CShader::E_VERTEX), CShader::load("res/shaders/skybox.frag", CShader::E_FRAGMENT)))
	{
		shader->create();
		shader->use();
		shader->setUniformValue(0, "skybox");
		shader->unuse();
		static float skyboxVertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};
		VertexArrayObject::Attributes attributes;
		attributes.stride = 3 * sizeof(float);
		attributes.attributes[VertexArrayObject::POSITION] = 0;
		vao = new VertexArrayObject(skyboxVertices, 36, GL_TRIANGLES, attributes);
	}
	// ������������ ����� IDrawable
	virtual void draw(void *data) override
	{
		glm::mat4 View = Pipeline::instance()->view;
		glm::mat4 Projection = Pipeline::instance()->projection;
		glCheck(glDepthMask(GL_FALSE));
		glCheck(glDepthFunc(GL_LEQUAL));
		shader->use();
		// ... ������� ������� � ������������ ������
		shader->setUniformValue(glm::mat4(glm::mat3(View)), "View");
		shader->setUniformValue(Projection, "Projection");
		
		texture->bind();
		vao->draw();

		glCheck(glDepthFunc(GL_LESS));
		glCheck(glDepthMask(GL_TRUE));
	}

	void setTextureCube(TextureCube* t)
	{
		if (texture)
			delete texture;
		texture = t;
	}
};
