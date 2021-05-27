#ifndef ISLAND_SCENE_H
#define ISLAND_SCENE_H

#include <Jass.h>

#include "BasicWater.h"
#include "../Sandbox3D/Skybox/Skybox.h"
#include "../Sandbox3D/Terrain/Terrain.h"
#include "../Sandbox3D/Model/Model.h"
#include "../Sandbox3D/Player/PlayerController.h"
#include "../Sandbox3D/Model/Billboard.h"

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

	Jass::JVec3 m_lightDirection = Jass::JVec3(0.0f);
	float m_lightAngle = 0.0f;
	float m_totalColor = 0.9f;
	float m_ambientReduction = 0.0f;
	float m_diffuseReduction = 0.0f;

	Skybox m_skybox;
	float m_skyRotation = 0.0f;
	float m_blendNight = 0.0f;
	float m_blendMorning = 0.0f;

	BasicWater m_water;
	float m_waterMotion = 0.0f;
	float m_waterMotionSpeed = 0.03f;

	Terrain m_terrain;

	std::vector<Model> m_sceneModels;
	std::vector<Billboard> m_sceneBillboards;

	Billboard m_seagle;
	Jass::JVec3 m_seaglePosition = Jass::JVec3(0.0f);
	float m_seagleAngle = 0.0f;

	Light m_light;

	Jass::ShaderLibrary m_shaderLib;

	void RenderScene(Jass::Timestep ts);

	void UpdateDayCycle(Jass::Timestep ts);
	void UpdateWater(Jass::Timestep ts);
	void UpdateSeagle(Jass::Timestep ts);

	void LoadShaders();
	void LoadTerrainTextures();
	void LoadSkyboxTextures();
	void LoadBillboards();
	void LoadModels();

	void FixCameraToTerrain();

	bool OnKeyPressedEvent(Jass::KeyPressedEvent& e);

};

#endif // !ISLAND_SCENE_H
