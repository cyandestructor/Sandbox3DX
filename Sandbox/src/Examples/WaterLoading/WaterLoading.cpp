#include "WaterLoading.h"

#include <imgui.h>

/*
	The Water should be initialized in the constructor as follows.
	The arguments are:
	1- A Scale in X and Z of the water plane in world coordinates
	2- The Water plane divisions in Width
	3- The Water plane divisions in Depth
	4- The Window Screen Width
	5- The Window Screen Height
*/
WaterLoading::WaterLoading()
	:m_terrain("assets/textures/Sandbox3D/heightmap.png", 300, 300, 2.0f),
	m_water(
		{ 600.0f, 600.0f },
		10, 10,
		Jass::Application::Get().GetWindow().GetWidth(),
		Jass::Application::Get().GetWindow().GetHeight())
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	m_playerController.GetCamera().SetProjection(60.0f, width, height, 0.01f, 1000.0f);
	m_playerController.GetCamera().SetPosition({ 0.0f, 40.0f, 0.0f });

	m_light.SetPosition({ 400.0f, 500.0f, 0.0f });

	m_terrain.SetPosition({ -300.0f, 0.0f, 300.0f });
	m_terrain.SetUVRepeat(10.0f);

	m_skyboxRotation = 0.0f;

	m_ambientReduction = 0.7f;

	m_waterMotion = 0.0f;
	m_waterMotionSpeed = 0.03f;
	m_waterReflectivity = 0.5f;
	m_waterShineDamper = 10.0f;
	m_tilingFactor = 7.0f;
	m_waterDistortion = 0.02f;

	m_waterPosition = { 0.0f, 25.0f, 0.0f };
	m_waterColor = { 0.2f, 0.6f, 0.8f, 1.0f };

	m_isCursorDisabled = true;
	m_isFlyMode = true;
}

void WaterLoading::OnAttach()
{
	if (m_isCursorDisabled)
		Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

	LoadShaders();
	LoadTerrainTextures();
	LoadSkyboxTextures();
	LoadModels();
	LoadWater();
}

void WaterLoading::OnDetach()
{
	// Nothing to do here, yet
}

void WaterLoading::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::Text("WASD to move, use the mouse to rotate the camera");
	ImGui::Text("Press F to enable/disable fly mode");
	ImGui::Text("Press P to enable/disable the cursor to edit the settings");
	ImGui::InputFloat3("Position", Jass::GetPtr(m_waterPosition));
	ImGui::ColorEdit4("Color", Jass::GetPtr(m_waterColor));
	ImGui::InputFloat("Distortion", &m_waterDistortion, 0.005f, 0.01f);
	ImGui::InputFloat("Motion speed", &m_waterMotionSpeed, 0.005f, 0.01f);
	ImGui::InputFloat("Reflectivity", &m_waterReflectivity, 0.05f, 0.1f);
	ImGui::InputFloat("Shine damper", &m_waterShineDamper, 0.05f, 0.1f);
	ImGui::InputFloat("Tile factor", &m_tilingFactor, 0.05f, 0.1f);
	ImGui::InputFloat("Ambient reduction", &m_ambientReduction, 0.05f, 0.1f);
	ImGui::End();

	ClampValues();
}

void WaterLoading::OnUpdate(Jass::Timestep ts)
{
	if (!m_isFlyMode)
		FixCameraToTerrain();

	m_playerController.OnUpdate(ts);

	UpdateTerrain();
	UpdateSkybox(ts);
	UpdateWater(ts);
	UpdateModel();

	

	// Before rendering the whole scene, first it is needed to prepare the water reflection and refraction
	PrepareWaterReflection(ts);
	//PrepareWaterRefraction(ts);

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_playerController.GetCamera());
	
	// In this we want to separate the render of the complete scene in another method for a clean code
	RenderScene(ts);

	Jass::Renderer::EndScene();
}

void WaterLoading::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(WaterLoading::OnKeyPressedEvent));
	dispatcher.Dispatch<Jass::WindowResizeEvent>(BIND_EVENT_FN(WaterLoading::OnWindowResizeEvent));
	m_playerController.OnEvent(e);
}

void WaterLoading::LoadShaders()
{
	m_shaderLib.Load("TerrainMaterial", "assets/shaders/DirectX11/TerrainShader.hlsl");
	m_shaderLib.Load("SkyboxShader", "assets/shaders/DirectX11/SkyboxShader.hlsl");
	m_shaderLib.Load("NormalsMaterial", "assets/shaders/DirectX11/NormalsMaterial.hlsl");

	// To render the water, use the Water shader
	m_shaderLib.Load("WaterMaterial", "assets/shaders/DirectX11/WaterShader.hlsl");
}

void WaterLoading::LoadModels()
{
	m_model.Load("assets/models/Lantern/lantern.obj");
	m_model.GetMaterial().SetDiffuseTexture("assets/models/Lantern/diffuse.jpg");
	m_model.GetMaterial().SetNormalTexture("assets/models/Lantern/normal.png");
	m_model.SetScale({ 12.0f, 12.0f, 12.0f });
	m_model.SetPosition({ 0.0f, 50.0f, 0.0f });
}

void WaterLoading::LoadTerrainTextures()
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

void WaterLoading::LoadSkyboxTextures()
{
	std::vector<std::string> textures
	{
		"assets/textures/Sandbox3D/Skybox/Day/right.png",
		"assets/textures/Sandbox3D/Skybox/Day/left.png",
		"assets/textures/Sandbox3D/Skybox/Day/top.png",
		"assets/textures/Sandbox3D/Skybox/Day/bottom.png",
		"assets/textures/Sandbox3D/Skybox/Day/front.png",
		"assets/textures/Sandbox3D/Skybox/Day/back.png"
	};

	m_skybox.SetTexture(textures);
}

void WaterLoading::LoadWater()
{
	// The water is loaded on the constructor, but here we give it its textures

	// The dudv map is a distortion map, the normal map is used for specular highlights
	m_water.SetTextures("assets/textures/Water/dudv.png", "assets/textures/Water/normal.png");
}

void WaterLoading::RenderScene(Jass::Timestep ts)
{
	m_shaderLib.GetShader("NormalsMaterial")->Bind();
	m_shaderLib.GetShader("NormalsMaterial")->SetFloat3("u_cameraPosition", m_playerController.GetCamera().GetPosition());
	
	m_model.Render(m_shaderLib.GetShader("NormalsMaterial"), m_light);

	m_terrain.Render(m_shaderLib.GetShader("TerrainMaterial"), m_light);

	// Render the water. Pass the Water Shader, the light and the current camera
	m_water.Render(m_shaderLib.GetShader("WaterMaterial"), m_light, m_playerController.GetCamera());

	// ALWAYS RENDER THE SKYBOX LAST!
	//m_skybox.Render(m_shaderLib.GetShader("SkyboxShader"), m_playerController.GetCamera());
}

void WaterLoading::UpdateTerrain()
{
	m_terrain.SetAmbientReduction(m_ambientReduction);
}

void WaterLoading::UpdateSkybox(Jass::Timestep ts)
{
	m_skyboxRotation += 1.0f * ts;
	m_skyboxRotation = m_skyboxRotation < 360.0f ? m_skyboxRotation : 0.0f;

	m_skybox.SetRotation(m_skyboxRotation);
}

void WaterLoading::UpdateModel()
{
	m_model.GetMaterial().SetAmbientReduction(m_ambientReduction);
}

void WaterLoading::ClampValues()
{
	m_ambientReduction = std::clamp(m_ambientReduction, 0.0f, 1.0f);
	m_waterMotionSpeed = std::clamp(m_waterMotionSpeed, 0.0f, 1.0f);
	m_waterReflectivity = std::max(m_waterReflectivity, 0.0f);
	m_waterShineDamper = std::max(m_waterShineDamper, 0.0f);
	m_tilingFactor = std::max(m_tilingFactor, 0.0f);
	m_waterDistortion = std::max(m_waterDistortion, 0.0f);
}

void WaterLoading::UpdateWater(Jass::Timestep ts)
{
	// You can modify all these properties of the water

	m_water.SetPosition(m_waterPosition);

	m_water.SetColor(m_waterColor);
	m_water.SetAmbientReduction(m_ambientReduction);
	m_water.SetSpecularProperties(m_waterReflectivity, m_waterShineDamper);
	
	m_water.SetDistortionFactor(m_waterDistortion);

	m_water.SetTilingFactor(m_tilingFactor);

	m_waterMotion += m_waterMotionSpeed * ts;
	m_waterMotion = m_waterMotion > 1.0f ? 0.0f : m_waterMotion;

	m_water.SetMotionFactor(m_waterMotion);
}

void WaterLoading::PrepareWaterReflection(Jass::Timestep ts)
{
	auto cameraPosition = m_playerController.GetCamera().GetPosition();
	// The pitch is the camera angle for up/down direction
	float pitch = m_playerController.GetPitch();

	// Move the camera below the water and inverse its pitch to get the reflection
	Jass::JVec3 reflectionCameraPosition = cameraPosition;
	float distance = 2.0f * (cameraPosition.y - m_water.GetPosition().y);
	float inversePitch = -pitch;
	reflectionCameraPosition.y -= distance;
	m_playerController.GetCamera().SetPosition(reflectionCameraPosition);
	m_playerController.SetPitch(inversePitch);
	m_playerController.OnUpdate(ts);

	// We begin another scene to render the water reflection scene
	Jass::Renderer::BeginScene(m_playerController.GetCamera());

	// Begin and End the reflection scene with the appropiate methods
	m_water.BeginReflection();
	
	/*
		Enable the clip plane. A Clip plane is a plane
		which defines which geometry should be rendered and which not.
		This is for optimization, we do not want to render something that will not be visible
	*/
	Jass::RenderCommand::EnableClipDistance(true);
	// Render the water scene. Pass a clip plane to only render what is above the water surface
	RenderWaterScene(ts, { 0.0f, 1.0f, 0.0f, -m_water.GetPosition().y });
	Jass::RenderCommand::EnableClipDistance(false);
	
	// Remember to render the skybox last
	//m_skybox.Render(m_shaderLib.GetShader("SkyboxShader"), m_playerController.GetCamera());
	
	m_water.EndReflection();

	Jass::Renderer::EndScene();

	// Restore the original camera properties
	m_playerController.GetCamera().SetPosition(cameraPosition);
	m_playerController.SetPitch(pitch);
	m_playerController.OnUpdate(ts);
}

void WaterLoading::PrepareWaterRefraction(Jass::Timestep ts)
{
	// We begin another scene to render the water refraction scene
	Jass::Renderer::BeginScene(m_playerController.GetCamera());

	// Begin and End the refraction scene with the appropiate methods
	m_water.BeginRefraction();

	/*
		Enable the clip plane. A Clip plane is a plane
		which defines which geometry should be rendered and which not.
		This is for optimization, we do not want to render something that will not be visible
	*/
	Jass::RenderCommand::EnableClipDistance(true);
	// Render the water scene. Pass a clip plane to only render what is below the water surface
	RenderWaterScene(ts, { 0.0f, -1.0f, 0.0f, m_water.GetPosition().y + 1.0f });
	Jass::RenderCommand::EnableClipDistance(false);

	// Remember to render the skybox last
	//m_skybox.Render(m_shaderLib.GetShader("SkyboxShader"), m_playerController.GetCamera());
	
	m_water.EndRefraction();

	Jass::Renderer::EndScene();
}

void WaterLoading::RenderWaterScene(Jass::Timestep ts, const Jass::JVec4& clipPlane)
{
	/*
		In the water scene we render everything we want to have in the water's reflection and refraction.
		Note that we do not render the water here, because that would cycle the rendering
	*/
	m_shaderLib.GetShader("NormalsMaterial")->Bind();
	m_shaderLib.GetShader("NormalsMaterial")->SetFloat3("u_cameraPosition", m_playerController.GetCamera().GetPosition());

	// We render using the clip plane, pass it as the last parameter on models and the terrain
	m_model.Render(m_shaderLib.GetShader("NormalsMaterial"), m_light, clipPlane);

	m_terrain.Render(m_shaderLib.GetShader("TerrainMaterial"), m_light, clipPlane);
}

void WaterLoading::FixCameraToTerrain()
{
	auto cameraPosition = m_playerController.GetCamera().GetPosition();
	cameraPosition.y = m_terrain.GetTerrainHeight(cameraPosition.x, cameraPosition.z);
	cameraPosition.y += 20.0f;
	m_playerController.GetCamera().SetPosition(cameraPosition);
}

bool WaterLoading::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
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

bool WaterLoading::OnWindowResizeEvent(Jass::WindowResizeEvent& e)
{
	m_water.OnWindowResizeEvent(e);
	return false;
}
