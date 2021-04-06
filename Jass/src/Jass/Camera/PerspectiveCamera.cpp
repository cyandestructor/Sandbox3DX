#include "jasspch.h"
#include "PerspectiveCamera.h"

namespace Jass {

	PerspectiveCamera::PerspectiveCamera(const PerspectiveCameraSettings& settings)
	{
		m_settings = settings;

		m_projection = PerspectiveFOV(
			Radians(m_settings.FOV),
			m_settings.ViewportWidth,
			m_settings.ViewportHeight,
			m_settings.Near,
			m_settings.Far
		);

		CalculateMatrices();
	}

	void PerspectiveCamera::SetPosition(const JVec3& position)
	{
		m_settings.Position = position;
		CalculateMatrices();
	}

	void PerspectiveCamera::SetRotation(const JVec3& rotation)
	{
		m_settings.Rotation = rotation;
		CalculateMatrices();
	}

	void PerspectiveCamera::LookAtTarget(const JVec3& target)
	{
		m_view = LookAt(m_settings.Position, target, m_upVector);

		m_view[0].z *= -1.0f;
		m_view[1].z *= -1.0f;
		m_view[2].z *= -1.0f;
		m_view[3].z *= -1.0f;

		m_viewProjection = m_projection * m_view;
	}

	void PerspectiveCamera::SetProjection(float fov, float width, float height, float pNear, float pFar)
	{
		m_settings.FOV = fov;
		m_settings.ViewportWidth = width;
		m_settings.ViewportHeight = height;
		m_settings.Near = pNear;
		m_settings.Far = pFar;

		m_projection = PerspectiveFOV(
			Radians(m_settings.FOV),
			m_settings.ViewportWidth,
			m_settings.ViewportHeight,
			m_settings.Near,
			m_settings.Far
		);

		m_viewProjection = m_projection * m_view;
	}

	void PerspectiveCamera::CalculateMatrices()
	{
		m_view = Translate(JMat4(1.0f), -m_settings.Position);
		m_view = Rotate(m_view, Radians(m_settings.Rotation.x), JVec3(-1.0f, 0.0f, 0.0f));
		m_view = Rotate(m_view, Radians(m_settings.Rotation.y), JVec3(0.0f, -1.0f, 0.0f));
		m_view = Rotate(m_view, Radians(m_settings.Rotation.z), JVec3(0.0f, 0.0f, -1.0f));

		m_view[0].z *= -1.0f;
		m_view[1].z *= -1.0f;
		m_view[2].z *= -1.0f;
		m_view[3].z *= -1.0f;

		m_viewProjection = m_projection * m_view;
	}

}