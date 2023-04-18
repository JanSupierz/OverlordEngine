#include "stdafx.h"
#include "W3_PongScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

#include "Prefabs/TorusPrefab.h"

W3_PongScene::W3_PongScene() :
	GameScene(L"W3_PongScene") {}

enum InputIds : int
{
	Left_LeftPlayer, Right_LeftPlayer, Left_RightPlayer, Right_RightPlayer, BallStart
};

void W3_PongScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;

	m_pFixedCamera = new FixedCamera();
	m_pFixedCamera->GetTransform()->Translate(0, 50, 0);
	m_pFixedCamera->GetTransform()->Rotate(90, 0, 0);
	AddChild(m_pFixedCamera);

	SetActiveCamera(m_pFixedCamera->GetComponent<CameraComponent>());

	auto& physX = PxGetPhysics();
	auto pPaddleMaterial = physX.createMaterial(10.f, 10.f, 1.f);
	auto pBouncyMaterial = physX.createMaterial(0.f, 0.f, 1.f);

	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);
	
	XMFLOAT3 groundDimensions{ 1000.f,0.f,1000.f };
	auto pGround = new CubePrefab(groundDimensions, XMFLOAT4(Colors::Black));
	AddChild(pGround);

	//Paddles
	XMFLOAT3 paddleDimensions{ 1.f,2.f,5.f };

	//Paddle Left
	m_pPaddleLeft = new CubePrefab(paddleDimensions, XMFLOAT4(Colors::White));
	AddChild(m_pPaddleLeft);

	auto pRigidBody = m_pPaddleLeft->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxBoxGeometry{ paddleDimensions.x / 2.f,paddleDimensions.y / 2.f,paddleDimensions.z / 2.f }, *pPaddleMaterial);
	pRigidBody->SetKinematic(true);

	//Paddle Right
	m_pPaddleRight = new CubePrefab(paddleDimensions, XMFLOAT4(Colors::White));
	AddChild(m_pPaddleRight);

	pRigidBody = m_pPaddleRight->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxBoxGeometry{ paddleDimensions.x / 2.f,paddleDimensions.y / 2.f,paddleDimensions.z / 2.f }, *pPaddleMaterial);
	pRigidBody->SetKinematic(true);

	//Sphere
	m_pBall = new SpherePrefab(1, 10, XMFLOAT4(Colors::Red));
	AddChild(m_pBall);

	pRigidBody = m_pBall->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransY | RigidBodyConstraint::AllRot, false);

	Reset();

	const float paddleOffsetFromCenter{ m_HalfLength - 2.5f };
	m_pPaddleLeft->GetTransform()->Translate(-paddleOffsetFromCenter, 1.f, 0);
	m_pPaddleRight->GetTransform()->Translate(paddleOffsetFromCenter, 1.f, 0);

	//Borders
	constexpr float borderHeight{ 4.f }, borderWidth{ 2.f };
	XMFLOAT3 sideDimensions{ m_HalfLength * 2.f,borderHeight,borderWidth };

	auto pSide = new CubePrefab(sideDimensions, XMFLOAT4(Colors::Black));
	AddChild(pSide);

	pRigidBody = pSide->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxBoxGeometry{ sideDimensions.x / 2.f,sideDimensions.y / 2.f,sideDimensions.z / 2.f }, *pBouncyMaterial);
	pSide->GetTransform()->Translate(0.f, borderHeight / 2.f, m_HalfWidth);

	pSide = new CubePrefab(sideDimensions, XMFLOAT4(Colors::Black));
	AddChild(pSide);

	pRigidBody = pSide->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxBoxGeometry{ sideDimensions.x / 2.f,sideDimensions.y / 2.f,sideDimensions.z / 2.f }, *pBouncyMaterial);
	pSide->GetTransform()->Translate(0.f, borderHeight / 2.f, -m_HalfWidth);

	//Left/Right
	auto triggerCallBack = [=](GameObject*, GameObject*, PxTriggerAction action)
	{
		if (action == PxTriggerAction::ENTER)
		{
			Reset();
		}
	};

	sideDimensions = { borderWidth,borderHeight, m_HalfWidth * 2.f };

	//Right
	pSide = new CubePrefab(sideDimensions, XMFLOAT4(Colors::Black));
	AddChild(pSide);

	pRigidBody = pSide->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxBoxGeometry{ sideDimensions.x / 2.f,sideDimensions.y / 2.f,sideDimensions.z / 2.f }, *pBouncyMaterial, true);
	pSide->GetTransform()->Translate(m_HalfLength + (sideDimensions.x / 2.f), borderHeight / 2.f, 0.f);

	pSide->SetOnTriggerCallBack(triggerCallBack);

	//Left
	pSide = new CubePrefab(sideDimensions, XMFLOAT4(Colors::Black));
	AddChild(pSide);

	pRigidBody = pSide->AddComponent(new RigidBodyComponent(true));
	pRigidBody->AddCollider(PxBoxGeometry{ sideDimensions.x / 2.f,sideDimensions.y / 2.f,sideDimensions.z / 2.f }, *pBouncyMaterial, true);
	pSide->GetTransform()->Translate(-(m_HalfLength + (sideDimensions.x / 2.f)), borderHeight / 2.f, 0.f);

	pSide->SetOnTriggerCallBack(triggerCallBack);

	//Input actions
	const auto pInput{ m_SceneContext.pInput };
	pInput->AddInputAction(InputAction{ InputIds::Left_LeftPlayer,InputState::down, 'S',-1,XINPUT_GAMEPAD_DPAD_RIGHT });
	pInput->AddInputAction(InputAction{ InputIds::Right_LeftPlayer,InputState::down,'W',-1,XINPUT_GAMEPAD_DPAD_LEFT });
	pInput->AddInputAction(InputAction{ InputIds::Left_RightPlayer,InputState::down, VK_DOWN,-1});
	pInput->AddInputAction(InputAction{ InputIds::Right_RightPlayer,InputState::down,VK_UP,-1});

	pInput->AddInputAction(InputAction{ InputIds::BallStart,InputState::pressed,VK_SPACE,-1 });
}

void W3_PongScene::Update()
{
	const auto pInput{ m_SceneContext.pInput };
	const float paddleSpeed{ 40.f * m_SceneContext.pGameTime->GetElapsed() };
	const float maxPaddleDistance{ m_HalfWidth - 5.f };

	if (m_IsPaused)
	{
		if (pInput->IsActionTriggered(BallStart))
		{
			m_pBall->GetComponent<RigidBodyComponent>()->AddForce({ 50.f,0.f,4.f }, PxForceMode::eIMPULSE);
			m_IsPaused = false;
		}
	}

	if (pInput->IsActionTriggered(Right_LeftPlayer))
	{
		auto pRigidBody = m_pPaddleLeft->GetComponent<RigidBodyComponent>();
		
		XMFLOAT3 translation{ 0.f, 0.f, paddleSpeed };
		XMFLOAT3 position{ pRigidBody->GetPosition() };
		
		XMStoreFloat3(&translation, XMVectorAdd(XMLoadFloat3(&translation), XMLoadFloat3(&position)));
		
		if (abs(translation.z) <= maxPaddleDistance)
		{
			pRigidBody->Translate(translation);
		}
	}

	if (pInput->IsActionTriggered(Left_LeftPlayer))
	{
		auto pRigidBody = m_pPaddleLeft->GetComponent<RigidBodyComponent>();
		
		XMFLOAT3 translation{ 0.f, 0.f, -paddleSpeed };
		XMFLOAT3 position{ pRigidBody->GetPosition() };

		XMStoreFloat3(&translation, XMVectorAdd(XMLoadFloat3(&translation), XMLoadFloat3(&position)));
		
		if (abs(translation.z) <= maxPaddleDistance)
		{
			pRigidBody->Translate(translation);
		}
	}

	if (pInput->IsActionTriggered(Right_RightPlayer))
	{
		auto pRigidBody = m_pPaddleRight->GetComponent<RigidBodyComponent>();

		XMFLOAT3 translation{ 0.f, 0.f, paddleSpeed };
		XMFLOAT3 position{ pRigidBody->GetPosition() };

		XMStoreFloat3(&translation, XMVectorAdd(XMLoadFloat3(&translation), XMLoadFloat3(&position)));

		if (abs(translation.z) <= maxPaddleDistance)
		{
			pRigidBody->Translate(translation);
		}
	}

	if (pInput->IsActionTriggered(Left_RightPlayer))
	{
		auto pRigidBody = m_pPaddleRight->GetComponent<RigidBodyComponent>();

		XMFLOAT3 translation{ 0.f, 0.f, -paddleSpeed };
		XMFLOAT3 position{ pRigidBody->GetPosition() };

		XMStoreFloat3(&translation, XMVectorAdd(XMLoadFloat3(&translation), XMLoadFloat3(&position)));

		if (abs(translation.z) <= maxPaddleDistance)
		{
			pRigidBody->Translate(translation);
		}
	}
}

void W3_PongScene::Draw()
{
	//Optional
}

void W3_PongScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}

void W3_PongScene::Reset()
{
	m_pBall->GetTransform()->Translate(0.f, 2.1f, 0.f);
	m_IsPaused = true;
}

