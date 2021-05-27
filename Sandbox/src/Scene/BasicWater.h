#ifndef BASIC_WATER_H
#define BASIC_WATER_H

#include "Jass.h"
#include "../Sandbox3D/Model/Light.h"
#include "../Sandbox3D/Primitives/Plane.h"

class BasicWater {
public:
	BasicWater(const Jass::JVec2& scale, unsigned int widthDiv = 10, unsigned int depthDiv = 10);

	void SetAmbientReduction(float reductionFactor) { m_ambientReduction = reductionFactor; }

	void SetDistortionFactor(float distortionFactor) { m_distortionFactor = distortionFactor; }
	void SetMotionFactor(float motionFactor) { m_motionFactor = motionFactor; }

	void SetPosition(const Jass::JVec3& position) { m_position = position; }
	const Jass::JVec3& GetPosition() const { return m_position; }

	void SetSpecularProperties(float reflectivity, float shineDamper) {
		m_reflectivity = reflectivity;
		m_shineDamper = shineDamper;
	}

	void SetColor(const Jass::JVec4& color) { m_color = color; }
	void SetTextures(const std::string& diffuse, const std::string& dudv, const std::string& normal);

	void SetTilingFactor(float tilingFactor) { m_tilingFactor = tilingFactor; }

	void Render(const Jass::Ref<Jass::Shader>& shader, const Light& light, const Jass::Camera& camera) const;

private:
	Mesh m_mesh;

	float m_distortionFactor = 1.0f;
	float m_motionFactor = 0.0f;

	float m_ambientReduction = 1.0f;

	float m_reflectivity = 1.0f;
	float m_shineDamper = 1.0f;

	float m_tilingFactor = 1.0f;

	Jass::Ref<Jass::Texture2D> m_diffuseTexture;
	Jass::Ref<Jass::Texture2D> m_dudvTexture;
	Jass::Ref<Jass::Texture2D> m_normalTexture;

	Jass::JVec3 m_position = Jass::JVec3(0.0f);

	Jass::JVec4 m_color = { 0.0f, 0.0f, 1.0f , 1.0f };
};

#endif // !BASIC_WATER_H
