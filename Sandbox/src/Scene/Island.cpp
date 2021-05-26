#include "Island.h"

#include <imgui.h>

Island::Island()
	:m_terrain("assets/textures/IslandTerrain/heightmap.png", 300, 300, 4.0f)
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	m_playerController.GetCamera().SetProjection(60.0f, width, height, 0.01f, 1000.0f);
	m_playerController.GetCamera().SetPosition({ 0.0f, 40.0f, 0.0f });

	m_light.SetPosition({ 400.0f, 500.0f, 0.0f });

	m_terrain.SetPosition({ -600.0f, 0.0f, 600.0f });
	m_terrain.SetUVRepeat(10.0f);
	m_terrain.SetAmbientReduction(0.5f);
	m_terrain.SetDiffuseReduction(0.9f);

	m_isCursorDisabled = true;
	m_isFlyMode = true;
}

void Island::OnAttach()
{
	if (m_isCursorDisabled)
		Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

	LoadModels();
	LoadShaders();
	LoadTerrainTextures();
}

void Island::OnDetach()
{
	// Nothing to do here, yet
}

void Island::OnImGuiRender()
{
	// Nothing to do here
}

void Island::OnUpdate(Jass::Timestep ts)
{
	if (!m_isFlyMode)
		FixCameraToTerrain();

	m_playerController.OnUpdate(ts);

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_playerController.GetCamera());

	RenderScene(ts);

	Jass::Renderer::EndScene();
}

void Island::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(Island::OnKeyPressedEvent));
	m_playerController.OnEvent(e);
}

void Island::RenderScene(Jass::Timestep ts)
{
	for (const auto& model : m_sceneModels) {
		model.Render(m_shaderLib.GetShader("ModelMaterial"), m_light);
	}

	m_terrain.Render(m_shaderLib.GetShader("TerrainMaterial"), m_light);
}

void Island::LoadShaders()
{
	m_shaderLib.Load("TerrainMaterial", "assets/shaders/DirectX11/TerrainShader.hlsl");
	m_shaderLib.Load("ModelMaterial", "assets/shaders/DirectX11/NormalsMaterial.hlsl");
}

void Island::LoadTerrainTextures()
{
	m_terrain.SetBlendMap("assets/textures/IslandTerrain/blendmap.png");

	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_2/color.jpg", "u_diffuseTex", 1);
	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_2/normal.jpg", "u_normalTex", 2);
	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_1/color.jpg", "u_diffuseTexR", 3);
	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_1/normal.jpg", "u_normalTexR", 4);
	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_5/color.jpg", "u_diffuseTexG", 5);
	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_5/normal.jpg", "u_normalTexG", 6);
	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_4/color.jpg", "u_diffuseTexB", 7);
	m_terrain.AddTexture("assets/textures/IslandTerrain/sand_4/normal.jpg", "u_normalTexB", 8);
}

void Island::LoadModels()
{
	Jass::JVec3 sceneScale = { 0.5f, 0.5, 0.5f };

	Model cabin;
	cabin.Load("assets/models/Island/Cabin-a/cabin-a.obj");
	cabin.SetPosition({ 0.0f, 45.0f, 0.0f });
	cabin.SetScale(sceneScale);
	cabin.GetMaterial().SetDiffuseTexture("assets/models/Island/Cabin-a/diffuse.png");
	cabin.GetMaterial().SetNormalTexture("assets/models/Island/Cabin-a/normal.png");
	cabin.GetMaterial().SetSpecularTexture("assets/models/Island/Cabin-a/specular.png");
	cabin.GetMaterial().SetSpecularSettings(5.0f, 10.0f);
	m_sceneModels.push_back(cabin);

	Model cabinB;
	cabinB.SetPosition({ 0.0f, 50.0f, 0.0f });
	cabinB.Load("assets/models/Island/Cabin-b/cabin-b.obj");
	cabinB.SetScale(sceneScale);
	cabinB.GetMaterial().SetDiffuseTexture("assets/models/Island/Cabin-b/diffuse.jpg");
	cabinB.GetMaterial().SetNormalTexture("assets/models/Island/Cabin-b/normal.jpg");
	m_sceneModels.push_back(cabinB);

	return;

	Model dock;
	dock.SetPosition({ 0.0f, 30.0f, 0.0f });
	dock.Load("assets/models/Island/Dock/dock.obj");
	dock.SetScale(sceneScale);
	dock.GetMaterial().SetDiffuseTexture("assets/models/Island/Dock/diffuse.jpg");
	dock.GetMaterial().SetNormalTexture("assets/models/Island/Dock/normal.jpg");
	dock.GetMaterial().SetSpecularTexture("assets/models/Island/Dock/specular.jpg");
	dock.GetMaterial().SetSpecularSettings(2.0f, 10.0f);
	m_sceneModels.push_back(dock);

	Model boat;
	boat.SetPosition({ 0.0f, 30.0f, 0.0f });
	boat.Load("assets/models/Island/Boat-a/boat-a.obj");
	boat.SetScale(sceneScale);
	boat.GetMaterial().SetDiffuseTexture("assets/models/Island/Boat-a/diffuse.png");
	boat.GetMaterial().SetNormalTexture("assets/models/Island/Boat-a/normal.png");
	boat.GetMaterial().SetSpecularTexture("assets/models/Island/Boat-a/specular.png");
	boat.GetMaterial().SetSpecularSettings(5.0f, 10.0f);
	m_sceneModels.push_back(boat);

	Model boatB;
	boatB.SetPosition({ 0.0f, 30.0f, 0.0f });
	boatB.Load("assets/models/Island/Boat-b/boat-b.obj");
	boatB.SetScale(sceneScale);
	boatB.GetMaterial().SetDiffuseTexture("assets/models/Island/Boat-b/diffuse.jpg");
	boatB.GetMaterial().SetNormalTexture("assets/models/Island/Boat-b/normal.jpg");
	m_sceneModels.push_back(boatB);

	// Coconut
	{
		Model trunk;
		trunk.SetPosition({ 0.0f, 45.0f, 0.0f });
		trunk.Load("assets/models/Island/Coconut/coconut-trunk.obj");
		trunk.SetScale(sceneScale);
		trunk.GetMaterial().SetDiffuseTexture("assets/models/Island/Coconut/trunk-diffuse.png");
		trunk.GetMaterial().SetNormalTexture("assets/models/Island/Coconut/trunk-normal.png");
		m_sceneModels.push_back(trunk);

		Model leaves;
		leaves.SetPosition({ 0.0f, 45.0f, 0.0f });
		leaves.Load("assets/models/Island/Coconut/coconut-leaves.obj");
		leaves.SetScale(sceneScale);
		leaves.GetMaterial().SetDiffuseTexture("assets/models/Island/Coconut/leaf-diffuse.png");
		leaves.GetMaterial().SetNormalTexture("assets/models/Island/Coconut/leaf-normal.png");
		m_sceneModels.push_back(leaves);
	}

	// Duo Palms
	{
		Model trunk;
		trunk.SetPosition({ 0.0f, 45.0f, 0.0f });
		trunk.Load("assets/models/Island/Duo-Palms/duo-palms-trunk.obj");
		trunk.SetScale(sceneScale);
		trunk.GetMaterial().SetDiffuseTexture("assets/models/Island/Duo-Palms/trunk-diffuse.jpg");
		m_sceneModels.push_back(trunk);

		Model leaves;
		leaves.SetPosition({ 0.0f, 45.0f, 0.0f });
		leaves.Load("assets/models/Island/Duo-Palms/duo-palms-leaves.obj");
		leaves.SetScale(sceneScale);
		leaves.GetMaterial().SetDiffuseTexture("assets/models/Island/Duo-Palms/leaves-diffuse.png");
		m_sceneModels.push_back(leaves);
	}

	Model pot;
	pot.SetPosition({ 0.0f, 50.0f, 0.0f });
	pot.Load("assets/models/Island/Pot/pot.obj");
	pot.SetScale(sceneScale);
	pot.GetMaterial().SetDiffuseTexture("assets/models/Island/Pot/diffuse.png");
	pot.GetMaterial().SetNormalTexture("assets/models/Island/Pot/normal.png");
	pot.GetMaterial().SetSpecularTexture("assets/models/Island/Pot/specular.png");
	pot.GetMaterial().SetSpecularSettings(5.0f, 10.0f);
	m_sceneModels.push_back(pot);

	Model palm;
	palm.SetPosition({ 0.0f, 50.0f, 0.0f });
	palm.Load("assets/models/Island/Arabic-Palm/arabic-palm.obj");
	palm.SetScale(sceneScale);
	palm.GetMaterial().SetDiffuseTexture("assets/models/Island/Arabic-Palm/diffuse.jpg");
	palm.GetMaterial().SetNormalTexture("assets/models/Island/Arabic-Palm/normal.jpg");
	m_sceneModels.push_back(palm);
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
