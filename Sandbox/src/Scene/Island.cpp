#include "Island.h"

#include <imgui.h>

Island::Island()
	:m_terrain("assets/textures/Sandbox3D/heightmap.png", 300, 300, 2.0f)
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	m_playerController.GetCamera().SetProjection(60.0f, width, height, 0.01f, 1000.0f);
	m_playerController.GetCamera().SetPosition({ 0.0f, 40.0f, 0.0f });

	m_light.SetPosition({ 400.0f, 500.0f, 0.0f });

	m_terrainUVRepeat = 10.0f;
	m_ambientReduction = 0.5f;
	m_diffuseReduction = 0.9f;
	m_terrainPosition = Jass::JVec3(0.0f);

	m_isCursorDisabled = true;
	m_isFlyMode = true;
}

void Island::OnAttach()
{
	if (m_isCursorDisabled)
		Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

	LoadShaders();
	LoadTerrainTextures();
}

void Island::OnDetach()
{
	// Nothing to do here, yet
}

void Island::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::Text("WASD to move, use the mouse to rotate the camera");
	ImGui::Text("Press F to enable/disable fly mode");
	ImGui::Text("Press P to enable/disable the cursor to edit the settings");
	ImGui::InputFloat3("Position", Jass::GetPtr(m_terrainPosition));
	ImGui::InputFloat("Ambient Reduction", &m_ambientReduction, 0.05f, 0.1f);
	ImGui::InputFloat("Diffuse Reduction", &m_diffuseReduction, 0.05f, 0.1f);
	ImGui::InputFloat("UV Repeat Factor", &m_terrainUVRepeat, 0.1f, 0.5f);
	ImGui::End();
}

void Island::OnUpdate(Jass::Timestep ts)
{
	UpdateTerrain();

	if (!m_isFlyMode)
		FixCameraToTerrain();

	m_playerController.OnUpdate(ts);

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_playerController.GetCamera());

	m_terrain.Render(m_shaderLib.GetShader("TerrainMaterial"), m_light);

	Jass::Renderer::EndScene();
}

void Island::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(Island::OnKeyPressedEvent));
	m_playerController.OnEvent(e);
}

void Island::LoadShaders()
{
	m_shaderLib.Load("TerrainMaterial", "assets/shaders/DirectX11/TerrainShader.hlsl");
}

void Island::LoadTerrainTextures()
{
	m_terrain.SetBlendMap("assets/textures/Terrain/blendTest.jpg");

	m_terrain.AddTexture("assets/textures/Terrain/grass.jpg", "u_diffuseTex", 1);
	m_terrain.AddTexture("assets/textures/Terrain/grassNorm.jpg", "u_normalTex", 2);
	m_terrain.AddTexture("assets/textures/Terrain/sand.png", "u_diffuseTexR", 3);
	m_terrain.AddTexture("assets/textures/Terrain/sandNorm.png", "u_normalTexR", 4);
	m_terrain.AddTexture("assets/textures/Terrain/dirt.jpg", "u_diffuseTexG", 5);
	m_terrain.AddTexture("assets/textures/Terrain/dirtNorm.jpg", "u_normalTexG", 6);
	m_terrain.AddTexture("assets/textures/Terrain/snow.jpg", "u_diffuseTexB", 7);
	m_terrain.AddTexture("assets/textures/Terrain/snowNorm.jpg", "u_normalTexB", 8);
}

void Island::UpdateTerrain()
{
	m_terrainUVRepeat = std::max(m_terrainUVRepeat, 1.0f);

	m_terrain.SetPosition(m_terrainPosition);
	m_terrain.SetUVRepeat(m_terrainUVRepeat);

	m_ambientReduction = std::clamp(m_ambientReduction, 0.0f, 1.0f);
	m_diffuseReduction = std::clamp(m_diffuseReduction, 0.0f, 1.0f);

	m_terrain.SetAmbientReduction(m_ambientReduction);
	m_terrain.SetDiffuseReduction(m_diffuseReduction);
}

void Island::FixCameraToTerrain()
{
	auto cameraPosition = m_playerController.GetCamera().GetPosition();
	cameraPosition.y = m_terrain.GetTerrainHeight(cameraPosition.x, cameraPosition.z);
	cameraPosition.y += 20.0f;
	m_playerController.GetCamera().SetPosition(cameraPosition);
}

bool Island::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case JASS_KEY_ESCAPE:
		Jass::Application::Get().Close();
		break;
	case JASS_KEY_F:
		m_isFlyMode = !m_isFlyMode;
		break;
	case JASS_KEY_P:
		if (m_isCursorDisabled)
			Jass::Input::SetCursorMode(Jass::CursorMode::Normal);
		else
			Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

		m_isCursorDisabled = !m_isCursorDisabled;
		break;
	default:
		break;
	}

	return false;
}
