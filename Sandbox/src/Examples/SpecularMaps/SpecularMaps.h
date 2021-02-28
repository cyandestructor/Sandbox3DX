#ifndef SPECULAR_MAPS_EXAMPLE
#define SPECULAR_MAPS_EXAMPLE

#include <Jass.h>
#include "../../Sandbox3D/Model/Model.h"
#include "../../Sandbox3D/Player/PlayerController.h"

class SpecularMaps : public Jass::Layer {

public:

	SpecularMaps();

	virtual ~SpecularMaps() = default;

	virtual void OnAttach() override;

	virtual void OnDetach() override;

	virtual void OnImGuiRender() override;

	virtual void OnUpdate(Jass::Timestep ts) override;

	virtual void OnEvent(Jass::Event& e) override;

private:
	PlayerController m_playerController;

	bool m_isCursorDisabled;

	Model m_model;

	float m_modelRotation;

	Light m_light;

	Jass::ShaderLibrary m_shaderLib;

	void LoadShaders();
	void LoadModels();

	void UpdateModels(Jass::Timestep ts);

	bool OnKeyPressedEvent(Jass::KeyPressedEvent& e);

};

#endif // !SPECULAR_MAPS_EXAMPLE
