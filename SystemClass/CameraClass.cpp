#include "CameraClass.h"

CameraClass* CameraClass::m_Instance = nullptr;

CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// 위쪽을 가리키는 벡터를 설정합니다.
	m_up.x = 0.0f;
	m_up.y = 1.0f;
	m_up.z = 0.0f;

	// 기본적으로 카메라가 찾고있는 위치를 설정합니다.
	m_lookAt.x = 0.0f;
	m_lookAt.y = 0.0f;
	m_lookAt.z = 1.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}



XMFLOAT3 CameraClass::GetCameraForwardDirection()
{
	/*
	XMFLOAT3 CameraPosition(m_position.x, m_position.y, m_position.z);
	XMVECTOR CameraPositionVector = XMLoadFloat3(&CameraPosition);
	XMVECTOR LookAtVector = XMLoadFloat3(&m_lookAt);
	XMVECTOR DirectionVector = XMVector3Normalize(LookAtVector - CameraPositionVector);
	XMStoreFloat3(&m_CameraFoward, DirectionVector);
	*/

	XMFLOAT3 CameraPosition(m_position.x, m_position.y, m_position.z);
	XMVECTOR CameraPositionVector = XMLoadFloat3(&CameraPosition);
	XMVECTOR DirectionVector = XMVector3Normalize(m_LookAtVector - CameraPositionVector);
	XMStoreFloat3(&m_CameraFoward, DirectionVector);
	return m_CameraFoward;
}

XMFLOAT3 CameraClass::GetCameraRightDirection()
{
	/*
	XMFLOAT3 ForwardVector = GetCameraForwardDirection();
	XMVECTOR upVector(XMLoadFloat3(&m_up));
	XMVECTOR rightVector = XMVector3Normalize(XMVector3Cross(upVector, XMLoadFloat3(&ForwardVector)));
	XMStoreFloat3(&m_CameraRight, rightVector);
	*/

	XMFLOAT3 ForwardVector = GetCameraForwardDirection();
	XMVECTOR rightVector = XMVector3Normalize(XMVector3Cross(m_UpVector, XMLoadFloat3(&ForwardVector)));
	XMStoreFloat3(&m_CameraRight, rightVector);

	return m_CameraRight;
}


XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}

XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}

XMFLOAT3 CameraClass::GetLookAt()
{
	return m_lookAt;
}

void CameraClass::Render()
{	

	XMVECTOR positionVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;
	

	// XMVECTOR 구조체에 up 벡터를 로드합니다.
	m_UpVector = XMLoadFloat3(&m_up);


	// XMVECTOR 구조체에 lookAt 벡터를 로드합니다.
	m_LookAtVector = XMLoadFloat3(&m_lookAt);


	// 3D 월드에서 카메라의 위치를 설정합니다.
	positionVector = XMLoadFloat3(&m_position);	

	// yaw (Y축), pitch (X축) 및 roll (Z축)의 회전값을 라디안 단위로 설정합니다.
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	// yaw, pitch, roll 값을 통해 회전 행렬을 만듭니다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAtVector 및 upVector를 rotationMatrix를 통해 회전 행렬로 변형해 뷰가 원점에서 올바르게 회전되도록 합니다.
	m_LookAtVector = XMVector3TransformCoord(m_LookAtVector, rotationMatrix);
	m_UpVector = XMVector3TransformCoord(m_UpVector, rotationMatrix);

	// 회전 된 카메라의 위치를 뷰어 위치로 변환합니다.
	m_LookAtVector = XMVectorAdd(positionVector, m_LookAtVector);

	// 마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬을 만듭니다.
	m_viewMatrix = XMMatrixLookAtLH(positionVector, m_LookAtVector, m_UpVector);
	

}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}

void CameraClass::ReflectionRender(float height)
{
	XMVECTOR positionVector;
	XMFLOAT3 up, position, lookAt;
	XMMATRIX rotationMatrix;
	float radians;


	// 위쪽을 가리키는 벡터를 만듭니다.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// 월드에 카메라 위치를 설정합니다.
	// 평면 반사를 위해 카메라의 Y값을 역전시킵니다.
	position.x = m_position.x;
	position.y = -m_position.y + (height * 2.0f);
	position.z = m_position.z;

	// XMVECTOR 구조체에 up 벡터를 로드합니다.
	m_UpVector = XMLoadFloat3(&m_up);

	// XMVECTOR 구조체에 position 벡터를 로드합니다.
	positionVector = XMLoadFloat3(&position);

	// XMVECTOR 구조체에 lookAt 벡터를 로드합니다.
	m_LookAtVector = XMLoadFloat3(&m_lookAt);

	// yaw (Y축), pitch (X축) 및 roll (Z축)의 회전값을 라디안 단위로 설정합니다.
	// 3.14 / 180 =  0.0174532925f
	float pitch = m_rotation.x * 0.0174532925f;
	float yaw = m_rotation.y * 0.0174532925f;
	float roll = m_rotation.z * 0.0174532925f;

	// yaw, pitch, roll 값을 통해 회전 행렬을 만듭니다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// lookAtVector 및 upVector를 rotationMatrix를 통해 회전 행렬로 변형해 뷰가 원점에서 올바르게 회전되도록 합니다.
	m_LookAtVector = XMVector3TransformCoord(m_LookAtVector, rotationMatrix);
	m_UpVector = XMVector3TransformCoord(m_UpVector, rotationMatrix);

	// 회전 된 카메라의 위치를 뷰어 위치로 변환합니다.
	m_LookAtVector = XMVectorAdd(positionVector, m_LookAtVector);

	// 위의 세 벡터를 이용하여 뷰 행렬을 생성합니다.
	m_reflectionViewMatrix = XMMatrixLookAtLH(positionVector, m_LookAtVector, m_UpVector);

}

void CameraClass::GetReflectionViewMatrix(XMMATRIX& ReflectionViewMatrix)
{
	ReflectionViewMatrix = m_reflectionViewMatrix;
}


void CameraClass::CameraMove(Camera_Move_Dir MoveDir)
{
	XMFLOAT3 ForwardDirection = GetCameraForwardDirection();
	XMFLOAT3 RightDirection = GetCameraRightDirection();

	float MoveVal = 0.3f + 2.7f;

	switch (MoveDir)
	{
		case MD_FRONT:
		{
			
			m_position.x += MoveVal * ForwardDirection.x;
			m_position.y += MoveVal * ForwardDirection.y;
			m_position.z += MoveVal * ForwardDirection.z;
			break;
		}

		case MD_BACK:
		{
			m_position.x -= MoveVal * ForwardDirection.x;
			m_position.y -= MoveVal * ForwardDirection.y;
			m_position.z -= MoveVal * ForwardDirection.z;
			break;
		}
		case MD_LEFT:
		{
			m_position.x -= MoveVal * RightDirection.x;
			m_position.y -= MoveVal * RightDirection.y;
			m_position.z -= MoveVal * RightDirection.z;
			break;
		}
		case MD_RIGHT:
		{
			m_position.x += MoveVal * RightDirection.x;
			m_position.y += MoveVal * RightDirection.y;
			m_position.z += MoveVal * RightDirection.z;
			break;
		}
	}
}

void CameraClass::CameraRotation(Camera_Rotation_Dir RotationDir)
{
	XMFLOAT3 RightDirection = GetCameraRightDirection();
	float Rotationval = 1.5f + 3.0f;
	
	switch (RotationDir)
	{
		case RT_UP:
		{
			// 회전을 업데이트 합니다.
			m_rotation.x -= Rotationval;

			// 회전은 최대 90 도로 유지 합니다.
			if (m_rotation.x > 90.0f)
			{
				m_rotation.x = 90.0f;
			}
			break;
		}
		case RT_DOWN:
		{
			// 회전을 업데이트 합니다.
			m_rotation.x += Rotationval;

			// 회전은 최대 90 도로 유지 합니다.
			if (m_rotation.x < -90.0f)
			{
				m_rotation.x = -90.0f;
			}
			break;
		}
		case RT_LEFT:
		{
			// 회전을 업데이트 합니다.
			m_rotation.y -= Rotationval;			

			// 회전값의 범위를 0 ~360 에서 유지합니다.
			if (m_rotation.y < 0.0f)
			{
				m_rotation.y += 360.0f;
			}
			break;
		}
		case RT_RIGHT:
		{
			m_rotation.y += Rotationval;

			// 회전은 최대 90 도로 유지 합니다.
			if (m_rotation.y > 360.f)
			{
				m_rotation.y -= 360.0f;
			}
			break;
		}	
	}
	
}