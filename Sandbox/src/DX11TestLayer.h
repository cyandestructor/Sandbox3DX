#ifndef DX11TESTLAYER_H
#define DX11TESTLAYER_H

#include <Jass.h>

class DX11TestLayer : public Jass::Layer {

public:
	DX11TestLayer();
	virtual ~DX11TestLayer() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
	virtual void OnUpdate(Jass::Timestep ts) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Jass::Event& e) override;

private:
	Jass::OrthographicCameraController m_cameraController;

	Jass::ShaderLibrary m_shaderLib;

	Jass::Ref<Jass::VertexArray> m_triangle;

	void LoadShaders();
	void CreateTriangle();

};

#endif // !DX11TESTLAYER_H
