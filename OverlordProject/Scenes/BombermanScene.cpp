#include "stdafx.h"
#include "BombermanScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

#include "Prefabs/TorusPrefab.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/ColorMaterial_Shadow_Skinned.h"
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
		InitPlayer(0, pWhiteMaterial, pBlueMaterial, pBlackMaterial, pPinkMaterial, pSkinMaterial, pGoldMaterial, pDefaultMaterial);
		InitPlayer(1, pBlackMaterial, pBlueMaterial, pBlackMaterial, pPinkMaterial, pSkinMaterial, pGoldMaterial, pDefaultMaterial);
	}

	m_pFixedCamera = new FixedCamera();
	m_pFixedCamera->GetTransform()->Translate(0, m_MaxCameraHeight, -80);
	m_pFixedCamera->GetTransform()->Rotate(70, 0, 0);
	AddChild(m_pFixedCamera);
	
	SetActiveCamera(m_pFixedCamera->GetComponent<CameraComponent>());

	const auto pWhiteMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
	pWhiteMaterial->SetColor(DirectX::Colors::White);

	auto pObject = AddChild(new GameObject);
	auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/Bonus.ovm"));
	pModel->SetMaterial(pWhiteMaterial);
	pModel->GetTransform()->Scale(0.1f);
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

void BombermanScene::InitPlayer(int index,
	ColorMaterial_Shadow_Skinned* pCharacterMaterial,
	ColorMaterial_Shadow_Skinned* pBodyMaterial,
	ColorMaterial_Shadow_Skinned* pDetailMaterial,
	ColorMaterial_Shadow_Skinned* pHandsMaterial,
	ColorMaterial_Shadow_Skinned* pSkinMaterial,
	ColorMaterial_Shadow_Skinned* pGoldMaterial,
	PxMaterial* pDefaultMaterial)
{
	constexpr float animationMeshSize{ 100.f };
	constexpr float animationMeshScale{ 0.1f };

	constexpr float playerHeight{ animationMeshSize * animationMeshScale };
	constexpr float animationMeshOffset{ -0.575f * playerHeight };

	const auto gamepadIndex{ static_cast<GamepadIndex>(index) };

	//Character
	CharacterDesc characterDesc{ pDefaultMaterial };
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

	characterDesc.clipId_Death = 1;
	characterDesc.clipId_PlaceBomb = 2;
	characterDesc.clipId_Floating = 3;
	characterDesc.clipId_Idle = 4;
	characterDesc.clipId_Walking = 6;

	//0 -> dancing
	//1 -> death
	//2 -> place bomb
	//3 -> floating
	//4 -> idle
	//5 -> loose
	//6 -> running

	m_pCharacters[index] = AddChild(new Character(characterDesc));
	m_pCharacters[index]->GetTransform()->Translate(0, 20.f, 0.f);

	//Animation
	const auto pSkinnedMaterial{ pCharacterMaterial };

	const auto pModelObject{ m_pCharacters[index]->AddChild(new GameObject()) };

	const auto pModelAnimated = pModelObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/Player.ovm"));
	pModelAnimated->SetMaterial(pSkinnedMaterial);
	m_pCharacters[index]->SetAnimator(pModelAnimated->GetAnimator());

	const auto transform{ pModelAnimated->GetTransform() };
	transform->Scale(animationMeshScale);
	transform->Translate(0.f, animationMeshOffset, 0.f);

	pModelAnimated->SetMaterial(pDetailMaterial, 0);
	pModelAnimated->SetMaterial(pBodyMaterial, 1);
	pModelAnimated->SetMaterial(pHandsMaterial, 2);
	pModelAnimated->SetMaterial(pHandsMaterial, 3);
	pModelAnimated->SetMaterial(pCharacterMaterial, 4);
	pModelAnimated->SetMaterial(pDetailMaterial, 5);
	pModelAnimated->SetMaterial(pDetailMaterial, 6);
	pModelAnimated->SetMaterial(pCharacterMaterial, 7);
	pModelAnimated->SetMaterial(pCharacterMaterial, 8);
	pModelAnimated->SetMaterial(pCharacterMaterial, 9);
	pModelAnimated->SetMaterial(pHandsMaterial, 10);
	pModelAnimated->SetMaterial(pSkinMaterial, 11);
	pModelAnimated->SetMaterial(pGoldMaterial, 12);

	//Input
	auto inputAction = InputAction(ToInputId(index, CharacterMoveLeft), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT, gamepadIndex);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(ToInputId(index, CharacterMoveRight), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT, gamepadIndex);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(ToInputId(index, CharacterMoveForward), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_UP, gamepadIndex);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(ToInputId(index, CharacterMoveBackward), InputState::down, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, gamepadIndex);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(ToInputId(index,CharacterPlaceBomb), InputState::pressed, -1, -1, XINPUT_GAMEPAD_A, gamepadIndex);
	m_SceneContext.pInput->AddInputAction(inputAction);
}

int BombermanScene::ToInputId(int index, int basicInputId) const
{
	return index * m_NrPlayerInputs + basicInputId;
}

