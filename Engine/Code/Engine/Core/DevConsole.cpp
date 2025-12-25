#include "Engine/Core/DevConsole.hpp"
#include "EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "VertexUtils.hpp"
#include "Timer.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include <sstream>
#include <mutex>
std::mutex g_devconsoleMutex;
DevConsole::DevConsole(DevConsoleConfig const& config)
:m_config(config)
{

}
DevConsole::~DevConsole() {

}
void DevConsole::StartUp() {
	

	DevConsoleLine testLine1;
	testLine1.m_cellHeight = 1.f;
	testLine1.m_color = Rgba8(0, 0, 0);
	testLine1.m_text = "Type help for a list of commands";
	m_lines.push_back(testLine1);

	SubscribeEventCallbackFunction("HELP", DevConsole::Command_Help);
	SubscribeEventCallbackFunction("CLEAR", DevConsole::Command_Clear);
	SubscribeEventCallbackFunction("KeyPressedForDev", DevConsole::Event_KeyPressed);
	SubscribeEventCallbackFunction("CharInput", DevConsole::Event_CharInput);
	if (&Clock::GetSystemClock()==nullptr) {
		Clock* sysClock = new Clock();
		UNUSED(sysClock);
	}
	m_insertionPointBlinkTimer = new Timer(.5);
	m_insertionPointBlinkTimer->Start();
	//FireEvent("HELP");
}
void DevConsole::ShutDown() {
	delete m_insertionPointBlinkTimer;
	m_insertionPointBlinkTimer = nullptr;
}
void DevConsole::BeginFrame() {
	bool elapsed=m_insertionPointBlinkTimer->DecrementPeriodIfElapsed();
	while (elapsed) {
		m_insertionPointVisible = !m_insertionPointVisible;
		elapsed = m_insertionPointBlinkTimer->DecrementPeriodIfElapsed();
	}
}
void DevConsole::EndFrame() {

}

void DevConsole::Execute(std::string const& consoleCommandText, bool echoCommand) {
    g_devconsoleMutex.lock();
	if (echoCommand) {
		AddLine(consoleCommandText);
	}
	std::stringstream ss(consoleCommandText);
	std::vector<std::string> arguments;
	std::string temp;
	while (std::getline(ss, temp, ' ')) {
		arguments.push_back(temp);
	}
	if (arguments.size() > 1) {
	    EventArgs args;
		for (int i = 1; i < arguments.size(); i++) {
			std::vector<std::string> words;
			std::stringstream commandArg(arguments[i]);
			while (std::getline(commandArg, temp, '=')) {
				words.push_back(temp);
			}
			args.SetValue(words[0], words[1]);
		}
		FireEvent(arguments[0], args);
	}
	else {
		FireEvent(consoleCommandText);
	}

	m_insertionPointBlinkTimer->Start();
	g_devconsoleMutex.unlock();
}
void DevConsole::Render(AABB2 const& bounds, Renderer* rendererOverride)const{
	if (GetMode()==DevConsoleMode::OPENFULL) {
		if (rendererOverride == nullptr){
			Render_OpenFull(bounds, *m_config.m_renderer, *m_config.m_font);
		}
		else {
			Render_OpenFull(bounds, *rendererOverride, *m_config.m_font);
		}
		
	}
}

DevConsoleMode DevConsole::GetMode()const {
	return m_mode;
}
void DevConsole::SetMode(DevConsoleMode mode) {
	m_mode = mode;
}
void DevConsole::ToggleMode(DevConsoleMode mode) {
	if (m_mode == mode) {
		m_mode = DevConsoleMode::HIDDEN;
	}
	else {
		m_mode = mode;
	}
}

void DevConsole::SetFontSize(float size)
{
    m_config.m_fontAspect=size;
}

void DevConsole::Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font) const {
	
	std::vector<Vertex_PCU> verts;
	float minsY = bounds.m_mins.y;
	float lineHeight = m_config.m_fontAspect;
	DevConsoleLine line;
	line.m_cellHeight = lineHeight;
	line.m_color = Rgba8(255, 255, 255);
	line.m_text = m_inputText;
	AABB2 lineBound(Vec2(bounds.m_mins.x, minsY), Vec2(bounds.m_maxs.x, minsY+lineHeight));
	font.AddVertsForTextInBox2D(verts, line.m_text, lineBound, lineHeight, line.m_color, 1.f, Vec2(0, 0));
	int maxLen = (int)(m_lines.size() - 1);
	//lazy way to prevent render off screen
	if (maxLen > 30) {
		maxLen = 30;
	}
	if (!m_lines.empty()) {
		for (int i = maxLen; i >= 0; i--) {
			line = m_lines[i];
			minsY += lineHeight;
			lineBound=AABB2(Vec2(bounds.m_mins.x, minsY), Vec2(bounds.m_maxs.x, minsY + lineHeight));
			font.AddVertsForTextInBox2D(verts, line.m_text, lineBound, lineHeight, line.m_color, 1.f, Vec2(0, 0));
		}
	}
	renderer.BindTexture(&font.GetTexture());
	renderer.DrawVertexArray(verts);
	//render insertion point
	if (m_insertionPointVisible) {
		renderer.BindTexture(nullptr);
		std::vector<Vertex_PCU> insertionPoint;
		float y = bounds.m_mins.y;
		float x = bounds.m_mins.x + (m_insertionPointPosition) *.5f;
		insertionPoint.clear();
		AddVertsForRect(insertionPoint, Vec2(x, y), Vec2(x + 0.1f, y), Vec2(x + 0.1f, y + 1.f), Vec2(x, y + 1.f), Rgba8(255, 255, 255), Vec2(0, 0));
		renderer.DrawVertexArray(insertionPoint);
	}
}
void DevConsole::AddLine(std::string text, Rgba8 color, float cellHeight) {
    g_devconsoleMutex.lock();
	DevConsoleLine newLine;
	newLine.m_cellHeight = cellHeight;
	newLine.m_color = color;
	newLine.m_text = text;
	m_lines.push_back(newLine);
	g_devconsoleMutex.unlock();
}

bool DevConsole::Event_KeyPressed(EventArgs& args) {
	unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", -1);
	if (g_theDevConsole->GetMode() == DevConsoleMode::OPENFULL) {
		//reset timer
		g_theDevConsole->m_insertionPointBlinkTimer->Start();
		g_theDevConsole->m_insertionPointVisible = true;
		//
		//Numbers
		if (keyCode >= 0x30 && keyCode <= 0x39) {
			FireEvent("CharInput", args);
		}
		//chars
		if (keyCode >= 0x41 && keyCode <= 0x5A) {
			FireEvent("CharInput", args);
		}
		//space
		if (keyCode ==' ') {
			FireEvent("CharInput", args);
		}
		if (keyCode == KEYCODE_EQUAL) {
			FireEvent("CharInput", args);
		}

		if (keyCode == KEYCODE_DECIMAL) {
			FireEvent("CharInput", args);
		}
		//Insertion--------------------------
		//Movement
		if (keyCode == KEYCODE_LEFT) {
			if (g_theDevConsole->m_insertionPointPosition > 0) {
				g_theDevConsole->m_insertionPointPosition--;
				return true;
			}
		}
		if (keyCode == KEYCODE_RIGHT) {
			if (g_theDevConsole->m_insertionPointPosition < (int)g_theDevConsole->m_inputText.size()) {
				g_theDevConsole->m_insertionPointPosition++;
				return true;
			}
		}
		if (keyCode == KEYCODE_HOME) {
			g_theDevConsole->m_insertionPointPosition = 0;
			return true;
		}
		if (keyCode == KEYCODE_END) {
			g_theDevConsole->m_insertionPointPosition = (int)g_theDevConsole->m_inputText.size();
			return true;
		}
		//Delete
		if (keyCode == KEYCODE_BACKSPACE) {
			if (g_theDevConsole->m_insertionPointPosition > 0) {
				g_theDevConsole->m_inputText.erase(g_theDevConsole->m_insertionPointPosition - 1, 1);
				g_theDevConsole->m_insertionPointPosition--;
				return true;
			}
		}
		if (keyCode == KEYCODE_DELETE) {
			if (g_theDevConsole->m_insertionPointPosition < (int)g_theDevConsole->m_inputText.size()) {
				g_theDevConsole->m_inputText.erase(g_theDevConsole->m_insertionPointPosition, 1);
				return true;
			}
		}
		if (keyCode == KEYCODE_ENTER) {
			g_theDevConsole->m_commandHistory.push_back(g_theDevConsole->m_inputText);
			g_theDevConsole->m_commandLineNum = (int)g_theDevConsole->m_commandHistory.size();
			g_theDevConsole->Execute(g_theDevConsole->m_inputText, true);
			g_theDevConsole->m_inputText.clear();
			g_theDevConsole->m_insertionPointPosition = 0;
			return true;
		}
		//History
		if (keyCode == KEYCODE_UP) {
			if (g_theDevConsole->m_commandLineNum > 0) {
				g_theDevConsole->m_commandLineNum--;
				g_theDevConsole->m_inputText = g_theDevConsole->m_commandHistory[g_theDevConsole->m_commandLineNum];
				return true;
			}

		}
		if (keyCode == KEYCODE_DOWN) {
			if (!g_theDevConsole->m_commandHistory.empty()) {
				if (g_theDevConsole->m_commandLineNum < g_theDevConsole->m_commandHistory.size() - 1) {
					g_theDevConsole->m_commandLineNum++;
					g_theDevConsole->m_inputText = g_theDevConsole->m_commandHistory[g_theDevConsole->m_commandLineNum];
					return true;
				}
			}
			
		}
	}
	
	
	if (keyCode == KEYCODE_DEV_CONSOLE) {
		g_theDevConsole->ToggleMode(DevConsoleMode::OPENFULL);
	}
	return false;
}
bool DevConsole::Event_CharInput(EventArgs& args) {
	unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", -1);
	if (keyCode == KEYCODE_EQUAL) {
		keyCode = '=';
	}
	g_theDevConsole->m_inputText.insert(g_theDevConsole->m_insertionPointPosition,1, keyCode);
	g_theDevConsole->m_insertionPointPosition++;
	return true;
}
bool DevConsole::Command_Clear(EventArgs& args) {
	UNUSED(args);
	g_theDevConsole->m_lines.clear();
	return true;
}
bool DevConsole::Command_Help(EventArgs& args) {
	UNUSED(args);
	for (std::string line : g_theEventSystem->GetAllSubscribedEvent()) {
		RandomNumberGenerator rng;
		Rgba8 color((unsigned char)rng.RollRandomIntInRange(0,255), (unsigned char)rng.RollRandomIntInRange(0, 255), (unsigned char)rng.RollRandomIntInRange(0, 255));
		g_theDevConsole->AddLine(line, color);
	}
	
	return true;
}