#include "stdafx.h"
#include "BombermanScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

#include "Prefabs/TorusPrefab.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/ColorMaterial_Shadow_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Materials/Shadow/ColorMaterial_Shadow.h"

#include "Prefabs/Character.h"

BombermanScene::BombermanScene() :
	GameScene(L"BombermanScene") 
{
	m_pCharacters.resize(4);
}

BombermanScene::~BombermanScene()
{
}

void BombermanScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;

	m_SceneContext.pLights->SetDirectionalLight({ -60.f,100.f, 10.f }, { 0.4f, -0.7f, 0.f });

	{
		const auto pBlackMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
		pBlackMaterial->SetColor(DirectX::Colors::Black);
	
		const auto pWhiteMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
		pWhiteMaterial->SetColor(DirectX::Colors::White);
	
		const auto pPinkMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
		pPinkMaterial->SetColor(DirectX::Colors::HotPink);
	
		const auto pBlueMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
		pBlueMaterial->SetColor(DirectX::Colors::Blue);
	
		const auto pGoldMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
		pGoldMaterial->SetColor(DirectX::Colors::Gold);
	
		const auto pSkinMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
		pSkinMaterial->SetColor(DirectX::Colors::Bisque);
	
		auto& physX = PxGetPhysics();
		auto pDefaultMaterial = physX.createMaterial(0.5f, 0.5f, 0.1f);
	
		InitArena();
		PlayerDesc playerDesc{ 0,pDefaultMaterial };
		playerDesc.pShirtMaterial = pBlueMaterial;
		playerDesc.pMainMaterial = pWhiteMaterial;
		playerDesc.pBlackMaterial = pBlackMaterial;
		playerDesc.pGoldMaterial = pGoldMaterial;
		playerDesc.pGlovesMaterial = pPinkMaterial;
		playerDesc.pSkinMaterial = pSkinMaterial;
		playerDesc.spriteOffsetY = 100.f;
		InitPlayer(playerDesc);

		playerDesc.index = 1;
		playerDesc.pMainMaterial = pBlackMaterial;
		playerDesc.spriteName = L"Icon_Black";
		playerDesc.spriteOffsetY += 200.f;
		InitPlayer(playerDesc);
	}
	
	m_pFixedCamera = new FixedCamera();
	m_pFixedCamera->GetTransform()->Translate(0, m_MaxCameraHeight, -80);
	m_pFixedCamera->GetTransform()->Rotate(70, 0, 0);
	AddChild(m_pFixedCamera);
	
	SetActiveCamera(m_pFixedCamera->GetComponent<CameraComponent>());
	
	const auto pBombMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>() };
	pBombMaterial->SetDiffuseTexture(L"Textures/Bomberman/Bomb.png");

	auto pObject = AddChild(new GameObject);
	auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/Bomb.ovm"));
	pModel->SetMaterial(pBombMaterial);

	pObject->GetTransform()->Translate(10.f, 10.f, 0.f);
	pObject->GetTransform()->Scale(10.f);

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,6.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 2.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 2.f;
	settings.minScale = 3.5f;
	settings.maxScale = 5.5f;
	settings.minEmitterRadius = .2f;
	settings.maxEmitterRadius = .5f;
	settings.color = { 1.f,1.f,1.f, .6f };

	const auto pVFX = pObject->AddChild(new GameObject);
	pVFX->GetTransform()->Translate(0.2f, 1.1f, 0.1f);
	pVFX->AddComponent(new ParticleEmitterComponent(L"Textures/Smoke.png", settings, 200));
}


void BombermanScene::Update()
{
	
}

void BombermanScene::Draw()
{
	//Optional
	//ShadowMapRenderer::Get()->Debug_DrawDepthSRV({ m_SceneContext.windowWidth - 10.f, 10.f }, { 0.5f, 0.5f }, { 1.f,0.f });
}

void BombermanScene::OnGUI()
{
	//ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	//ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	//ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
	//
	//if (ImGui::Button(pAnimator->IsPlaying() ? "PAUSE" : "PLAY"))
	//{
	//	if (pAnimator->IsPlaying())pAnimator->Pause();
	//	else pAnimator->Play();
	//}
	//
	//if (ImGui::Button("RESET"))
	//{
	//	pAnimator->Reset();
	//}
	//
	//ImGui::Dummy({ 0,5 });
	//
	//bool reversed = pAnimator->IsReversed();
	//if (ImGui::Checkbox("Play Reversed", &reversed))
	//{
	//	pAnimator->SetPlayReversed(reversed);
	//}
	//
	//if (ImGui::ListBox("Animation Clip", &m_AnimationClipId, m_ClipNames, static_cast<int>(m_ClipCount)))
	//{
	//	pAnimator->SetAnimation(m_AnimationClipId);
	//}
	//
	//if (ImGui::SliderFloat("Animation Speed", &m_AnimationSpeed, 0.f, 4.f))
	//{
	//	pAnimator->SetAnimationSpeed(m_AnimationSpeed);
	//}
}

void BombermanScene::InitArena()
{
	auto& physX = PxGetPhysics();
	auto pBouncyMaterial = physX.createMaterial(0.f, 0.f, 1.f);
	auto pStaticMaterial = physX.createMaterial(1.f, 1.f, 0.f);

	auto pColorMaterialNoShadow = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pColorMaterialNoShadow->SetDiffuseTexture(L"Textures/Bomberman/GroundWood.jpg");

	auto pObject = AddChild(new GameObject);
	auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	pModel->SetMaterial(pColorMaterialNoShadow);
	pModel->GetTransform()->Scale(40.f);

	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	auto pColorMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pColorMaterial->SetDiffuseTexture(L"Textures/Bomberman/SideCube_Diffuse.png");
	
	//Cubes
	RigidBodyComponent* pRigid{ nullptr };
	
	constexpr int nrCubesPerSide{ 15 };
	constexpr float left{ -(nrCubesPerSide - 1) / 2.f };
	constexpr float bottom{ -(nrCubesPerSide - 1) / 2.f };
	
	//Horizontal lines
	constexpr float cubeDimensions{ 10.f };
	constexpr float halfCube{ cubeDimensions * 0.5f };
	
	float offset{ bottom };

	for (int i{}; i < 2; ++i)
	{
		for (int index{}; index < nrCubesPerSide; ++index)
		{
			pObject = AddChild(new GameObject);
			const auto transform{ pObject->GetTransform() };
			transform->Translate((index + left) * cubeDimensions, cubeDimensions, offset * cubeDimensions);
			transform->Scale(cubeDimensions);

			pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/SideCubes.ovm"));
			pModel->SetMaterial(pColorMaterial);
	
			pRigid = pObject->AddComponent(new RigidBodyComponent(false));
	
			const auto geo{ PxBoxGeometry{ halfCube,halfCube,halfCube } };
			pRigid->AddCollider(geo, *pStaticMaterial, false, PxTransform{ 0.f,halfCube,0.f });
			pRigid->SetConstraint(RigidBodyConstraint::All, false);
		}
	
		//Top
		offset = -bottom;
	}
	
	//Vertical lines
	offset = left;
	
	for (int i{}; i < 2; ++i)
	{
		for (int index{ 1 }; index < nrCubesPerSide - 1; ++index)
		{
			pObject = AddChild(new GameObject);
			const auto transform{ pObject->GetTransform() };
			transform->Translate(offset * cubeDimensions, cubeDimensions, (bottom + index) * cubeDimensions);
			transform->Scale(cubeDimensions);

			pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/SideCubes.ovm"));
			pModel->SetMaterial(pColorMaterial);

			pRigid = pObject->AddComponent(new RigidBodyComponent(false));
	
			const auto geo{ PxBoxGeometry{ halfCube, halfCube, halfCube} };
			pRigid->AddCollider(geo, *pStaticMaterial, false, PxTransform{ 0.f,halfCube,0.f });
			pRigid->SetConstraint(RigidBodyConstraint::All, false);
		}
	
		//Right
		offset = -left;
	}

	//Ground
	const auto pLightColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	pLightColorMaterial->SetColor(DirectX::Colors::LightGreen);
	
	const auto pDarkColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	pDarkColorMaterial->SetColor(DirectX::Colors::DarkGreen);
	
	bool isDark{ true };
	
	for (int row{}; row < nrCubesPerSide; ++row)
	{
		for (int column{}; column < nrCubesPerSide; ++column)
		{
			pObject = AddChild(new GameObject);
			const auto transform{ pObject->GetTransform() };
			transform->Translate((left + column) * cubeDimensions, halfCube, (bottom + row) * cubeDimensions);
			transform->Scale(cubeDimensions);
	
			pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/GroundCubes.ovm"));
	
			if (isDark)
			{
				pModel->SetMaterial(pDarkColorMaterial);
			}
			else
			{
				pModel->SetMaterial(pLightColorMaterial);
			}
	
			isDark = !isDark;
		}
	}

	pObject = AddChild(new GameObject);
	pRigid = pObject->AddComponent(new RigidBodyComponent(false));

	const auto geoGround{ PxBoxGeometry{ halfCube * nrCubesPerSide, cubeDimensions, halfCube * nrCubesPerSide} };
	pRigid->AddCollider(geoGround, *pStaticMaterial, false, PxTransform{ 0.f,0.f,0.f });
	pRigid->SetConstraint(RigidBodyConstraint::All, false);

	//Static obstacles
	const auto pGreyColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	pGreyColorMaterial->SetColor(DirectX::Colors::DimGray);

	constexpr float scale{ 0.7f };
	for (int row{ 1 }; row < nrCubesPerSide - 1; ++row)
	{
		for (int column{ 1 }; column < nrCubesPerSide - 1; ++column)
		{
			if (column % 2 == 0 and row % 2 == 0)
			{
				pObject = AddChild(new GameObject);
				pObject->GetTransform()->Translate((left + column) * cubeDimensions, cubeDimensions + (halfCube * scale), (bottom + row) * cubeDimensions);
				pObject->GetTransform()->Scale(scale * cubeDimensions);

				pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/CenterCubes.ovm"));

				pModel->SetMaterial(pGreyColorMaterial);

				pRigid = pObject->AddComponent(new RigidBodyComponent(false));

				constexpr float geoScale{ scale * halfCube };
				const auto geo{ PxBoxGeometry{ geoScale, geoScale, geoScale} };
				pRigid->AddCollider(geo, *pStaticMaterial, false, PxTransform{ 0.f,0.f,0.f });
				pRigid->SetConstraint(RigidBodyConstraint::All, false);
			}
		}
	}
}

void BombermanScene::InitPlayer(const PlayerDesc& playerDesc)
{
	constexpr float animationMeshSize{ 100.f };
	constexpr float animationMeshScale{ 0.1f };

	constexpr float playerHeight{ animationMeshSize * animationMeshScale };
	constexpr float animationMeshOffset{ -0.85f * playerHeight };

	const auto index{ playerDesc.index };
	const auto gamepadIndex{ static_cast<GamepadIndex>(index) };

	//Character
	{
		CharacterDesc characterDesc{ playerDesc.pPxMaterial };
		characterDesc.actionId_MoveForward = ToInputId(index, CharacterMoveForward);
		characterDesc.actionId_MoveBackward = ToInputId(index, CharacterMoveBackward);
		characterDesc.actionId_MoveLeft = ToInputId(index, CharacterMoveLeft);
		characterDesc.actionId_MoveRight = ToInputId(index, CharacterMoveRight);
		characterDesc.actionId_PlaceBomb = ToInputId(index, CharacterPlaceBomb);
		characterDesc.gamepadIndex = gamepadIndex;
		characterDesc.controller.height = playerHeight;
		characterDesc.controller.radius = playerHeight / 3.f;
		characterDesc.controller.stepOffset = 0.1f;
		characterDesc.maxMoveSpeed = 3.f * playerHeight;
		characterDesc.rotationSpeed = 10.f;

		characterDesc.clipId_Death = playerDesc.clipId_Death;
		characterDesc.clipId_PlaceBomb = playerDesc.clipId_PlaceBomb;
		characterDesc.clipId_Floating = playerDesc.clipId_Floating;
		characterDesc.clipId_Idle = playerDesc.clipId_Idle;
		characterDesc.clipId_Walking = playerDesc.clipId_Walking;

		m_pCharacters[index] = AddChild(new Character(characterDesc));
		m_pCharacters[index]->GetTransform()->Translate(0, 2 * playerHeight, 0.f);
	}

	//Animation
	{
		const auto pModelObject{ m_pCharacters[index]->AddChild(new GameObject()) };

		const auto pModelAnimated = pModelObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/Player.ovm"));
		pModelAnimated->SetMaterial(playerDesc.pMainMaterial);
		m_pCharacters[index]->SetAnimator(pModelAnimated->GetAnimator());

		const auto transform{ pModelAnimated->GetTransform() };
		transform->Scale(animationMeshScale);
		transform->Translate(0.f, animationMeshOffset, 0.f);

		pModelAnimated->SetMaterial(playerDesc.pBlackMaterial, 0);
		pModelAnimated->SetMaterial(playerDesc.pShirtMaterial, 1);
		pModelAnimated->SetMaterial(playerDesc.pGlovesMaterial, 2);
		pModelAnimated->SetMaterial(playerDesc.pGlovesMaterial, 3);
		pModelAnimated->SetMaterial(playerDesc.pMainMaterial, 4);
		pModelAnimated->SetMaterial(playerDesc.pBlackMaterial, 5);
		pModelAnimated->SetMaterial(playerDesc.pBlackMaterial, 6);
		pModelAnimated->SetMaterial(playerDesc.pMainMaterial, 7);
		pModelAnimated->SetMaterial(playerDesc.pMainMaterial, 8);
		pModelAnimated->SetMaterial(playerDesc.pMainMaterial, 9);
		pModelAnimated->SetMaterial(playerDesc.pGlovesMaterial, 10);
		pModelAnimated->SetMaterial(playerDesc.pSkinMaterial, 11);
		pModelAnimated->SetMaterial(playerDesc.pGoldMaterial, 12);
	}

	//Input
	{
		auto inputAction = InputAction(ToInputId(index, CharacterMoveLeft), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(ToInputId(index, CharacterMoveRight), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(ToInputId(index, CharacterMoveForward), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_UP, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(ToInputId(index, CharacterMoveBackward), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(ToInputId(index, CharacterPlaceBomb), InputState::pressed, -1, -1, XINPUT_GAMEPAD_A, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);
	}

	//UI
	const auto pSprite = AddChild(new GameObject);
	pSprite->AddComponent(new SpriteComponent(L"Textures/Bomberman/" + playerDesc.spriteName + L".png", {playerDesc.spritePivotX, playerDesc.spritePivotY}, {1.f,1.f,1.f,0.8f}));

	pSprite->GetTransform()->Translate(playerDesc.spriteOffsetX, playerDesc.spriteOffsetY, 0.f);
	pSprite->GetTransform()->Scale(0.5f);
}

int BombermanScene::ToInputId(int index, int basicInputId) const
{
	return index * m_NrPlayerInputs + basicInputId;
}

