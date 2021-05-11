#include "BillboardLoading.h"

#include <imgui.h>

BillboardLoading::BillboardLoading()
	:m_terrain("assets/textures/Sandbox3D/heightmap.png", 300, 300, 2.0f)
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	m_playerController.GetCamera().SetProjection(60.0f, width, height, 0.01f, 1000.0f);
	m_playerController.GetCamera().SetPosition({ 0.0f, 40.0f, 0.0f });

	m_light.SetPosition({ 400.0f, 500.0f, 0.0f });

	m_terrain.SetPosition({ -300.0f, 0.0f, 300.0f });
	m_terrain.SetUVRepeat(10.0f);

	m_billboardPosition = { 0.0f, 40.0f, 0.0f };
	m_billboardScale = { 30.0f, 50.0f, 1.0f };
	m_billboardIsCylindrical = 1;

	m_isCursorDisabled = true;
	m_isFlyMode = true;
}

void BillboardLoading::OnAttach()
{
	if (m_isCursorDisabled)
		Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

	LoadShaders();
	LoadTerrainTextures();
	LoadBillboards();
}

void BillboardLoading::OnDetach()
{
	// Nothing to do here, yet
}

void BillboardLoading::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::Text("WASD to move, use the mouse to rotate the camera");
	ImGui::Text("Press F to enable/disable fly mode");
	ImGui::Text("Press P to enable/disable the cursor to edit the settings");
	ImGui::InputFloat3("Position", Jass::GetPtr(m_billboardPosition));
	ImGui::InputFloat3("Scale", Jass::GetPtr(m_billboardScale));
	ImGui::Text("Billboard type");
	ImGui::RadioButton("Spherical", &m_billboardIsCylindrical, 0);
	ImGui::RadioButton("Cylindrical", &m_billboardIsCylindrical, 1);
	ImGui::End();
}

void BillboardLoading::OnUpdate(Jass::Timestep ts)
{
	if (!m_isFlyMode)
		FixCameraToTerrain();

	m_playerController.OnUpdate(ts);

	UpdateBillboard();

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_playerController.GetCamera());

	m_terrain.Render(m_shaderLib.GetShader("TerrainMaterial"), m_light);
	
	// To render, pass the shader depending of the billboard type
	if (m_billboard.GetType() == BillboardType::Cylindrical)
		m_billboard.Render(m_shaderLib.GetShader("CylindricalBillboard"), m_light, m_playerController.GetCamera());
	else
		m_billboard.Render(m_shaderLib.GetShader("SphericalBillboard"), m_light, m_playerController.GetCamera());

	Jass::Renderer::EndScene();
}

void BillboardLoading::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(BillboardLoading::OnKeyPressedEvent));
	m_playerController.OnEvent(e);
}

void BillboardLoading::LoadShaders()
{
	m_shaderLib.Load("TerrainMaterial", "assets/shaders/DirectX11/TerrainShader.hlsl");
	/*
		For the billboards there are two types:

		Cylindrical: They follow the camera on the Y axis only
		Spherical: They follow the camera in every axis

		For those two types, there are two different shaders.
		The shaders also support ambient, diffuse and specular light.
	*/
	m_shaderLib.Load("SphericalBillboard", "assets/shaders/DirectX11/SphericalBillboardShader.hlsl");
	m_shaderLib.Load("CylindricalBillboard", "assets/shaders/DirectX11/CylindricalBillboardShader.hlsl");
}

void BillboardLoading::LoadTerrainTextures()
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

void BillboardLoading::LoadBillboards()
{
	/*
		The only thing you need to do is to assign a diffuse texture (with transparency)
		and an optional normal map texture. This is done through the material of the billboard.
	*/
	m_billboard.GetMaterial().SetDiffuseTexture("assets/textures/Billboard/arbol.png");
	m_billboard.GetMaterial().SetNormalTexture("assets/textures/Billboard/arbolNormal.png");
	// You can also change the type of billboard. The default value is Spherical.
	if (m_billboardIsCylindrical)
		m_billboard.SetType(BillboardType::Cylindrical);
}

void BillboardLoading::UpdateBillboard()
{
	// It is also possible to change the billboard scale and position;
	m_billboard.SetPosition(m_billboardPosition);
	m_billboard.SetScale(m_billboardScale);

	if (m_billboardIsCylindrical)
		m_billboard.SetType(BillboardType::Cylindrical);
	else
		m_billboard.SetType(BillboardType::Spherical);
}

void BillboardLoading::FixCameraToTerrain()
{
	auto cameraPosition = m_playerController.GetCamera().GetPosition();
	cameraPosition.y = m_terrain.GetTerrainHeight(cameraPosition.x, cameraPosition.z);
	cameraPosition.y += 20.0f;
	m_playerController.GetCamera().SetPosition(cameraPosition);
}

bool BillboardLoading::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
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
