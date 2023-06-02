#include "stdafx.h"
#include "Bomberman/Character.h"
#include "Bomberman/Bomb.h"
#include "Bomberman/Grid.h"
#include "Scenes/BombermanScene.h"
#include "PlayerGameIcon.h"

Character::Character(const CharacterDesc& characterDesc, Grid* pGrid) :
	m_CharacterDesc{ characterDesc }, m_pGrid{ pGrid },
	m_MoveAcceleration(characterDesc.maxMoveSpeed / characterDesc.moveAccelerationTime),
	m_FallAcceleration(characterDesc.maxFallSpeed / characterDesc.fallAccelerationTime),
	m_Score{ 0 }, m_TotalYaw{ characterDesc.startYaw }, m_CanPlaceBomb{ false }
{
	SetTag(L"Player");
}

void Character::Initialize(const SceneContext& /*sceneContext*/)
{
	//Controller
	m_pControllerComponent = AddComponent(new ControllerComponent(m_CharacterDesc.controller));
}

void Character::Update(const SceneContext& sceneContext)
{
	if (m_IsActive)
	{
		//Input
		HandleInput(sceneContext);

		//Animations
		UpdateAnimations(sceneContext);

		//Update power up's
		if (m_PowerUpTimer > 0.f)
		{
			m_PowerUpTimer -= sceneContext.pGameTime->GetElapsed();

			if (m_PowerUpTimer <= 0.f)
			{
				m_PowerUpTimer = 0.f;
				AddPowerUp(PickUpType::None);
			}
		}

	}
	else if (m_VibrationCounter > 0.f)
	{
		m_VibrationCounter -= sceneContext.pGameTime->GetElapsed();

		if (m_VibrationCounter <= 0.f)
		{
			sceneContext.pInput->SetVibration(0.f, 0.f, m_CharacterDesc.gamepadIndex);
		}
	}
}

void Character::HandleInput(const SceneContext& sceneContext)
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
		bool detonateTriggered{ sceneContext.pInput->IsActionTriggered(m_CharacterDesc.actionId_Detonate) };

		if (detonateTriggered)
		{
			if (m_pLastBomb)
			{
				m_pLastBomb->Explode();
			}
		}

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
	case CharacterAction::placingBomb:
		if (m_CanPlaceBomb)
		{
			PlaceBomb();
			m_CanPlaceBomb = false;
		}
		break;
	default:
		break;
	}
}

void Character::UpdateAnimations(const SceneContext&)
{
	//constexpr float epsilon{ 0.01f }; //Constant that can be used to compare if a float is near zero
	//constexpr float placeBombAnimDuration{ 0.3f }; //Constant that can be used to compare if a float is near zero

	UINT clipId{ 0 };

	switch (m_CurrentAction)
	{
		case CharacterAction::running:
		{
			m_pAnimator->Play();
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
			m_pAnimator->Play();
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
			constexpr float animationSpeed{ 3.f };
			m_pAnimator->SetAnimationSpeed(animationSpeed);

			clipId = m_CharacterDesc.clipId_PlaceBomb;

			if (m_CharacterDesc.currentClipId != clipId)
			{
				//Set dropping bomb anim
				m_pAnimator->SetAnimation(clipId);
				m_pAnimator->Play();
				//Set current id
				m_CharacterDesc.currentClipId = clipId;

				m_pAnimator->SetDoOnce(true);

				m_CanPlaceBomb = true;
			}

			//Stop placing
			if (!m_pAnimator->IsPlaying())
			{			
				m_CurrentAction = CharacterAction::standing;

				m_pAnimator->SetPlayReversed(false);
				m_pAnimator->SetDoOnce(false);
			}
		}
		break;
	default:
		break;
	}
}

void Character::PlaceBomb()
{
	const auto transform{ GetTransform() };
	XMFLOAT3 worldPos{ transform->GetWorldPosition() };

	Node* pNode{ m_pGrid->GetNode(XMFLOAT2{worldPos.x, worldPos.z}) };

	bool flames{ m_CurrentPowerUp == PickUpType::Flames };
	bool fireUp{ m_CurrentPowerUp == PickUpType::FireUp };

	if(flames || fireUp)
	{
		AddPowerUp(PickUpType::None);
	}

	m_pLastBomb = new Bomb(pNode->GetCol(), pNode->GetRow(), this, m_pGrid, flames || fireUp, fireUp);
	BombermanScene::AddGameObject(m_pLastBomb);
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

void Character::AddScore()
{
	++m_Score;
	m_ScoreChanged = true;
}

bool Character::Kill()
{
	if (m_CurrentPowerUp != PickUpType::Shield)
	{
		m_IsActive = false;
		GetTransform()->Translate(0.f, 100.f, -200.f);

		SceneManager::Get()->GetActiveSceneContext().pInput->SetVibration(1.f, 1.f, m_CharacterDesc.gamepadIndex);

		m_VibrationCounter = 1.f;

		return true;
	}

	m_PowerUpTimer = 2.f;

	return false;
}

void Character::SetIsActive(bool isActive)
{
	m_IsActive = isActive;
}

void Character::SetIcon(PlayerGameIcon* pIcon)
{
	m_pIcon = pIcon;
}

void Character::UpdateIcon()
{
	if (m_ScoreChanged)
	{
		m_pIcon->AddStar();
		m_ScoreChanged = false;
	}
}

void Character::AddPowerUp(PickUpType type)
{
	m_CurrentPowerUp = type;

	std::string text{ "" };

	switch (m_CurrentPowerUp)
	{
	case PickUpType::Flames:
		text = "Flames";
		break;
	case PickUpType::Shield:
		text = "Shield";
		break;
	case PickUpType::FireUp:
		text = "Fire Up";
		break;
	default:
		text = "";
		break;
	}

	m_pIcon->SetPowerUpText(text);
}
