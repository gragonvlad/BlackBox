#include <BlackBox/Render/HdrTechnique.hpp>
#include <BlackBox/Render/ShadowMapTechnique.hpp>
#include <BlackBox/Render/FrameBufferObject.hpp>
#include <BlackBox/Render/IRender.hpp>
#include <BlackBox/Scene.hpp>
#include <BlackBox/Render/SkyBox.hpp>
#include <BlackBox/IConsole.hpp>
#include <BlackBox/Profiler/Profiler.h>
#include <BlackBox/Resources/MaterialManager.hpp>

#define NBLOOM
#define PREVIOS 0
#define CURRENT 1

HdrTechnique::HdrTechnique()
{
}

HdrTechnique::~HdrTechnique()
{
}

bool HdrTechnique::Init(Scene* pScene, FrameBufferObject* renderTarget)
{
  if (inited)
    return true;
  render = GetISystem()->GetIRender();
  m_Scene = pScene;
	initConsoleVariables();
	initTest();
  createShader();
  shadowMapping = new ShadowMapping();
	auto console = GetISystem()->GetIConsole();

	glm::ivec2 resolution = glm::vec2(bloomTest[testid]);
  hdrBuffer =  FrameBufferObject::create(FrameBufferObject::HDR_BUFFER, resolution.x, resolution.y, 2, false);

	auto mip_cnt = getMips(resolution);
	pass0.resize(mip_cnt);
	pass1.resize(mip_cnt);
	for (int i = 0, width = resolution.x, height = resolution.y; i < mip_cnt; i++)
	{
		pass0[i] = FrameBufferObject::create(FrameBufferObject::HDR_BUFFER, width, height, 1, false);
		width >>= 1;
		if (width <= 0) 
			width = 1;
		height >>= 1;
		if (height <= 0) 
			height = 1;
	}
	for (int i = 0, width = resolution.x, height = resolution.y; i < mip_cnt; i++)
	{
		pass1[i] = FrameBufferObject::create(FrameBufferObject::HDR_BUFFER, width, height, 1, false);
		width >>= 1;
		if (width <= 0) 
			width = 1;
		height >>= 1;
		if (height <= 0) 
			height = 1;
	}

	// generate three query objects
	glGenQueries(3, timer_queries);

  inited = true;
  return shadowMapping->Init(pScene, hdrBuffer);
}

bool HdrTechnique::OnRenderPass(int pass)
{
	DEBUG_GROUP(__FUNCTION__);
  if (!shadowMapping->OnRenderPass(pass))
  {
		if (m_Scene->GetSkyBox() != nullptr)
			void(0);// m_Scene->GetSkyBox()->draw(m_Scene->getCurrentCamera());
		{
			Object *water;
			if ((water = m_Scene->getObject("water")) != nullptr)
			{
				water->m_transparent = true;
				static_cast<ShadowMapping*>(shadowMapping)->RenderTransparent(water);

			}
		}
    return HdrPass();
  }
  return true;
}

int HdrTechnique::GetFrame()
{
  return shadowMapping->GetFrame();
}

bool HdrTechnique::HdrPass()
{
	if (enabled->GetIVal())
	{
    m_Scene->setPostProcessor(this);
		PROFILER_PUSH_GPU_MARKER("BLOOM PASS", Utils::COLOR_DARK_GREEN);
		BloomPass();
		PROFILER_POP_GPU_MARKER();
	}
  else
    m_Scene->setPostProcessor(nullptr);

  return false;
}

void HdrTechnique::BloomPass()
{
#if 0
	GLuint64 time_0, time_1, time_2;
#endif

	{
		const char section[] = "DOWNSAMPLING";
		PROFILER_PUSH_CPU_MARKER(section, Utils::COLOR_BLACK);
			DEBUG_GROUP(section);
			downsampling();
		PROFILER_POP_CPU_MARKER();
	}
	{
		const char section[] = "UPSAMPLING";
		PROFILER_PUSH_CPU_MARKER(section, Utils::COLOR_RED);
			DEBUG_GROUP(section);
			upsampling();
		PROFILER_POP_CPU_MARKER();
	}


#if 0
	glGetQueryObjectui64v(timer_queries[0], GL_QUERY_RESULT, &time_0);
	glGetQueryObjectui64v(timer_queries[1], GL_QUERY_RESULT, &time_1);
	glGetQueryObjectui64v(timer_queries[2], GL_QUERY_RESULT, &time_2);

	upsampleTime->Set(float((time_1 - time_0) / 1e06));
	downsampleTime->Set(float((time_2 - time_1) / 1e06));
#endif
}

void HdrTechnique::createShader()
{
	/*
  m_ScreenShader =new CShaderProgram(
    CShader::load("res/shaders/screenshader.vs", CShader::E_VERTEX), 
    CShader::load("res/shaders/hdrshader.frag", CShader::E_FRAGMENT)
  );
	*/

	ProgramDesc desc[] = {
		{
			"hdr_shader",
			"screenshader.vs",
			"hdrshader.frag"
		},
		{
			"downsampling",
			"screenshader.vs",
			"downsampling.frag"
		},
		{
			"upsampling",
			"screenshader.vs",
			"upsampling.frag",
		}

	};

	MaterialManager::instance()->loadProgram(desc[0],false);
	MaterialManager::instance()->loadProgram(desc[1],false);
	MaterialManager::instance()->loadProgram(desc[2],false);

	m_ScreenShader = MaterialManager::instance()->getProgram("hdr_shader");
	//m_ScreenShader->create();
	m_ScreenShader->use();
	m_ScreenShader->setUniformValue(0,"scene");
	m_ScreenShader->setUniformValue(1,"bloomBlur");
	m_ScreenShader->unuse();

	/*
  m_DownsampleShader =new CShaderProgram(
    CShader::load("res/shaders/screenshader.vs", CShader::E_VERTEX), 
    CShader::load("res/shaders/downsampling.frag", CShader::E_FRAGMENT)
  );
	*/
	m_DownsampleShader = MaterialManager::instance()->getProgram("downsampling");
	//m_DownsampleShader->create();
	m_DownsampleShader->use();
	m_DownsampleShader->setUniformValue(0,"image");
	m_DownsampleShader->unuse();

	/*
  m_UpsampleShader =new CShaderProgram(
    CShader::load("res/shaders/screenshader.vs", CShader::E_VERTEX), 
    CShader::load("res/shaders/upsampling.frag", CShader::E_FRAGMENT)
  );
	*/
	m_UpsampleShader = MaterialManager::instance()->getProgram("upsampling");
	//m_UpsampleShader->create();

}

void HdrTechnique::initConsoleVariables()
{
  exposure =				GetISystem()->GetIConsole()->CreateVariable("exp", 1.0f, 0, "exposure");
  enabled =					GetISystem()->GetIConsole()->CreateVariable("hdr", 1, 0, "Enable/disable HDR");
  bloom =						GetISystem()->GetIConsole()->CreateVariable("bloom", 1, 0, "Enable/disable HDR");
  bloomThreshold =	GetISystem()->GetIConsole()->CreateVariable("bt", 2.0f, 0, "Bloom threshold");
  blurOn =					GetISystem()->GetIConsole()->CreateVariable("blur", 1, 0, "Enable/disable blur for bloom");
  bloom_exposure =	GetISystem()->GetIConsole()->CreateVariable("bexp", 0.007f, 0, "Enable/disable blur for bloom");
  bloomTime =				GetISystem()->GetIConsole()->CreateVariable("bloomtime", 0.f, 0, "Time of bloom");
  upsampleTime =		GetISystem()->GetIConsole()->CreateVariable("uptime", 0.f, 0, "Time of bloom");
  downsampleTime =	GetISystem()->GetIConsole()->CreateVariable("dtime", 0.f, 0, "Time of bloom");
  offset =					GetISystem()->GetIConsole()->CreateVariable("offset", -3.0f, 0, "Enable/disable blur for bloom");
  useBoxFilter =		GetISystem()->GetIConsole()->CreateVariable("bf", 0, 0, "Enable/disable BoxFilter in bloom");
  defaultFilter =		GetISystem()->GetIConsole()->CreateVariable("df", 1, 0, "Enable/disable default filtering in bloom");
}

void HdrTechnique::initTest()
{
	bloomTest.push_back({ 640, 480 });
	bloomTest.push_back({ 800, 600 });
	bloomTest.push_back({ 1024, 768 });
	bloomTest.push_back({ 1366, 768 });
	bloomTest.push_back({ 1400, 900 });
	bloomTest.push_back({ 1600, 900 });
	bloomTest.push_back({ 1920, 1080 });
	bloomTest.push_back({ 2560, 1440 });
	bloomTest.push_back({ 2560, 1600 });
	testid = std::min(GetISystem()->GetIConsole()->GetCVar("testid")->GetIVal(), (int)bloomTest.size());


}

int HdrTechnique::getMips(glm::vec2 resolution)
{
	//return std::log2(std::max(pass0[0]->viewPort.z, pass0[0]->viewPort.w)) + 1;
	return 6;
}

bool HdrTechnique::PreRenderPass()
{
  return false;
}

void HdrTechnique::PostRenderPass()
{
}

void HdrTechnique::downsampling()
{
	// 2. blur bright fragments with two-pass Gaussian Blur 
				 // --------------------------------------------------
	bool horizontal = true, first_iteration = true;
	unsigned int amount = PASSES;
	m_DownsampleShader->use();
  m_DownsampleShader->setUniformValue(useBoxFilter->GetIVal(), "use_box_filter");
  m_DownsampleShader->setUniformValue(defaultFilter->GetIVal(), "default_filter");
  m_DownsampleShader->setUniformValue(offset->GetFVal(), "offset");
	glCheck(glDisable(GL_DEPTH_TEST));
	glActiveTexture(GL_TEXTURE0);
	if (useBoxFilter->GetIVal())
		amount = 1;
	else
		amount = getMips({ pass0[0]->viewPort.z, pass0[0]->viewPort.w });
	for (unsigned int i = 0; i < amount - 1; i++)
	{
		pass0[i + 1]->bind();
		//m_DownsampleShader->setUniformValue(horizontal, "horizontal");
		glBindTexture(GL_TEXTURE_2D, first_iteration ? hdrBuffer->texture[1] : pass0[i]->texture[0]);  // bind texture of other framebuffer (or scene if first iteration)
		//renderQuad();
		m_ScreenQuad.draw();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	pingpong = !horizontal;
	glCheck(glEnable(GL_DEPTH_TEST));
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void HdrTechnique::upsampling()
{
	m_UpsampleShader->use();
	m_UpsampleShader->setUniformValue(blurOn->GetIVal(), "blurOn");
	
	uint32_t amount;
	bool first_iteration = true;
	glCheck(glDisable(GL_DEPTH_TEST));

	amount = getMips({ pass0[0]->viewPort.z, pass0[0]->viewPort.w });
	for (unsigned int i = amount - 1; i > 0; i--)
	{
		pass1[i - 1]->bind();
		m_UpsampleShader->bindTexture2D(first_iteration ? pass0[amount - 1]->texture[0] : pass1[i]->texture[0], PREVIOS, "previos");
		m_UpsampleShader->bindTexture2D(first_iteration ? pass0[amount - 1]->texture[0] : pass0[i - 1]->texture[0], CURRENT, "current");
		m_ScreenQuad.draw();
		if (first_iteration)
			first_iteration = false;
	}

}

void HdrTechnique::Do(unsigned int texture)
{
	glm::vec3 fog = glm::vec3(
		GetISystem()->GetIConsole()->GetCVar("fogR")->GetFVal(),
		GetISystem()->GetIConsole()->GetCVar("fogG")->GetFVal(),
		GetISystem()->GetIConsole()->GetCVar("fogB")->GetFVal());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glClearColor(fog.r, fog.g, fog.b, 1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	m_ScreenShader->use();
  m_ScreenShader->setUniformValue(exposure->GetFVal(), "exposure");
  m_ScreenShader->setUniformValue(bloom_exposure->GetFVal(), "bloom_exposure");
	glDisable(GL_DEPTH_TEST);

	m_ScreenShader->bindTexture2D(hdrBuffer->texture[0], 0, "scene");
	m_ScreenShader->bindTexture2D(pass1[0]->texture[0], 1, "bloomBlur");
	m_ScreenShader->setUniformValue(bloom->GetIVal(), "bloom");

	glCheck(glViewport(0, 0, GetISystem()->GetIRender()->GetWidth(), GetISystem()->GetIRender()->GetHeight()));
	m_ScreenQuad.draw();
}


#if 0
GLuint timer_queries[3];
// generate three query objects
glGenQueries(3, &timer_queries);
[...]
// query time stamps around all draw routines
glQueryCounter(timer_queries[0], GL_TIMESTAMP);
draw_world();
glQueryCounter(timer_queries[1], GL_TIMESTAMP);
draw_models();
glQueryCounter(timer_queries[2], GL_TIMESTAMP);
[...]
// later in the program retrieve the query results
GLuint64 time_0, time_1, time_2;
glGetQueryObjectui64v(timer_queries[0], GL_QUERY_RESULT, &time_0);
glGetQueryObjectui64v(timer_queries[1], GL_QUERY_RESULT, &time_1);
glGetQueryObjectui64v(timer_queries[2], GL_QUERY_RESULT, &time_2);
printf(" world draw time : %f ms \n", double(time_1 - time_0) / 1 e06);
printf(" models draw time: %f ms \n", double(time_2 - time_1) / 1 e06);
#endif
