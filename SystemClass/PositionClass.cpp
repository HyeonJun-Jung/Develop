#include "PositionClass.h"

PositionClass::PositionClass()
{
}

PositionClass::PositionClass(const PositionClass&)
{
}

PositionClass::~PositionClass()
{
}

void PositionClass::SetFrameTime(float FrameTime)
{
	m_frameTime = FrameTime;
}

void PositionClass::GetRotation(float& RotationY)
{
	RotationY = m_rotationY;
}

void PositionClass::TurnLeft(bool KeyDown)
{
	// If the key is pressed increase the speed at which the camera turns left.  If not slow down the turn speed.
	if (KeyDown)
	{
		m_leftTurnSpeed += m_frameTime * 0.001f;

		if (m_leftTurnSpeed > (m_frameTime * 0.15f))
		{
			m_leftTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_leftTurnSpeed -= m_frameTime * 0.005f;

		if (m_leftTurnSpeed < 0.0f)
		{
			m_leftTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY -= m_leftTurnSpeed;
	if (m_rotationY < 0.0f)
	{
		m_rotationY += 360.0f;
	}

}

void PositionClass::TurnRight(bool KeyDown)
{
	// If the key is pressed increase the speed at which the camera turns right.  If not slow down the turn speed.
	if (KeyDown)
	{
		m_rightTurnSpeed += m_frameTime * 0.001f;

		if (m_rightTurnSpeed > (m_frameTime * 0.15f))
		{
			m_rightTurnSpeed = m_frameTime * 0.15f;
		}
	}
	else
	{
		m_rightTurnSpeed -= m_frameTime * 0.005f;

		if (m_rightTurnSpeed < 0.0f)
		{
			m_rightTurnSpeed = 0.0f;
		}
	}

	// Update the rotation using the turning speed.
	m_rotationY += m_rightTurnSpeed;
	if (m_rotationY > 360.0f)
	{
		m_rotationY -= 360.0f;
	}

	return;
}
