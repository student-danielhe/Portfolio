#include "Game2DCurve.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
void Game2DCurve::StartUp() {
	m_clock = new Clock();
	m_timer = new Timer(1.f);
	g_ScreenCamera = new Camera();
	g_ScreenCamera->SetOrthoView(Vec2(0, 0), Vec2(WORLD_SIZE_X, WORLD_SIZE_Y));
	m_timer->Start();
	g_RNG = new RandomNumberGenerator();
	RandomStart();

}
void Game2DCurve::Update(float deltaSeconds) {
	Clock::TickSystemClock();
	bool nextspline = m_timer->DecrementPeriodIfElapsed();
	if (nextspline) {
		m_currentSplineSection++;
		m_currentSplineSection = m_currentSplineSection % m_spline.size();
	}
	HandleKeyPress(deltaSeconds);
}
void Game2DCurve::Render() {
	g_theRenderer->BeginCamera(*g_ScreenCamera);
	RenderEase();
	RenderBeizer();
	RenderSpline();
	g_theRenderer->EndCamera(*g_ScreenCamera);
}
void Game2DCurve::HandleKeyPress(float deltaSecond) {
	if (g_theInput->WasKeyJustReleased('Q')) {
		m_currentMode--;
		if (m_currentMode < 0) {
			m_currentMode = (int)EasingMode::Count-1;
		}

	}if (g_theInput->WasKeyJustReleased('E')) {
		m_currentMode++;
		m_currentMode = m_currentMode % (int)EasingMode::Count;
	}if (g_theInput->WasKeyJustReleased('N')) {
		if (m_numSplit > 1) {
			m_numSplit /= 2;
		}
	}if (g_theInput->WasKeyJustReleased('M')) {
		m_numSplit *= 2;
	}
	bool slow = g_theInput->WasKeyJustPressed('T');
	if (slow) {
		m_clock->SetTimeScale(.1f);
	}
	else {
		m_clock->SetTimeScale(1.f);
	}
}

void Game2DCurve::RenderEase() {
	float t = (float)m_timer->GetElapsedTime();
	std::vector<Vertex_PCU> verts;
	std::string name="";
	float point =0.f;
	AddVertsForRect(verts, Vec2(0.f, 50.f), Vec2(50.f, 50.f), Vec2(50.f, 0.f), Vec2(0.f, 0.f),Rgba8(100,100,150),Vec2());
	if (m_currentMode == (int)EasingMode::SMOOTH_START2) {
		point = SmoothStart2(t) * 50.f;
		name = "Smooth Start2";
	}
	if (m_currentMode == (int)EasingMode::SMOOTH_START3) {
		point = SmoothStart3(t) * 50.f;
		name = "Smooth Start3";
	}
	if (m_currentMode == (int)EasingMode::SMOOTH_START4) {
		point = SmoothStart4(t) * 50.f;
		name = "Smooth Start4";
	}
	if (m_currentMode == (int)EasingMode::SMOOTH_START5) {
		point = SmoothStart5(t) * 50.f;
		name = "Smooth Start5";
	}
	if (m_currentMode == (int)EasingMode::SMOOTH_START6) {
		point = SmoothStart6(t) * 50.f;
		name = "Smooth Start6";

	}

	if (m_currentMode == (int)EasingMode::SMOOTH_STOP2) {
		point = SmoothStop2(t) * 50.f;
		name = "Smooth Stop2";
	} 
	if (m_currentMode == (int)EasingMode::SMOOTH_STOP3) {
		point = SmoothStop3(t) * 50.f;
		name = "Smooth Stop3";
	}		   
	if (m_currentMode == (int)EasingMode::SMOOTH_STOP4) {
		point = SmoothStop4(t) * 50.f;
		name = "Smooth Stop4";
	}
	if (m_currentMode == (int)EasingMode::SMOOTH_STOP5) {
		point = SmoothStop5(t) * 50.f;
		name = "Smooth Stop5";
	}
	if (m_currentMode == (int)EasingMode::SMOOTH_STOP6) {
		point = SmoothStop6(t) * 50.f;
		name = "Smooth Stop6";
	}

	if (m_currentMode == (int)EasingMode::SMOOTH_STEP3) {
		point = SmoothStep3(t) * 50.f;
		name = "Smooth Step3";
	}
	if (m_currentMode == (int)EasingMode::SMOOTH_STEP5) {
		point = SmoothStep5(t) * 50.f;
		name = "Smooth Step5";
	}
	if (m_currentMode == (int)EasingMode::HESITATE3) {
		point = Hesitate3(t) * 50.f;
		name = "Hesitate3";
	}
	if (m_currentMode == (int)EasingMode::HESITATE5) {
		point = Hesitate5(t) * 50.f;
		name = "Hesitate5";
	}

	if (m_currentMode == (int)EasingMode::CUSTOM_FUNKY) {
		point = CustomFunkyEasingFunction(t) * 50.f;
		name = "Custom Funky";
	}

	if (m_currentMode == (int)EasingMode::INDENTITY) {
		point = t * 50.f;
		name = "Identity";
	}

	for (int i = 0; i < 32; i++) {
		float lineSeg1 = 0.f;
		float lineSeg2 = 0.f;
		if (m_currentMode == (int)EasingMode::SMOOTH_START2) {
			lineSeg1 = SmoothStart2((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStart2((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::SMOOTH_START3) {
			lineSeg1 = SmoothStart3((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStart3((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::SMOOTH_START4) {
			lineSeg1 = SmoothStart4((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStart4((float)(i + 1) / 32.f) * 50.f;
		}if (m_currentMode == (int)EasingMode::SMOOTH_START5) {
			lineSeg1 = SmoothStart5((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStart5((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::SMOOTH_START6) {
			lineSeg1 = SmoothStart6((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStart6((float)(i + 1) / 32.f) * 50.f;
		}

		if (m_currentMode == (int)EasingMode::SMOOTH_STOP2) {
			lineSeg1 = SmoothStop2((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStop2((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::SMOOTH_STOP3) {
			lineSeg1 = SmoothStop3((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStop3((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::SMOOTH_STOP4) {
			lineSeg1 = SmoothStop4((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStop4((float)(i + 1) / 32.f) * 50.f;
		}if (m_currentMode == (int)EasingMode::SMOOTH_STOP5) {
			lineSeg1 = SmoothStop5((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStop5((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::SMOOTH_STOP6) {
			lineSeg1 = SmoothStop6((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStop6((float)(i + 1) / 32.f) * 50.f;
		}

		if (m_currentMode == (int)EasingMode::SMOOTH_STEP3) {
			lineSeg1 = SmoothStep3((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStep3((float)(i + 1) / 32.f) * 50.f;
		}
        if (m_currentMode == (int)EasingMode::SMOOTH_STEP5) {
			lineSeg1 = SmoothStep5((float)i / 32.f) * 50.f;
			lineSeg2 = SmoothStep5((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::HESITATE3) {
			lineSeg1 = Hesitate3((float)i / 32.f) * 50.f;
			lineSeg2 = Hesitate3((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::HESITATE5) {
			lineSeg1 = Hesitate5((float)i / 32.f) * 50.f;
			lineSeg2 = Hesitate5((float)(i + 1) / 32.f) * 50.f;
		}

		if (m_currentMode == (int)EasingMode::CUSTOM_FUNKY) {
			lineSeg1 = CustomFunkyEasingFunction((float)i / 32.f) * 50.f;
			lineSeg2 = CustomFunkyEasingFunction((float)(i + 1) / 32.f) * 50.f;
		}
		if (m_currentMode == (int)EasingMode::INDENTITY) {
			lineSeg1 = (float)i / 32.f * 50.f;
			lineSeg2 = (float)(i + 1) / 32.f * 50.f;
		}

		AddVertsForLine(verts, Vec2((float)i / 32.f * 50.f,lineSeg1) ,
			Vec2((float)(i + 1) / 32.f * 50.f, lineSeg2), Rgba8::GREEN, Vec2());
	}
	AddVertsForDisc(verts, Vec2(t * 50.f, point), 3.f, Rgba8::GREEN, Vec2());
	
	for (Vertex_PCU& vert : verts) {
		vert.m_position += Vec3(40.f,120.f,0.f);
	}
	g_theRenderer->DrawVertexArray(verts);
	verts.clear();
	BitmapFont* font = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	font->AddVertsForText2D(verts, Vec2(30.f,100.f), 5.f, name);
	g_theRenderer->BindTexture(&font->GetTexture());
	g_theRenderer->DrawVertexArray(verts);
	g_theRenderer->BindTexture(nullptr);
}
void Game2DCurve::RenderBeizer() {
	float t = (float)m_timer->GetElapsedTime();
	Vec2 parametric = m_curve->EvaluateAtParametric(t);
	Vec2 dist = m_curve->EvaluateAtApproximateDistance(t * m_curve->GetApproximateLength(m_numSplit),m_numSplit);
	std::vector<Vertex_PCU> verts;
	RenderCurve(verts, *m_curve);
	AddVertsForDisc(verts, parametric, 3.f, Rgba8::WHITE, Vec2());
	AddVertsForDisc(verts, dist, 3.f, Rgba8::GREEN, Vec2());
	g_theRenderer->DrawVertexArray(verts);
}
void Game2DCurve::RenderSpline() {
	float t = (float)m_timer->GetElapsedTime();
	std::vector<Vertex_PCU> verts;
	for (CubicBeizerCurve2D curve : m_spline) {
		RenderCurve(verts, curve);
	}
	CubicBeizerCurve2D curve = m_spline[m_currentSplineSection];
	Vec2 parametric = curve.EvaluateAtParametric(t);
	Vec2 dist = curve.EvaluateAtApproximateDistance(t*curve.GetApproximateLength(m_numSplit),m_numSplit);
	AddVertsForDisc(verts, parametric, 3.f, Rgba8::WHITE, Vec2());
	AddVertsForDisc(verts, dist, 3.f, Rgba8::GREEN, Vec2());
	for (int i = 1; i < m_splineHermite.size(); i++) {
		CubicHermiteCurve hermite = m_splineHermite[i];
		AddVertsForArrow2D(verts, hermite.m_startPos, hermite.m_startPos + hermite.m_startVelocity, 3.f, Rgba8::RED);
	}
	g_theRenderer->DrawVertexArray(verts);
}

void Game2DCurve::RandomStart() {
	//normal curve
	m_curve = new CubicBeizerCurve2D(Vec2(g_RNG->RollRandomFloatInRange(250, 350), g_RNG->RollRandomFloatInRange(120, 180)),
		                             Vec2(g_RNG->RollRandomFloatInRange(250, 350), g_RNG->RollRandomFloatInRange(120, 180)),
		                             Vec2(g_RNG->RollRandomFloatInRange(250, 350), g_RNG->RollRandomFloatInRange(120, 180)),
		                             Vec2(g_RNG->RollRandomFloatInRange(250, 350), g_RNG->RollRandomFloatInRange(120, 180)));

	int splineSections = g_RNG->RollRandomIntInRange(5, 8);
	std::vector<Vec2> splinePoints;
	for (int i = 0; i < splineSections; i++) {
		splinePoints.push_back(Vec2(g_RNG->RollRandomFloatInRange(100,300),g_RNG->RollRandomFloatInRange(20,80)));
	}
	m_spline = CubicBeizerCurve2D::MakeCatmullRom(splinePoints, m_splineHermite);
}

void Game2DCurve::RenderCurve(std::vector<Vertex_PCU>& verts, CubicBeizerCurve2D curve) {




	//
	AddVertsForLine(verts, m_curve->m_startPos, m_curve->m_guidePos1 , Rgba8(0,0,100), Vec2(), .5f);
	AddVertsForLine(verts, m_curve->m_guidePos1, m_curve->m_guidePos2, Rgba8(0,0,100), Vec2(), .5f);
	AddVertsForLine(verts, m_curve->m_guidePos2, m_curve->m_endPos,    Rgba8(0,0,100), Vec2(), .5f);
	
	AddVertsForCubicBeizer(verts, curve, Rgba8(100, 100, 100), 64);
	AddVertsForCubicBeizer(verts, curve, Rgba8::GREEN, m_numSplit);

	AddVertsForDisc(verts, m_curve->m_startPos,  3.f, Rgba8::BLUE, Vec2());
	AddVertsForDisc(verts, m_curve->m_guidePos1, 3.f, Rgba8::BLUE, Vec2());
	AddVertsForDisc(verts, m_curve->m_guidePos2, 3.f, Rgba8::BLUE, Vec2());
	AddVertsForDisc(verts, m_curve->m_endPos,    3.f, Rgba8::BLUE, Vec2());

}