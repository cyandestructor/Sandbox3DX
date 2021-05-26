#include "Material.h"

Material::Material()
{
	m_whiteTexture = Jass::Texture2D::Create(1, 1);
	uint32_t texData = 0xffffffff; // RGBA (1.0, 1.0, 1.0, 1.0)
	m_whiteTexture->SetData(&texData, sizeof(uint32_t));

	m_neutralNormalTexture = Jass::Texture2D::Create(1, 1);
	uint32_t normalTexData = 0x8080ffff; // RGBA (0.5, 0.5, 1.0, 1.0)
	m_neutralNormalTexture->SetData(&normalTexData, sizeof(uint32_t));
}

void Material::SetDiffuseTexture(const std::string& textureFilename)
{
	m_diffuseTexture = Jass::Texture2D::Create(textureFilename);
}

void Material::SetSpecularTexture(const std::string& textureFilename)
{
	m_specularTexture = Jass::Texture2D::Create(textureFilename);
}

void Material::SetNormalTexture(const std::string& textureFilename)
{
	m_normalTexture = Jass::Texture2D::Create(textureFilename);
}

void Material::Prepare(Jass::Ref<Jass::Shader>& shader, const Light& light) const
{
	shader->Bind();

	shader->SetFloat3("u_lightPosition", light.GetPosition());
	shader->SetFloat4("u_lightColor", light.GetColor());

	shader->SetFloat("u_ambientReduction", m_ambientReduction);
	shader->SetFloat("u_diffuseReduction", m_diffuseReduction);
	shader->SetFloat("u_reflectivity", m_reflectivity);
	shader->SetFloat("u_shineDamper", m_shineDamper);

	if (m_diffuseTexture)
		m_diffuseTexture->Bind(0);
	else
		m_whiteTexture->Bind(0);

	//shader->SetInt("u_diffuseTex", 0);

	shader->SetFloat4("u_color", m_color);

	if (m_normalTexture)
		m_normalTexture->Bind(1);
	else
		m_neutralNormalTexture->Bind(1);

	//shader->SetInt("u_normalTex", 1);

	if (m_specularTexture)
		m_specularTexture->Bind(2);
	else
		m_whiteTexture->Bind(2);

	//shader->SetInt("u_specularTex", 2);
}
