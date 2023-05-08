#include "stdafx.h"
#include "Prefabs/Character.h"

Character::Character(const CharacterDesc& characterDesc) :
	m_CharacterDesc{ characterDesc },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime)
{}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
}

void Character::Update(const SceneContext& sceneContext)
{
		constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero
		const float elapsedSec{ sceneContext.pGameTime->GetElapsed() };
		
		//***************
		//HANDLE INPUT

		XMFLOAT2 move{ 0.f,0.f };
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveForward))
		{
			move.y = 1.f;
		}
		else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveBackward))
		{
			move.y = -1.f;
		}

		//Optional: if move.y is near zero (abs(move.y) < epsilon), you could use the ThumbStickPosition.y for movement
		if (abs(move.y) < epsilon)
		{
			move.y = sceneContext.pInput->GetThumbstickPosition(true, m_CharacterDesc.gamepadIndex).y;
		}

		//move.x should contain a 1 (Right) or -1 (Left) based on the active input (check corresponding actionId in m_CharacterDesc)
		if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveRight))
		{
			move.x = 1.f;
		}
		else if(sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
		{
			move.x = -1.f;
		}

		//Optional: if move.x is near zero (abs(move.x) < epsilon), you could use the Left ThumbStickPosition.x for movement
		if (abs(move.x) < epsilon)
		{
			move.x = sceneContext.pInput->GetThumbstickPosition(true, m_CharacterDesc.gamepadIndex).x;
		}

		//********
		//MOVEMENT
		const XMVECTOR worldForward{ 0.f,0.f,1.f };
		const XMVECTOR worldRight{ 1.f,0.f,0.f };

		//## Horizontal Velocity (Forward/Backward/Right/Left)
		//Calculate the current move acceleration for this frame (m_MoveAcceleration * ElapsedTime)
		const float acceleration{ m_MoveAcceleration * sceneContext.pGameTime->GetElapsed() };

		//If the character is moving (= input is pressed)
		if (abs(move.x) > epsilon || abs(move.y) > epsilon)
		{
			//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
			const XMVECTOR direction{ worldForward * move.y + worldRight * move.x };
			XMStoreFloat3(&m_CurrentDirection, direction);

			//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed += acceleration;

			//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
			m_MoveSpeed = std::min(m_MoveSpeed, m_CharacterDesc.maxMoveSpeed);

			//--- Lerp to move direction ---
			const XMVECTOR normalizedDirection{ XMVector3Normalize(direction) };
			const XMVECTOR playerRotation{ -XMLoadFloat3(&GetTransform()->GetForward()) };

			const float angleDistanceInRadians{ XMVectorGetX(XMVector3AngleBetweenNormals(normalizedDirection, playerRotation)) };
			const float angleDistance{ XMConvertToDegrees(angleDistanceInRadians) };

			constexpr float rotationEpsilon{ 4.f };
			if (angleDistance > rotationEpsilon)
			{
				const XMVECTOR perpendicular{ -XMLoadFloat3(&GetTransform()->GetRight()) };

				//Check sign
				const float dot{ XMVectorGetX(XMVector3Dot(perpendicular, normalizedDirection)) };
				const int sign{ dot > 0.f ? 1 : -1 };

				//Rotate
				m_TotalYaw += sign * m_CharacterDesc.rotationSpeed * angleDistance * elapsedSec;
				
				//Prevent overflow
				if (abs(m_TotalYaw) > 360.f)
				{
					m_TotalYaw -= 360.f * m_TotalYaw / abs(m_TotalYaw);
				}

				GetTransform()->Rotate(0.f, m_TotalYaw, 0.f);
			}
		}
		else
		{
			//Decrease the current MoveSpeed with the current Acceleration (m_MoveSpeed)
			m_MoveSpeed -= acceleration;

			//Make sure the current MoveSpeed doesn't get smaller than zero
			m_MoveSpeed = std::max(m_MoveSpeed, 0.0f);
		}


		//Now we can calculate the Horizontal Velocity which should be stored in m_TotalVelocity.xz
		//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
		//Set the x/z component of m_TotalVelocity (horizontal_velocity x/z)
		//It's important that you don't overwrite the y component of m_TotalVelocity (contains the vertical velocity)
		m_TotalVelocity.x = m_CurrentDirection.x * m_MoveSpeed;
		m_TotalVelocity.z = m_CurrentDirection.z * m_MoveSpeed;

		//If the Controller Component is NOT grounded (= freefall)
		if (!(m_pControllerComponent->GetCollisionFlags() & PxControllerCollisionFlag::eCOLLISION_DOWN))
		{
			//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
			m_TotalVelocity.y -= m_FallAcceleration * sceneContext.pGameTime->GetElapsed();

			//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
			m_TotalVelocity.y = std::max(m_TotalVelocity.y, -m_CharacterDesc.maxFallSpeed);
		}
		else
		{
			//m_TotalVelocity.y is zero
			m_TotalVelocity.y = 0.f;
		}

		//************
		//DISPLACEMENT

		m_pControllerComponent->Move(XMFLOAT3{ m_TotalVelocity.x * elapsedSec ,m_TotalVelocity.y * elapsedSec ,m_TotalVelocity.z * elapsedSec });
}

void Character::DrawImGui()
{
}