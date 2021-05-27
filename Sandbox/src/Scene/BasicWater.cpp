#include "BasicWater.h"

BasicWater::BasicWater(const Jass::JVec2& scale, unsigned int widthDiv, unsigned int depthDiv)
{
	Plane plane(scale.x, scale.y, widthDiv, depthDiv);
	m_mesh = plane.Generate();
}

void BasicWater::SetTextures(const std::string& diffuse, const std::string& dudv, const std::string& normal)
{
	m_diffuseTexture = Jass::Texture2D::Create(diffuse);
	m_dudvTexture = Jass::Texture2D::Create(dudv);
	m_normalTexture = Jass::Texture2D::Create(normal);
}

void BasicWater::Render(const Jass::Ref<Jass::Shader>& shader, const Light& light, const Jass::Camera& camera) const
{
	shader->Bind();

	// Send information to the shader
	shader->SetFloat("u_tilingFactor", m_tilingFactor);
	shader->SetFloat4("u_color", m_color);

	shader->SetFloat("u_reflectivity", m_reflectivity);
	shader->SetFloat("u_shineDamper", m_shineDamper);

	shader->SetFloat("u_ambientReduction", m_ambientReduction);

	shader->SetFloat3("u_lightPosition", light.GetPosition());
	shader->SetFloat4("u_lightColor", light.GetColor());

	shader->SetFloat("u_distortionFactor", m_distortionFactor);
	shader->SetFloat("u_motionFactor", m_motionFactor);

	shader->SetFloat3("u_cameraPosition", camera.GetPosition());

	if (m_diffuseTexture) {
		m_diffuseTexture->Bind(0);
	}

	if (m_dudvTexture) {
		m_dudvTexture->Bind(1);
	}

	if (m_normalTexture) {
		m_normalTexture->Bind(2);
	}

	// Render
	Jass::JMat4 transformation = Jass::Translate(Jass::JMat4(1.0f), m_position);
	Jass::Renderer::Submit(shader, m_mesh.GetVertexArray(), m_mesh.GetRenderMode(), transformation);
}
