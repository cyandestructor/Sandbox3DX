#ifndef ISLAND_SCENE_H
#define ISLAND_SCENE_H

#include <Jass.h>

#include "../Sandbox3D/Terrain/Terrain.h"
#include "../Sandbox3D/Model/Model.h"
#include "../Sandbox3D/Player/PlayerController.h"

class Island : public Jass::Layer {

public:

	Island();

	virtual ~Island() = default;

	virtual void OnAttach() override;

	virtual void OnDetach() override;

	virtual void OnImGuiRender() override;

	virtual void OnUpdate(Jass::Timestep ts) override;

	virtual void OnEvent(Jass::Event& e) override;

private:
	PlayerController m_playerController;

	bool m_isCursorDisabled;
	bool m_isFlyMode;

	Terrain m_terrain;

	std::vector<Model> m_sceneModels;

	Light m_light;

	Jass::ShaderLibrary m_shaderLib;

	void RenderScene(Jass::Timestep ts);

	void LoadShaders();
	void LoadTerrainTextures();
	void LoadModels();

	void FixCameraToTerrain();

	bool OnKeyPressedEvent(Jass::KeyPressedEvent& e);

};

#endif // !ISLAND_SCENE_H
