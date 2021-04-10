#include "SkyboxLoading.h"

#include <imgui.h>

SkyboxLoading::SkyboxLoading()
	:m_terrain("assets/textures/Sandbox3D/heightmap.png", 300, 300, 2.0f)
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	m_playerController.GetCamera().SetProjection(60.0f, width, height, 0.01f, 1000.0f);
	m_playerController.GetCamera().SetPosition({ 0.0f, 40.0f, 0.0f });

	m_light.SetPosition({ 400.0f, 500.0f, 0.0f });

	m_terrain.SetPosition({ -300.0f, 0.0f, 300.0f });
	m_terrain.SetUVRepeat(10.0f);

	m_skyboxTextureBlend = 0.0f;
	m_skyboxRotation = 0.0f;

	m_isCursorDisabled = true;
	m_isFlyMode = true;
}

void SkyboxLoading::OnAttach()
{
	if (m_isCursorDisabled)
		Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

	LoadShaders();
	LoadTerrainTextures();
	LoadSkyboxTextures();
}

void SkyboxLoading::OnDetach()
{
	// Nothing to do here, yet
}

void SkyboxLoading::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::Text("WASD to move, use the mouse to rotate the camera");
	ImGui::Text("Press F to enable/disable fly mode");
	ImGui::Text("Press P to enable/disable the cursor to edit the settings");
	ImGui::InputFloat("Texture blend", &m_skyboxTextureBlend, 0.1f, 0.5f);
	ImGui::End();
}

void SkyboxLoading::OnUpdate(Jass::Timestep ts)
{
	if (!m_isFlyMode)
		FixCameraToTerrain();

	m_playerController.OnUpdate(ts);

	UpdateSkybox(ts);

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_playerController.GetCamera());

	m_terrain.Render(m_shaderLib.GetShader("TerrainMaterial"), m_light);

	// Use the Render method to render the Skybox, pass the Skybox Shader and your current camera
	// ALWAYS RENDER THE SKYBOX LAST!
	m_skybox.Render(m_shaderLib.GetShader("SkyboxShader"), m_playerController.GetCamera());

	Jass::Renderer::EndScene();
}

void SkyboxLoading::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(SkyboxLoading::OnKeyPressedEvent));
	m_playerController.OnEvent(e);
}

void SkyboxLoading::LoadShaders()
{
	m_shaderLib.Load("TerrainMaterial", "assets/shaders/DirectX11/TerrainShader.hlsl");
	m_shaderLib.Load("SkyboxShader", "assets/shaders/DirectX11/SkyboxShader.hlsl");
}

void SkyboxLoading::LoadTerrainTextures()
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

void SkyboxLoading::LoadSkyboxTextures()
{
	/*
		The skybox uses Cube Map textures.
		A Cube Map is a 3D texture, as its name says, is a cube, with 6 different
		2D textures on the internal faces of a cube, so you will need 6 textures for
		a cube map, representing each face of the cube.

		Order your textures names on the vector as follows:
		1 - right
		2 - left
		3 - top
		4 - bottom
		5 - front
		6 - back

		Note:
		Maybe you will need to invert your textures vertically, maybe horizontally,
		or you may need to change the order of you textures to have a proper seamless skybox,
		it depends of you textures.
		Also, try to use images with the same bits per pixel.
	*/
	std::vector<std::string> textures
	{
		"assets/textures/Sandbox3D/Skybox/Day/right.png",
		"assets/textures/Sandbox3D/Skybox/Day/left.png",
		"assets/textures/Sandbox3D/Skybox/Day/top.png",
		"assets/textures/Sandbox3D/Skybox/Day/bottom.png",
		"assets/textures/Sandbox3D/Skybox/Day/front.png",
		"assets/textures/Sandbox3D/Skybox/Day/back.png"
	};

	std::vector<std::string> texturesNight
	{
		"assets/textures/Sandbox3D/Skybox/Night/right.png",
		"assets/textures/Sandbox3D/Skybox/Night/left.png",
		"assets/textures/Sandbox3D/Skybox/Night/top.png",
		"assets/textures/Sandbox3D/Skybox/Night/bottom.png",
		"assets/textures/Sandbox3D/Skybox/Night/front.png",
		"assets/textures/Sandbox3D/Skybox/Night/back.png"
	};

	// With the SetTexture method, you set you main texture on the texture slot 0
	m_skybox.SetTexture(textures);
	/*
		You can add more textures to the skybox, but use a different
		texture slot for each one (in this case is 1).
		The shader only supports 3 textures, but you can made some changes
		if you want to use more than 3 textures.
	*/
	m_skybox.AddTexture(texturesNight, "u_night", 1);
}

void SkyboxLoading::UpdateSkybox(Jass::Timestep ts)
{
	// You can blend from one texture to another using the BlendTextures method
	
	// Use the same texture slot number you used when you loaded the textures. The main one is always 0
	unsigned int day = 0;		// This is your first texture
	unsigned int night = 1;		// This is the second one

	// Clamp the values between 0 and 1
	m_skyboxTextureBlend = std::min(m_skyboxTextureBlend, 1.0f);
	m_skyboxTextureBlend = std::max(m_skyboxTextureBlend, 0.0f);

	/*
		The transition is from the first to the second texture.
		The blend level is a number from 0 to 1,
		0 is 100% the first texture,
		1 is 100% the second texture
	*/
	m_skybox.BlendTextures(day, night, m_skyboxTextureBlend);

	// You can also rotate the skybox to simulate the motion of the clouds

	m_skyboxRotation += 1.0f * ts;
	m_skyboxRotation = m_skyboxRotation < 360.0f ? m_skyboxRotation : 0.0f;

	m_skybox.SetRotation(m_skyboxRotation);
}

void SkyboxLoading::FixCameraToTerrain()
{
	auto cameraPosition = m_playerController.GetCamera().GetPosition();
	cameraPosition.y = m_terrain.GetTerrainHeight(cameraPosition.x, cameraPosition.z);
	cameraPosition.y += 20.0f;
	m_playerController.GetCamera().SetPosition(cameraPosition);
}

bool SkyboxLoading::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
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
