#include "SpecularMaps.h"

SpecularMaps::SpecularMaps()
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	
	m_playerController.GetCamera().SetProjection(60.0f, width, height, 0.01f, 1000.0f);
	m_playerController.GetCamera().SetPosition({ 0.0f, 40.0f, 0.0f });

	m_light.SetPosition({ 400.0f, 500.0f, 0.0f });

	m_modelRotation = 0.0f;

	m_isCursorDisabled = true;
}

void SpecularMaps::OnAttach()
{
	if (m_isCursorDisabled)
		Jass::Input::SetCursorMode(Jass::CursorMode::Disabled);

	LoadShaders();
	LoadModels();
}

void SpecularMaps::OnDetach()
{
	// Nothing to do here, yet
}

void SpecularMaps::OnImGuiRender()
{
}

void SpecularMaps::OnUpdate(Jass::Timestep ts)
{
	UpdateModels(ts);

	m_playerController.OnUpdate(ts);
	
	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_playerController.GetCamera());

	m_shaderLib.GetShader("ModelMaterial")->Bind();
	m_shaderLib.GetShader("ModelMaterial")->SetFloat3("u_cameraPosition", m_playerController.GetCamera().GetPosition());

	m_model.Render(m_shaderLib.GetShader("ModelMaterial"), m_light);

	Jass::Renderer::EndScene();
}

void SpecularMaps::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(SpecularMaps::OnKeyPressedEvent));
	
	m_playerController.OnEvent(e);
}

void SpecularMaps::LoadShaders()
{
	m_shaderLib.Load("ModelMaterial", "assets/shaders/NormalsMaterial.glsl");
}

void SpecularMaps::LoadModels()
{
	m_model.Load("assets/models/Radio/radio.obj");
	m_model.GetMaterial().SetDiffuseTexture("assets/models/Radio/diffuse.png");
	m_model.GetMaterial().SetNormalTexture("assets/models/Radio/normal.png");
	m_model.GetMaterial().SetSpecularTexture("assets/models/Radio/specular.png");
	m_model.GetMaterial().SetSpecularSettings(10.0f, 9.0f);

	m_model.SetPosition({ 0.0f, 40.0f, -50.0f });
}

void SpecularMaps::UpdateModels(Jass::Timestep ts)
{
	const float rotationSpeed = 1.0f;
	m_modelRotation += rotationSpeed * ts;
	m_modelRotation = m_modelRotation < 360.0f ? m_modelRotation : 0.0f;

	m_model.Rotate({ 0.0f, m_modelRotation, 0.0f });
}

bool SpecularMaps::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case JASS_KEY_ESCAPE:
		Jass::Application::Get().Close();
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