#include "stdafx.h"
#include "Bomberman/Character.h"
#include "Bomberman/Bomb.h"
#include "Bomberman/Grid.h"
#include "BombermanScene.h"

Character::Character(const CharacterDesc& characterDesc, Grid* pGrid) :
	m_CharacterDesc{ characterDesc }, m_pGrid{ pGrid }, 
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
	switch (m_CurrentAction)
	{
		case CharacterAction::running:
		case CharacterAction::standing:
		{
			bool placeBombTriggered{ sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_PlaceBomb) };

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
			else if (sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_MoveLeft))
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
			const float acceleration{ m_MoveAcceleration * elapsedSec };

			//If the character is moving (= input is pressed)
			if (abs(move.x) > epsilon || abs(move.y) > epsilon)
			{
				m_CurrentAction = CharacterAction::running;

				//Calculate & Store the current direction (m_CurrentDirection) >> based on the forward/right vectors and the pressed input
				const XMVECTOR direction{ worldForward * move.y + worldRight * move.x };
				XMStoreFloat3(&m_CurrentDirection, direction);

				//Increase the current MoveSpeed with the current Acceleration (m_MoveSpeed)
				m_MoveSpeed += acceleration;

				//Make sure the current MoveSpeed stays below the maximum MoveSpeed (CharacterDesc::maxMoveSpeed)
				m_MoveSpeed = std::min(m_MoveSpeed, m_CharacterDesc.maxMoveSpeed);

				//--- Lerp to move direction ---
				const XMVECTOR normalizedDirection{ XMVector3Normalize(direction) };
				const XMVECTOR playerRotation{ XMLoadFloat3(&GetTransform()->GetForward()) };

				const float angleDistanceInRadians{ XMVectorGetX(XMVector3AngleBetweenNormals(normalizedDirection, playerRotation)) };
				const float angleDistance{ XMConvertToDegrees(angleDistanceInRadians) };

				constexpr float rotationEpsilon{ 4.f };
				if (angleDistance > rotationEpsilon)
				{
					const XMVECTOR perpendicular{ XMLoadFloat3(&GetTransform()->GetRight()) };

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

				if (m_MoveSpeed <= epsilon)
				{
					m_CurrentAction = CharacterAction::standing;
				}

				//Make sure the current MoveSpeed doesn't get smaller than zero
				m_MoveSpeed = std::max(m_MoveSpeed, 0.0f);
			}

			//Calculate the horizontal velocity (m_CurrentDirection * MoveSpeed)
			m_TotalVelocity.x = m_CurrentDirection.x * m_MoveSpeed;
			m_TotalVelocity.z = m_CurrentDirection.z * m_MoveSpeed;

			//If the Controller Component is NOT grounded (= freefall)
			if (!(m_pControllerComponent->GetCollisionFlags() & PxControllerCollisionFlag::eCOLLISION_DOWN))
			{
				//Decrease the y component of m_TotalVelocity with a fraction (ElapsedTime) of the Fall Acceleration (m_FallAcceleration)
				m_TotalVelocity.y -= m_FallAcceleration * elapsedSec;

				//Make sure that the minimum speed stays above -CharacterDesc::maxFallSpeed (negative!)
				m_TotalVelocity.y = std::max(m_TotalVelocity.y, -m_CharacterDesc.maxFallSpeed);
			}
			else
			{
				//m_TotalVelocity.y is zero
				m_TotalVelocity.y = 0.f;

				if (placeBombTriggered)
				{
					m_CurrentAction = CharacterAction::placingBomb;
				}
			}

			//************
			//DISPLACEMENT

			m_pControllerComponent->Move(XMFLOAT3{ m_TotalVelocity.x * elapsedSec ,m_TotalVelocity.y * elapsedSec ,m_TotalVelocity.z * elapsedSec });
		}
		break;
		default:
			break;
	}

	//Animations
	UpdateAnimations(sceneContext);
}

void Character::UpdateAnimations(const SceneContext& sceneContext)
{
	constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero
	constexpr float placeBombAnimDuration{ 0.9f }; //Constant that can be used to compare if a float is near zero

	UINT clipId{ 0 };

	switch (m_CurrentAction)
	{
		case CharacterAction::running:
		{
			clipId = m_CharacterDesc.clipId_Walking;

			if (m_CharacterDesc.currentClipId != clipId)
			{
				m_pAnimator->SetAnimation(clipId);
				m_CharacterDesc.currentClipId = clipId;
			}

			m_pAnimator->SetAnimationSpeed(m_MoveSpeed / m_CharacterDesc.maxMoveSpeed);
		}
		break;

		case CharacterAction::standing:
		{
			clipId = m_CharacterDesc.clipId_Idle;

			if (m_CharacterDesc.currentClipId != clipId)
			{
				m_pAnimator->SetAnimation(clipId);
				m_CharacterDesc.currentClipId = clipId;
			}

			m_pAnimator->SetAnimationSpeed(1.f);
		}
		break;

		case CharacterAction::placingBomb:
		{
			m_AnimationTimeLeft -= sceneContext.pGameTime->GetElapsed();

			clipId = m_CharacterDesc.clipId_PlaceBomb;

			if (m_CharacterDesc.currentClipId != clipId)
			{
				m_pAnimator->SetAnimation(clipId);
				m_pAnimator->Reset(false);
				m_CharacterDesc.currentClipId = clipId;
				m_AnimationTimeLeft = placeBombAnimDuration;
			}

			if (m_AnimationTimeLeft < -epsilon)
			{
				m_AnimationTimeLeft = placeBombAnimDuration;
				m_pAnimator->SetPlayReversed(true);

				m_CurrentAction = CharacterAction::stopPlacing;
			}
		}
		break;

		case CharacterAction::stopPlacing:
		{
			m_AnimationTimeLeft -= sceneContext.pGameTime->GetElapsed();

			if (m_AnimationTimeLeft < epsilon)
			{
				m_pAnimator->SetPlayReversed(false);
				m_CurrentAction = CharacterAction::standing;

				PlaceBomb();
			}
		}
		break;

	default:
		break;
	}

	if (!m_pAnimator->IsPlaying())
	{
		m_pAnimator->Play();
	}
}

void Character::PlaceBomb() const
{
	const auto transform{ GetTransform() };
	XMFLOAT3 worldPos{ transform->GetWorldPosition() };
	XMVECTOR forwardOffset{ XMLoadFloat3(&transform->GetForward()) * m_pGrid->GetCellSize() * 0.5f };
	XMVECTOR currentPosition{ XMLoadFloat3(&transform->GetWorldPosition()) };

	XMFLOAT3 bombPosition{};
	XMStoreFloat3(&bombPosition, currentPosition + forwardOffset);

	Node* pNode{ m_pGrid->GetNode(XMFLOAT2{bombPosition.x, bombPosition.z}) };

	//If node in front of you is blocked
	if (!pNode || (pNode && pNode->IsBlocked()))
	{
		//Spawn the bomb in your cell
		pNode = m_pGrid->GetNode(XMFLOAT2{ worldPos.x, worldPos.z });
	}

	BombermanScene::AddGameObject(new Bomb(pNode->GetCol(), pNode->GetRow(), this, m_pGrid));
}

void Character::DrawImGui()
{
}

void Character::SetAnimator(ModelAnimator* pAnimator)
{
	m_pAnimator = pAnimator;
}

int Character::GetIndex() const
{
	return static_cast<int>(m_CharacterDesc.gamepadIndex);
}
