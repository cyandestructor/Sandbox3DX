#ifndef MATERIAL_H
#define MATERIAL_H

#include "Jass.h"

#include "Light.h"

class Material {

public:
	Material();
	
	void SetColor(const Jass::JVec4& color) { m_color = color; }
	
	void SetDiffuseTexture(const std::string& textureFilename);
	void SetSpecularTexture(const std::string& textureFilename);
	void SetNormalTexture(const std::string& textureFilename);

	void SetAmbientReduction(float reductionFactor) { m_ambientReduction = reductionFactor; }
	void SetDiffuseReduction(float reductionFactor) { m_diffuseReduction = reductionFactor; }

	void SetSpecularSettings(float reflectivity, float shineDamper)
	{
		m_reflectivity = reflectivity;
		m_shineDamper = shineDamper;
	}

	void Prepare(Jass::Ref<Jass::Shader>& shader, const Light& light) const;

private:
	float m_ambientReduction = 1.0f;
	
	Jass::Ref<Jass::Texture2D> m_whiteTexture;
	Jass::Ref<Jass::Texture2D> m_neutralNormalTexture;
	Jass::JVec4 m_color = Jass::JVec4(1.0f);

	Jass::Ref<Jass::Texture2D> m_diffuseTexture;
	float m_diffuseReduction = 1.0f;
	
	Jass::Ref<Jass::Texture2D> m_specularTexture;
	float m_reflectivity = 0.0f;
	float m_shineDamper = 0.0f;

	Jass::Ref<Jass::Texture2D> m_normalTexture;

};

#endif // !MATERIAL_H
