#include "TerrainLoading.h"

#include <imgui.h>

/*
	You will need to initialize the terrain here, as the Terrain class does not have a default
	parameterless constructor.
	The first argument is the filename of a height map texture.
	The second and third arguments are the width and depth of the terrain,
	this is the number of vertices it will have on the x and z axes respectively,
	the higher the number, the higher the resolution of the terrain.
	The last parameter is the step or distance between each vertex of the terrain,
	the total width and depth of the terrain in world coordinates is:
	totalWidth = (width - 1) * step
	totalDepth = (depth - 1) * step
*/
TerrainLoading::TerrainLoading()
	:m_terrain("assets/textures/Sandbox3D/heightmap.png", 300, 300, 2.0f)
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	// You can get the internal camera of the controller with the GetCamera method and set its settings
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

void TerrainLoading::OnAttach()
{
	/*
		Set the cursor to disable, so you can use the player controller
		without getting the cursor out of window.
	*/
	if (m_isCursorDisabled)
		Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

	LoadShaders();
	LoadTerrainTextures();
}

void TerrainLoading::OnDetach()
{
	// Nothing to do here, yet
}

void TerrainLoading::OnImGuiRender()
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

void TerrainLoading::OnUpdate(Jass::Timestep ts)
{
	UpdateTerrain();

	if (!m_isFlyMode)
		FixCameraToTerrain();

	// You need to update the controller with the OnUpdate method
	m_playerController.OnUpdate(ts);

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_playerController.GetCamera());
	
	// Render the terrain with the Render method, pass the TerrainMaterial shader and the current light
	m_terrain.Render(m_shaderLib.GetShader("TerrainMaterial"), m_light);

	Jass::Renderer::EndScene();
}

void TerrainLoading::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(TerrainLoading::OnKeyPressedEvent));
	// The player controller has an event dispatcher inside, so you need to pass the event to it
	m_playerController.OnEvent(e);
}

void TerrainLoading::LoadShaders()
{
	m_shaderLib.Load("TerrainMaterial", "assets/shaders/DirectX11/TerrainShader.hlsl");
}

void TerrainLoading::LoadTerrainTextures()
{
	/*
		Set the Terrain Blend Map. The Blend is the textures which indicates how the
		different textures blend together.
	*/
	m_terrain.SetBlendMap("assets/textures/Terrain/blendTest.jpg");

	/*
		Add the textures for the RGB channels of the blend map.
		You can also specify a normal texture for each diffuse texture.
		Use a different slot and uniform name for each one.
		As you can see in the following code, for each channel there are two uniforms:
		u_diffuseTex + Channel
		u_normalTex + Channel
		Ex. u_diffuseTexR is the diffuse texture for the red channel of the blend map
		The default texture is labeled with no channel at the end. This is the texture
		for the black color of the blend map.
	*/
	m_terrain.AddTexture("assets/textures/Terrain/grass.jpg", "u_diffuseTex", 1);
	m_terrain.AddTexture("assets/textures/Terrain/grassNorm.jpg", "u_normalTex", 2);
	m_terrain.AddTexture("assets/textures/Terrain/sand.png", "u_diffuseTexR", 3);
	m_terrain.AddTexture("assets/textures/Terrain/sandNorm.png", "u_normalTexR", 4);
	m_terrain.AddTexture("assets/textures/Terrain/dirt.jpg", "u_diffuseTexG", 5);
	m_terrain.AddTexture("assets/textures/Terrain/dirtNorm.jpg", "u_normalTexG", 6);
	m_terrain.AddTexture("assets/textures/Terrain/snow.jpg", "u_diffuseTexB", 7);
	m_terrain.AddTexture("assets/textures/Terrain/snowNorm.jpg", "u_normalTexB", 8);
}

void TerrainLoading::UpdateTerrain()
{
	// Set a min value of 1
	m_terrainUVRepeat = std::max(m_terrainUVRepeat, 1.0f);
	
	// You can change the terrain position and the UV repeat factor of the textures
	m_terrain.SetPosition(m_terrainPosition);
	m_terrain.SetUVRepeat(m_terrainUVRepeat);

	// Clamp the values between 0 and 1
	m_ambientReduction = std::clamp(m_ambientReduction, 0.0f, 1.0f);
	m_diffuseReduction = std::clamp(m_diffuseReduction, 0.0f, 1.0f);

	// You can also modify the ambient and diffuse reduction
	m_terrain.SetAmbientReduction(m_ambientReduction);
	m_terrain.SetDiffuseReduction(m_diffuseReduction);
}

void TerrainLoading::FixCameraToTerrain()
{
	// This is  way in which you can implement terrain collision
	auto cameraPosition = m_playerController.GetCamera().GetPosition();
	// The GetTerrainHeight method returns the height of the terrain in a given world position
	cameraPosition.y = m_terrain.GetTerrainHeight(cameraPosition.x, cameraPosition.z);
	// Add certain height (the player height)
	cameraPosition.y += 20.0f;
	m_playerController.GetCamera().SetPosition(cameraPosition);
}

bool TerrainLoading::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
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
		// Activate the cursor so you can change the settings as needed
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
