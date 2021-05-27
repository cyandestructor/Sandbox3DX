#ifndef MODEL_H
#define MODEL_H

#include "Mesh.h"
#include "Material.h"

class Model {

public:
	Model();
	Model(const std::string& filename);
	Model(const Mesh& mesh) : m_mesh(mesh) {}

	void Load(const std::string& filename);

	void SetMesh(const Mesh& mesh) { m_mesh = mesh; }

	void SetPosition(const Jass::JVec3& position);
	const Jass::JVec3& GetPosition() const { return m_position; }

	void Rotate(const Jass::JVec3& angles);
	void Rotate(const Jass::JVec3& axis, float angle);
	Jass::JMat4 GetRotation() const { return m_rotation; }

	void SetScale(const Jass::JVec3& scale);
	const Jass::JVec3& GetScale() const { return m_scale; }

	void SetTransformation(const Jass::JMat4& transformation) { m_transformation = transformation; }
	const Jass::JMat4& GetTransformation() const { return m_transformation; }

	Material& GetMaterial() { return m_material; }

	void SetCullBackFace(bool enable) { m_cullBackface = enable; }
	bool IsCullBackFace() const { return m_cullBackface; }

	void Render(Jass::Ref<Jass::Shader>& shader, const Light& light, const Jass::JVec4& clipPlane = Jass::JVec4(0.0f)) const;

private:
	Mesh m_mesh;
	Material m_material;

	bool m_cullBackface = true;

	Jass::JVec3 m_position = Jass::JVec3(0.0f);
	Jass::JMat4 m_rotation = Jass::JMat4(1.0f);
	Jass::JVec3 m_scale = Jass::JVec3(1.0f);

	Jass::JMat4 m_transformation = Jass::JMat4(1.0f);

	void CalculateTransformation();

};

#endif // !MODEL_H
