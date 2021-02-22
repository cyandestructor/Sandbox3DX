#include "DX11TestLayer.h"

DX11TestLayer::DX11TestLayer()
	:Layer("DX11TestLayer"), m_cameraController(1280.0f / 720.0f)
{
}

void DX11TestLayer::OnAttach()
{
	LoadShaders();
	CreateTriangle();
}

void DX11TestLayer::OnDetach()
{
}

void DX11TestLayer::OnUpdate(Jass::Timestep ts)
{
	m_cameraController.OnUpdate(ts);

	Jass::RenderCommand::SetClearColor({ 0.2f, 0.2f, 0.2f, 0.0f });
	Jass::RenderCommand::Clear();

	Jass::Renderer::BeginScene(m_cameraController.GetCamera());
	
	Jass::Renderer::Submit(m_shaderLib.GetShader("Triangle"), m_triangle);

	Jass::Renderer::EndScene();
}

void DX11TestLayer::OnImGuiRender()
{
}

void DX11TestLayer::OnEvent(Jass::Event& e)
{
	m_cameraController.OnEvent(e);
}

void DX11TestLayer::LoadShaders()
{
	m_shaderLib.Load("assets/shaders/Triangle.glsl");
}

void DX11TestLayer::CreateTriangle()
{
	float positions[]{
		-0.5f, -0.5f, 0.0f,
		0.8f, 0.1f, 0.4f, 1.0f,
		
		0.5f, -0.5f, 0.0f,
		0.1f, 0.2f, 0.8f, 1.0f,
		
		0.0f, 0.5f, 0.0f,
		0.2f, 0.5f, 0.5f, 1.0f
	};

	unsigned int indices[]{
		0, 1, 2
	};

	m_triangle = Jass::VertexArray::Create();

	auto vertexBuffer = Jass::VertexBuffer::Create({ positions,sizeof(positions),Jass::DataUsage::StaticDraw });

	Jass::BufferLayout layout = {
		{Jass::ShaderDataType::Float3, "position" },
		{Jass::ShaderDataType::Float4, "v_color"}
	};
	vertexBuffer->SetLayout(layout);

	m_triangle->AddVertexBuffer(vertexBuffer);

	auto indexBuffer = Jass::IndexBuffer::Create({ indices,3,Jass::DataUsage::StaticDraw });

	m_triangle->SetIndexBuffer(indexBuffer);
}
