#include "ModelLoading.h"

// Include this file to access Dear ImGui (GUI dialogs)
#include <imgui.h>

ModelLoading::ModelLoading()
{
	float width = float(Jass::Application::Get().GetWindow().GetWidth());
	float height = float(Jass::Application::Get().GetWindow().GetHeight());
	// Initialize your camera. This is my configuration but you can play with the settings
	m_camera.SetProjection(60.0f, width, height, 0.01f, 1000.0f);

	m_lightPosition = { 100.0f, 100.0f, 0.0f };
	m_lightColor = { 1.0f, 1.0f, 1.0f, 1.0f };
}

void ModelLoading::OnAttach()
{
	LoadShaders();
	LoadModels();
}

void ModelLoading::OnDetach()
{
	// Nothing to do here yet
}

void ModelLoading::OnImGuiRender()
{
	ImGui::Begin("Settings");
	ImGui::InputFloat3("Light position", Jass::GetPtr(m_lightPosition));
	ImGui::ColorEdit4("Light color", Jass::GetPtr(m_lightColor));
	ImGui::End();
}

void ModelLoading::OnUpdate(Jass::Timestep ts)
{
	UpdateCamera(ts);
	UpdateLight();

	// The Clear Color is the background color. These two lines are needed for rendering
	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_camera);
	
	// In this lines, we bind the shader and then we pass the camera position for the specular light
	m_shaderLib.GetShader("ModelMaterial")->Bind();
	m_shaderLib.GetShader("ModelMaterial")->SetFloat3("u_cameraPosition", m_camera.GetPosition());

	// Render here!

	/*
		To render a model, provide a shader.
		You can use the Shader Library to get the shader using the name you wrote when load it.
		Also provide a light.
	*/
	m_model.Render(m_shaderLib.GetShader("ModelMaterial"), m_light);
	
	Jass::Renderer::EndScene();
}

void ModelLoading::OnEvent(Jass::Event& e)
{
	// Create an Event Dispatcher
	Jass::EventDispatcher dispatcher(e);

	// Dispatch for every Event Type you need
	// In this example, we bind our OnKeyPressedEvent method to receive any message of type KeyPressedEvent
	dispatcher.Dispatch<Jass::KeyPressedEvent>(BIND_EVENT_FN(ModelLoading::OnKeyPressedEvent));
	dispatcher.Dispatch<Jass::WindowResizeEvent>(BIND_EVENT_FN(ModelLoading::OnWindowResizeEvent));
}

void ModelLoading::LoadShaders()
{
	// In this way, you can load a shader. The first argument is an optional name
	m_shaderLib.Load("ModelMaterial", "assets/shaders/DirectX11/NormalsMaterial.hlsl");
	// NormalsMaterial.glsl is a shader that supports Ambient, Diffuse and Specular Light
}

void ModelLoading::LoadModels()
{
	m_model.Load("assets/models/Lantern/lantern.obj");
	/*
		Once you have load the model, you can change its textures through the material.
		Both are optional. If no diffuse texture is set, the model will be white, you can
		set its color with the SetColor method of the Material.
	*/
	m_model.GetMaterial().SetDiffuseTexture("assets/models/Lantern/diffuse.jpg");
	m_model.GetMaterial().SetNormalTexture("assets/models/Lantern/normal.png");
	// You can also apply transformations (move, rotate, scale) to the model at any time
	m_model.SetScale({ 12.0f, 12.0f, 12.0f });

}

void ModelLoading::UpdateCamera(Jass::Timestep ts)
{
	m_cameraRotation += 15.0f * ts;
	m_cameraRotation = m_cameraRotation < 360.0f ? m_cameraRotation : 0.0f;

	float cameraX = 50.0f * cos(Jass::Radians(m_cameraRotation));
	float cameraZ = 50.0f * sin(Jass::Radians(m_cameraRotation));

	// You can transform the camera (move and rotate)
	m_camera.SetPosition({ cameraX, 0.0f, cameraZ });
	m_camera.LookAtTarget({ 0.0f, 0.0f, 0.0f });
}

void ModelLoading::UpdateLight()
{
	m_light.SetPosition(m_lightPosition);
	m_light.SetColor(m_lightColor);
}

bool ModelLoading::OnKeyPressedEvent(Jass::KeyPressedEvent& e)
{
	// Every Event Type has some info related to the message
	switch (e.GetKeyCode())
	{
	case JASS_KEY_ESCAPE:
		/*
			There is only one application at a time (singleton)
			so first use the Get Method, then any method you need
		*/
		Jass::Application::Get().Close();
		break;
	default:
		break;
	}

	// Note that the Key Codes have the syntax JASS_KEY_*

	/*
		The return value indicates if the event has been handled or not.
		If false, the event is not propagated to the next layers.
		This is only relevant if you have more than one layer on the stack,
		if not, only return false.
	*/
	return false;
}

bool ModelLoading::OnWindowResizeEvent(Jass::WindowResizeEvent& e)
{
	float width = (float)e.GetWidth();
	float height = (float)e.GetHeight();

	m_camera.SetProjection(60.0f, width, height, 0.001f, 1000.0f);

	return false;
}
