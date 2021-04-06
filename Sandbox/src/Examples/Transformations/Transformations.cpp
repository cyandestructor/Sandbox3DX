#include "Transformations.h"

#include <imgui.h>

Transformations::Transformations()
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	m_camera.SetProjection(60.0f, width, height, 0.01f, 1000.0f);
	m_camera.SetPosition({ 0.0f, 0.0f, 50.0f });

	m_cameraRotation = 0.0f;

	m_light.SetPosition({ 100.0f, 100.0f, 0.0f });
	
	m_modelPosition = Jass::JVec3(0.0f);
	m_modelScale = { 12.0f, 12.0f, 12.0f };
	m_modelRotation = Jass::JVec3(0.0f);

	m_modelColor = Jass::JVec4(1.0f);
	m_modelReflectivity = 0.0f;
	m_modelShineDamper = 0.0f;
}

void Transformations::OnAttach()
{
	LoadShaders();
	LoadModels();
}

void Transformations::OnDetach()
{
	// Nothing to do here, yet
}

void Transformations::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::Text("Hold R to rotate the camera");
	ImGui::InputFloat3("Position", Jass::GetPtr(m_modelPosition));
	ImGui::InputFloat3("Scale", Jass::GetPtr(m_modelScale));
	ImGui::InputFloat3("Rotation", Jass::GetPtr(m_modelRotation));
	ImGui::ColorEdit4("Color", Jass::GetPtr(m_modelColor));
	ImGui::InputFloat("Reflectivity", &m_modelReflectivity, 0.01f, 0.05f);
	ImGui::InputFloat("Shine damper", &m_modelShineDamper, 0.1f, 0.3f);
	ImGui::End();

	// Clamp the values of the reflectivity and shine damper (only positive values)
	m_modelReflectivity = std::max(m_modelReflectivity, 0.0f);
	m_modelShineDamper = std::max(m_modelShineDamper, 0.0f);
}

void Transformations::OnUpdate(Jass::Timestep ts)
{
	UpdateModel();

	/*
		The Input class is very useful to query if a key is pressed.
		It has also methods for mouse input and gamepad input.
	*/
	if (Jass::Input::IsKeyPressed(JASS_KEY_R))
		UpdateCamera(ts);

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_camera);

	// In this lines, we bind the shader and then we pass the camera position for the specular light
	m_shaderLib.GetShader("ModelMaterial")->Bind();
	m_shaderLib.GetShader("ModelMaterial")->SetFloat3("u_cameraPosition", m_camera.GetPosition());

	m_model.Render(m_shaderLib.GetShader("ModelMaterial"), m_light);

	Jass::Renderer::EndScene();
}

void Transformations::OnEvent(Jass::Event& e)
{
	Jass::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(Transformations::OnKeyPressedEvent));
	dispatcher.Dispatch<Jass::WindowResizeEvent>(BIND_EVENT_FN(Transformations::OnWindowResizeEvent));
}

void Transformations::LoadShaders()
{
	m_shaderLib.Load("ModelMaterial", "assets/shaders/DirectX11/NormalsMaterial.hlsl");
}

void Transformations::LoadModels()
{
	m_model.Load("assets/models/Lantern/lantern.obj");
	m_model.GetMaterial().SetDiffuseTexture("assets/models/Lantern/diffuse.jpg");
	m_model.GetMaterial().SetNormalTexture("assets/models/Lantern/normal.png");
}

void Transformations::UpdateModel()
{
	// You can apply transformations to a model. Change its position, scale or rotate it
	
	m_model.SetPosition(m_modelPosition);
	m_model.SetScale(m_modelScale);

	/*
		Angle supplied in degrees here.
		There are two overloads of the Rotate method. Use this one preferably,
		as the other one is a little bit bug but is there for compatibility with old code.
	*/
	m_model.Rotate(m_modelRotation);
	
	// You can also change some material properties such as the color or the specular properties
	m_model.GetMaterial().SetColor(m_modelColor);
	m_model.GetMaterial().SetSpecularSettings(m_modelReflectivity, m_modelShineDamper);
}

void Transformations::UpdateCamera(Jass::Timestep ts)
{
	m_cameraRotation += 15.0f * ts;
	m_cameraRotation = m_cameraRotation < 360.0f ? m_cameraRotation : 0.0f;

	float cameraX = 50.0f * sin(Jass::Radians(m_cameraRotation));
	float cameraZ = 50.0f * cos(Jass::Radians(m_cameraRotation));

	// You can transform the camera (move and rotate)
	m_camera.SetPosition({ cameraX, 0.0f, cameraZ });
	m_camera.LookAtTarget({ 0.0f, 0.0f, 0.0f });
}

bool Transformations::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
{
	switch (e.GetKeyCode())
	{
	case JASS_KEY_ESCAPE:
		Jass::Application::Get().Close();
		break;
	default:
		break;
	}

	return false;
}

bool Transformations::OnWindowResizeEvent(Jass::WindowResizeEvent& e)
{
	float width = (float)e.GetWidth();
	float height = (float)e.GetHeight();

	m_camera.SetProjection(60.0f, width, height, 0.001f, 1000.0f);

	return false;
}
