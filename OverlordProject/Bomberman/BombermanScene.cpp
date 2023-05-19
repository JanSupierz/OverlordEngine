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

#include "Bomberman/Character.h"
#include "Bomberman/Grid.h"
#include "Bomberman/Bomb.h"
#include "Bomberman/Fire.h"

std::vector<GameObject*> BombermanScene::s_pObjectsToAdd{};
std::vector<GameObject*> BombermanScene::s_pObjectsToRemove{};
bool BombermanScene::s_CheckVectors{ false };

BombermanScene::BombermanScene() :
	GameScene(L"BombermanScene"), m_CubeSize{ 10.f }, 
	m_pGrid{ std::move(std::make_unique<Grid>(15, 15, m_CubeSize))
}
{
	m_pCharacters.resize(4);
}

BombermanScene::~BombermanScene()
{
	for (GameObject* pObject : s_pObjectsToAdd)
	{
		delete pObject;
	}

	for (GameObject* pObject : s_pObjectsToRemove)
	{
		delete pObject;
	}
}

void BombermanScene::AddGameObject(GameObject* pGameObject)
{
	s_pObjectsToAdd.push_back(pGameObject);
	s_CheckVectors = true;
}

void BombermanScene::RemoveGameObject(GameObject* pGameObject)
{
	s_pObjectsToRemove.push_back(pGameObject);
	s_CheckVectors = true;
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
}

void BombermanScene::Update()
{
	if (s_CheckVectors)
	{
		for (auto pObject : s_pObjectsToAdd)
		{
			AddChild(pObject);
		}

		for (auto pObject : s_pObjectsToRemove)
		{
			RemoveChild(pObject, true);
		}

		s_pObjectsToAdd.clear();
		s_pObjectsToRemove.clear();
	}
}

void BombermanScene::Draw()
{
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

void BombermanScene::CreateCube(int col, int row, int height, const std::wstring& meshFilePath, BaseMaterial* pColorMaterial, PxMaterial* pStaticMaterial, float heightOffset, float scale, bool disableRigidBody)
{
	const float cubeDimensions{ m_CubeSize * scale };
	const float halfCube{ cubeDimensions * 0.5f };

	Node* pNode{ m_pGrid->GetNode(col, row) };

	//Blocks above ground are blocking the player
	if (height == 1)
	pNode->SetBlocked(true);

	//Translate to the node position
	auto pObject{ AddChild(new GameObject) };
	const auto transform{ pObject->GetTransform() };
	const XMFLOAT2 nodePos{ pNode->GetWorldPos() };

	transform->Translate(nodePos.x, m_CubeSize * height + heightOffset, nodePos.y);
	transform->Scale(cubeDimensions);

	//Model
	auto pModel{ pObject->AddComponent(new ModelComponent(meshFilePath)) };
	pModel->SetMaterial(pColorMaterial);

	//Rigid body
	if (disableRigidBody) return;

	auto pRigid{ pObject->AddComponent(new RigidBodyComponent(true)) };

	const auto geo{ PxBoxGeometry{ halfCube,halfCube,halfCube } };
	pRigid->AddCollider(geo, *pStaticMaterial, false, PxTransform{ 0.f,halfCube - heightOffset,0.f });
}

void BombermanScene::InitArena()
{
	auto& physX = PxGetPhysics();
	auto pBouncyMaterial = physX.createMaterial(0.f, 0.f, 1.f);
	auto pStaticMaterial = physX.createMaterial(1.f, 1.f, 0.f);

	//Set bomb materials
	const auto pBombMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>() };
	pBombMaterial->SetDiffuseTexture(L"Textures/Bomberman/Bomb.png");

	Bomb::SetBombMaterials(pBombMaterial, pStaticMaterial);

	//Set Fire materials
	const auto pFireMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>() };
	pFireMaterial->SetDiffuseTexture(L"Textures/Bomberman/Bomb.png");

	Fire::SetFireMaterials(pFireMaterial, pStaticMaterial);

	//Create ground
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

	//Side Cubes
	std::wstring meshFilePath{ L"Meshes/Bomberman/SideCubes.ovm" };

	const int nrCols{ m_pGrid->GetNrCols() };
	for (int col{}; col < nrCols; ++col)
	{
		CreateCube(col, 0, 1, meshFilePath, pColorMaterial, pStaticMaterial);
	}

	const int nrRows{ m_pGrid->GetNrRows() };
	for (int col{}; col < nrCols; ++col)
	{
		CreateCube(col, nrRows - 1, 1, meshFilePath, pColorMaterial, pStaticMaterial);
	}

	for (int row{ 1 }; row < nrRows - 1; ++row)
	{
		CreateCube(0, row, 1, meshFilePath, pColorMaterial, pStaticMaterial);
	}

	for (int row{ 1 }; row < nrRows - 1; ++row)
	{
		CreateCube(nrCols - 1, row, 1, meshFilePath, pColorMaterial, pStaticMaterial);
	}

	//Ground Cubes
	const auto pLightColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	pLightColorMaterial->SetColor(DirectX::Colors::LightGreen);
	
	const auto pDarkColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	pDarkColorMaterial->SetColor(DirectX::Colors::DarkGreen);
	
	bool isDark{ true };
	
	meshFilePath = L"Meshes/Bomberman/GroundCubes.ovm";
	for (int row{}; row < nrRows; ++row)
	{
		for (int col{}; col < nrCols; ++col)
		{
			CreateCube(col, row, 0, meshFilePath, isDark ? pDarkColorMaterial : pLightColorMaterial, pStaticMaterial, 0.5f * m_CubeSize, 1.f, true);
	
			isDark = !isDark;
		}
	}
	
	pObject = AddChild(new GameObject);
	const auto pRigid{ pObject->AddComponent(new RigidBodyComponent(true)) };
	
	const auto geoGround{ PxBoxGeometry{ m_CubeSize * nrCols * 0.5f, m_CubeSize, m_CubeSize * nrRows * 0.5f} };
	pRigid->AddCollider(geoGround, *pStaticMaterial, false, PxTransform{ 0.f,0.f,0.f });
	
	//Static obstacles
	const auto pGreyColorMaterial = MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow>();
	pGreyColorMaterial->SetColor(DirectX::Colors::DimGray);
	
	constexpr float scale{ 0.7f };
	meshFilePath = L"Meshes/Bomberman/CenterCubes.ovm";

	for (int row{ 2 }; row < nrRows - 2; row += 2)
	{
		for (int col{ 2 }; col < nrCols - 2; col += 2)
		{
			CreateCube(col, row, 1, meshFilePath, pGreyColorMaterial, pStaticMaterial, 0.5f * m_CubeSize * scale, scale);
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

		m_pCharacters[index] = AddChild(new Character(characterDesc, m_pGrid.get()));
		m_pCharacters[index]->GetTransform()->Translate(0.f, 2 * playerHeight, 0.f);
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
		transform->Rotate(0.f, 180.f, 0.f);

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
	{
		const auto pSprite = AddChild(new GameObject);
		pSprite->AddComponent(new SpriteComponent(L"Textures/Bomberman/" + playerDesc.spriteName + L".png", { playerDesc.spritePivotX, playerDesc.spritePivotY }, { 1.f,1.f,1.f,0.8f }));

		pSprite->GetTransform()->Translate(playerDesc.spriteOffsetX, playerDesc.spriteOffsetY, 0.f);
		pSprite->GetTransform()->Scale(0.5f);
	}
}

int BombermanScene::ToInputId(int index, int basicInputId) const
{
	return index * m_NrPlayerInputs + basicInputId;
}

