#pragma once

#include <BlackBox/ISystem.hpp>
#include <BlackBox/Window.hpp>
#include <BlackBox/IConsole.hpp>
#include <BlackBox/IScriptSystem.hpp>

//#define SAFE_RELEASE(o) if (o) o->Release();
template<typename T>
inline auto SAFE_RELEASE(T const* t) -> decltype((void)(t->Release()), void())
{
	t->Release();
}

template<typename T>
inline auto SAFE_RELEASE(T *& t)
{
	if (t != nullptr)
	{
		delete t;
		t = nullptr;
	}
}

class CConsole;
class CRender;

class CSystem : public ISystem, public IInputEventListener, public IConsoleVarSink
{
public:
	CSystem();
	~CSystem();
  
	// Inherited via ISystem
  virtual bool Init() override;
  virtual void Start() override;
	virtual void Update() override;
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
  virtual void Release() override;

  virtual IShaderManager*		GetShaderManager() override;
  virtual IRender*					GetIRender() override;
  virtual ILog*							GetILog() override;
  virtual IConsole*					GetIConsole() override;
  virtual IGame*						getIGame() override;
	virtual IFont*						getIFont() override;
	virtual IWindow*					getIWindow() override;
	virtual IInputHandler*		getIInputHandler() override;
	virtual IScriptSystem*		getIIScriptSystem() override;

  virtual IGame*						CreateGame(IGame* game) override;

	virtual void ShowMessage(const char* message, const char* caption, MessageType messageType) override;
	virtual void Log(const char* message) override;

	virtual bool OnInputEvent(sf::Event& event) override;
	// Inherited via IConsoleVarSink
	virtual bool OnBeforeVarChange(ICVar* pVar, const char* sNewValue) override;

	bool ConfigLoad(const char* file);
private:
  ILog *m_pLog;
  CConsole *m_pConsole;
  IGame *m_pGame;
	IFont* m_pFont;
	IWindow* m_pWindow;
	IInputHandler* m_InputHandler;
	IRender* m_Render;
	IScriptSystem* m_pScriptSystem;
private:
	ICVar* r_window_width;
	ICVar* r_window_height;
	ICVar* r_bpp;
	ICVar* r_zbpp;
	ICVar* r_sbpp;
	ICVar* r_fullscreen;
};
