#pragma once
#include <vector>
#include <string>
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Clock.hpp"
class Timer;
class DevConsole;
extern DevConsole* g_theDevConsole;

enum class DevConsoleMode {
	HIDDEN,
	OPENFULL,
	NUM_MODE,
};


struct DevConsoleLine {
	std::string m_text;
	Rgba8 m_color;
	float m_cellHeight=1.f;
	
};

struct DevConsoleConfig {
	
	Renderer* m_renderer=nullptr;
	Camera* m_camera = nullptr;
	//std::string m_fontName = "SquirrelFixedFont";
	float m_fontAspect = 0.7f;
	int m_lineOnScreen = 40;
	int m_maxCommandHistory = 128;
	bool m_startOpen = false;
	BitmapFont* m_font=nullptr;
};

class DevConsole{
public:
	DevConsole(DevConsoleConfig const& config);
	~DevConsole();

	void StartUp();
	void ShutDown();
	void BeginFrame();
	void EndFrame();

	void Execute(std::string const& consoleCommandText, bool echoCommand=true);
	
	void AddLine(std::string text, Rgba8 color=Rgba8(0,0,0),float cellHeight=1.f);

	void Render(AABB2 const& bounds, Renderer* rendererOverride = nullptr)const;

	DevConsoleMode GetMode()const;
	void SetMode(DevConsoleMode mode);
	void ToggleMode(DevConsoleMode mode);
	void SetFontSize(float size);
/*
	static const Rgba8 ERROR;
	static const Rgba8 WARNNING;
	static const Rgba8 INFO_MAJOR;
	static const Rgba8 INFO_MINOR;
	static const Rgba8 INPUT_TEXT;
	static const Rgba8 INPUT_INSERTION_POINT;*/

	static bool Event_KeyPressed(EventArgs& args);
	static bool Event_CharInput(EventArgs& args);
	static bool Command_Clear(EventArgs& args);
	static bool Command_Help(EventArgs& args);

	

protected: 
	void Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font) const;
protected:
	DevConsoleConfig m_config;
	
	DevConsoleMode m_mode = DevConsoleMode::HIDDEN;
	
	std::vector<DevConsoleLine> m_lines;

	std::string m_inputText;

	int m_insertionPointPosition = 0;


	bool m_insertionPointVisible = true;

	Timer* m_insertionPointBlinkTimer=nullptr;

	std::vector<std::string> m_commandHistory;

	int m_frameNumber = 0;

	int m_commandLineNum = 0;
};