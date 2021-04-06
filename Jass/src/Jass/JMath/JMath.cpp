#include "jasspch.h"

#include "JMath.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Jass {

	JMat4 Scale(const JMat4& matrix, const JVec3& vector)
	{
		return glm::scale(matrix, vector);
	}

	JMat4 Translate(const JMat4& matrix, const JVec3& vector)
	{
		return glm::translate(matrix, vector);
	}

	JMat4 Rotate(const JMat4& matrix, float angle, const JVec3& vector)
	{
		return glm::rotate(matrix, angle, vector);
	}

	JMat4 PerspectiveFOV(float fov, float width, float height, float zNear, float zFar)
	{
		return glm::perspectiveFovLH_ZO(fov, width, height, zNear, zFar);
	}

	JMat4 LookAt(const JVec3& position, const JVec3& target, const JVec3& up)
	{
		return glm::lookAtRH(position, target, up);
	}

	JMat4 Orthographic(float left, float right, float bottom, float top, float zNear = -1.0f, float zFar = 1.0f)
	{
		return glm::orthoLH_ZO(left, right, bottom, top, zNear, zFar);
	}

	float Radians(float degrees)
	{
		return glm::radians(degrees);
	}

	float* GetPtr(JMat3& matrix)
	{
		return glm::value_ptr(matrix);
	}

	float* GetPtr(JMat4& matrix)
	{
		return glm::value_ptr(matrix);
	}

	float* GetPtr(JVec4& vector)
	{
		return glm::value_ptr(vector);
	}

	float* GetPtr(JVec3& vector)
	{
		return glm::value_ptr(vector);
	}

	const float* GetPtr(const JMat3& matrix)
	{
		return glm::value_ptr(matrix);
	}

	const float* GetPtr(const JMat4& matrix)
	{
		return glm::value_ptr(matrix);
	}

	const float* GetPtr(const JVec4& vector)
	{
		return glm::value_ptr(vector);
	}

	const float* GetPtr(const JVec3& vector)
	{
		return glm::value_ptr(vector);
	}

	JVec3 Cross(const JVec3& v1, const JVec3& v2)
	{
		return glm::cross(v1, v2);
	}

	JVec3 Normalize(const JVec3& vector)
	{
		return glm::normalize(vector);
	}

	JMat3 Transpose(const JMat3& matrix)
	{
		return glm::transpose(matrix);
	}

	JMat4 Transpose(const JMat4& matrix)
	{
		return glm::transpose(matrix);
	}

	JMat3 Inverse(const JMat3& matrix)
	{
		return glm::inverse(matrix);
	}

	JMat4 Inverse(const JMat4& matrix)
	{
		return glm::inverse(matrix);
	}

}