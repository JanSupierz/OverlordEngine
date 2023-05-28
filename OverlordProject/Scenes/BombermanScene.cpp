#include "stdafx.h"
#include <cstdlib>
#include <ctime>

#include "BombermanScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

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
#include "Bomberman/Cube.h"
#include "Bomberman/PickUp.h"
#include "Materials/Post/PostChromaticAberration.h"

std::vector<GameObject*> BombermanScene::s_pObjectsToAdd{};
std::vector<GameObject*> BombermanScene::s_pObjectsToRemove{};
bool BombermanScene::s_CheckVectors{ false };

BombermanScene* BombermanScene::s_CurrentScene{};

BombermanScene::BombermanScene() :
	GameScene(L"BombermanScene"), m_CubeSize{ 10.f }, m_pChromatic{ nullptr }, 
	m_pGrid{ std::move(std::make_unique<Grid>(15, 15, m_CubeSize)) }, m_DefaultCameraOffset{ 0, m_MaxCameraHeight, -65.f }
{
	s_CurrentScene = this;

	//Init random values
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	m_pCharacters.resize(4);
	
	for (int index{}; index < 4; ++index)
	{
		m_pCharacters[index] = nullptr;
	}
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

BombermanScene* BombermanScene::GetCurrent()
{
	return s_CurrentScene;
}

void BombermanScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = false;

	m_SceneContext.pLights->SetDirectionalLight({ -60.f,100.f, 10.f }, { 0.4f, -0.7f, 0.f });

	InitArena();
	
	m_pFixedCamera = new FixedCamera();
	m_pFixedCamera->GetTransform()->Translate(m_DefaultCameraOffset);
	m_pFixedCamera->GetTransform()->Rotate(70, 0, 0);
	AddChild(m_pFixedCamera);
	
	m_pCameraComp = m_pFixedCamera->GetComponent<CameraComponent>();
	SetActiveCamera(m_pCameraComp);

	//Add post-processing effect
	m_pChromatic = MaterialManager::Get()->CreateMaterial<PostChromaticAberration>();
	m_pChromatic->SetIsEnabled(false);
	AddPostProcessingEffect(m_pChromatic);
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

	if (Bomb::CheckExplosion())
	{
		m_ScreenShakeTimer = m_MaxScreenShakeDuration;
		m_pChromatic->SetIsEnabled(true);
	}

	if (m_ScreenShakeTimer > 0.f)
	{
		m_ScreenShakeTimer -= m_SceneContext.pGameTime->GetElapsed();

		if (m_ScreenShakeTimer <= 0.f)
		{
			m_pChromatic->SetIsEnabled(false);
		}
	}

	XMFLOAT3 cameraPosition{};
	XMVECTOR cameraVector{ XMLoadFloat3(&cameraPosition) };
	int nrPlayers{};
	float largestDistance{};

	for (Character* pCharacter : m_pCharacters)
	{
		if (pCharacter && pCharacter->GetIsActive())
		{
			XMFLOAT3 position{ pCharacter->GetTransform()->GetWorldPosition() };
			XMVECTOR positionVector{ XMLoadFloat3(&position) };

			cameraVector += positionVector;
			++nrPlayers;

			//Get largest distance between players
			for (Character* pOther : m_pCharacters)
			{
				if (pOther != pCharacter && pOther && pOther->GetIsActive())
				{
					XMFLOAT3 otherPosition{ pOther->GetTransform()->GetWorldPosition() };
					XMVECTOR otherVector{ XMLoadFloat3(&otherPosition) };
					
					const float distance{ XMVectorGetX(XMVector3Length(otherVector - positionVector)) };
					if (distance > largestDistance)
					{
						largestDistance = distance;
					}
				}
			}
		}
	}

	//Average player position
	if(nrPlayers > 0)
	cameraVector /= static_cast<float>(nrPlayers);

	//Lerp towards the desired position
	const float t{ m_SceneContext.pGameTime->GetElapsed() * 1.1f };
	cameraVector = XMVectorLerp(XMLoadFloat3(&m_LastCameraPosition), cameraVector, t);

	XMStoreFloat3(&cameraPosition, cameraVector);

	m_LastCameraPosition = cameraPosition;

	//Fov interpolation
	const float fov{ XM_PIDIV4 - (1.f - (largestDistance / 80.0f)) * XM_PIDIV4 * 0.1f };
	m_LastFov = std::lerp(m_LastFov, fov, t);

	m_pCameraComp->SetFieldOfView(m_LastFov);

	//Add basic offset
	cameraVector += XMLoadFloat3(&m_DefaultCameraOffset);

	m_pFixedCamera->GetTransform()->Translate(cameraVector);
}

void BombermanScene::Draw()
{
}

void BombermanScene::OnGUI()
{
}

void BombermanScene::CreateCube(bool isDestructible, int col, int row, int height, const std::wstring& meshFilePath, BaseMaterial* pColorMaterial, PxMaterial* pStaticMaterial, float heightOffset, float scale, bool disableRigidBody)
{
	const float cubeDimensions{ m_CubeSize * scale };
	const float halfCube{ cubeDimensions * 0.5f };

	Node* pNode{ m_pGrid->GetNode(col, row) };

	//Skip if node is not free
	if (height == 1 && pNode->GetCellState() != CellState::Empty) return;

	//Translate to the node position
	GameObject* pObject{};

	if (isDestructible)
	{
		pObject = AddChild(new Cube(pNode, m_pGrid.get()));
	}
	else
	{
		pObject = AddChild(new GameObject());

		//Blocks above ground are blocking fire
		if (height == 1)
		{
			pNode->SetCellState(CellState::NonDestructible);
		}
	}

	const auto transform{ pObject->GetTransform() };
	const XMFLOAT2 nodePos{ pNode->GetWorldPos() };

	transform->Translate(nodePos.x, m_CubeSize * height + heightOffset, nodePos.y);
	transform->Scale(cubeDimensions);

	//Model
	auto pModel{ pObject->AddComponent(new ModelComponent(meshFilePath)) };
	pModel->SetMaterial(pColorMaterial);

	//Rigid body
	if (disableRigidBody) return;

	auto pRigid{ pObject->AddComponent(new RigidBodyComponent(!isDestructible)) };

	const auto geo{ PxBoxGeometry{ halfCube,halfCube,halfCube } };
	pRigid->AddCollider(geo, *pStaticMaterial, false, PxTransform{ 0.f,halfCube - heightOffset,0.f });

	if (isDestructible)
	{
		pRigid->SetConstraint(RigidBodyConstraint::All, false);
	}
}

void BombermanScene::InitArena()
{
	auto& physX = PxGetPhysics();
	auto pBouncyMaterial = physX.createMaterial(0.f, 0.f, 1.f);
	auto pStaticMaterial = physX.createMaterial(1.f, 1.f, 0.f);

	//Set bomb materials
	{
		const auto pBombMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>() };
		pBombMaterial->SetDiffuseTexture(L"Textures/Bomberman/Bomb.png");

		Bomb::SetBombMaterials(pBombMaterial, pStaticMaterial);
	}

	//Set Fire materials
	{
		const auto pFireMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>() };
		pFireMaterial->SetDiffuseTexture(L"Textures/Bomberman/Bomb.png");

		Fire::SetFireMaterials(pFireMaterial, pStaticMaterial);
	}

	//Set pick ups materials
	{
		PickUp::SetPhysicsMaterial(pStaticMaterial);

		const auto pFrameMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
		pFrameMaterial->SetColor(DirectX::Colors::DarkGray);

		PickUp::SetPickUpMaterial(PickUpType::None, pFrameMaterial);

		auto pPickUpMaterial{ MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow_Skinned>() };
		pPickUpMaterial->SetDiffuseTexture(L"Textures/Bomberman/PickUp/Bomb.png");

		PickUp::SetPickUpMaterial(PickUpType::PlaceTwoBombs, pPickUpMaterial);
		PickUp::SetPickUpMaterial(PickUpType::DoubleRange, pPickUpMaterial);
		PickUp::SetPickUpMaterial(PickUpType::NotBlockable, pPickUpMaterial);
	}

	//Create ground
	auto pColorMaterialNoShadow = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pColorMaterialNoShadow->SetDiffuseTexture(L"Textures/Bomberman/GroundWood.jpg");

	auto pObject = AddChild(new GameObject);
	auto pModel = pObject->AddComponent(new ModelComponent(L"Meshes/UnitPlane.ovm"));
	pModel->SetMaterial(pColorMaterialNoShadow);
	pModel->GetTransform()->Scale(50.f);

	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	auto pColorMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pColorMaterial->SetDiffuseTexture(L"Textures/Bomberman/SideCube_Diffuse.png");

	//Side Cubes
	std::wstring meshFilePath{ L"Meshes/Bomberman/SideCubes.ovm" };

	const int nrCols{ m_pGrid->GetNrCols() };
	for (int col{}; col < nrCols; ++col)
	{
		CreateCube(false, col, 0, 1, meshFilePath, pColorMaterial, pStaticMaterial);
	}

	const int nrRows{ m_pGrid->GetNrRows() };
	for (int col{}; col < nrCols; ++col)
	{
		CreateCube(false, col, nrRows - 1, 1, meshFilePath, pColorMaterial, pStaticMaterial);
	}

	for (int row{ 1 }; row < nrRows - 1; ++row)
	{
		CreateCube(false, 0, row, 1, meshFilePath, pColorMaterial, pStaticMaterial);
	}

	for (int row{ 1 }; row < nrRows - 1; ++row)
	{
		CreateCube(false, nrCols - 1, row, 1, meshFilePath, pColorMaterial, pStaticMaterial);
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
			CreateCube(false, col, row, 0, meshFilePath, isDark ? pDarkColorMaterial : pLightColorMaterial, pStaticMaterial, 0.5f * m_CubeSize, 1.f, true);
	
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
			CreateCube(false, col, row, 1, meshFilePath, pGreyColorMaterial, pStaticMaterial, 0.5f * m_CubeSize * scale, scale);
		}
	}

	//Destructible Cubes
	meshFilePath = L"Meshes/Bomberman/Rock.ovm";
	const auto pRockMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial_Shadow>();
	pRockMaterial->SetDiffuseTexture(L"Textures/Bomberman/Rock_Diffuse.jpg");

	//Leave space for the players
	std::vector<std::pair<int, int>> reserved;
	reserved.emplace_back(1, 1);
	reserved.emplace_back(2, 1);
	reserved.emplace_back(1, 2);

	reserved.emplace_back(1, nrCols - 2);
	reserved.emplace_back(2, nrCols - 2);
	reserved.emplace_back(1, nrCols - 3);
	
	reserved.emplace_back(nrRows - 2, nrCols - 2);
	reserved.emplace_back(nrRows - 3, nrCols - 2);
	reserved.emplace_back(nrRows - 2, nrCols - 3);

	reserved.emplace_back(nrRows - 2, 1);
	reserved.emplace_back(nrRows - 3, 1);
	reserved.emplace_back(nrRows - 2, 2);

	for (int row{ 1 }; row < nrRows - 1; ++row)
	{
		for (int col{ 1 }; col < nrCols - 1; ++col)
		{
			if(!std::any_of(reserved.begin(), reserved.end(), [&](const std::pair<int, int>& pair) { return pair.first == col && pair.second == row; }))
			{ 
				//CreateCube(true, col, row, 1, meshFilePath, pRockMaterial, pStaticMaterial);
			}
		}
	}
}

void BombermanScene::InitPlayer(const PlayerDesc& playerDesc)
{
	++m_NrPlayers;

	constexpr float animationMeshSize{ 100.f };
	constexpr float animationMeshScale{ 0.1f };

	constexpr float playerHeight{ animationMeshSize * animationMeshScale };
	constexpr float animationMeshOffset{ -0.85f * playerHeight };

	const auto index{ playerDesc.index };
	const auto gamepadIndex{ static_cast<GamepadIndex>(index) };

	//Character
	{
		const int nrRows{ m_pGrid->GetNrRows() };
		const int nrCols{ m_pGrid->GetNrCols() };

		XMFLOAT2 nodePos{};
		float angle{};

		switch (index)
		{
		case 0:
			nodePos = m_pGrid->GetNode(1, 1)->GetWorldPos();
			angle = 45.f;
			break;
		case 1:
			nodePos = m_pGrid->GetNode(nrCols - 2, 1)->GetWorldPos();
			angle = -45;
			break;
		case 2:
			nodePos = m_pGrid->GetNode(nrCols - 2, nrRows - 2)->GetWorldPos();
			angle = -135;
			break;
		case 3:
		default:
			nodePos = m_pGrid->GetNode(1, nrRows - 2)->GetWorldPos();
			angle = 135;
			break;
		}

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
		characterDesc.startYaw = angle;

		characterDesc.clipId_Death = playerDesc.clipId_Death;
		characterDesc.clipId_PlaceBomb = playerDesc.clipId_PlaceBomb;
		characterDesc.clipId_Floating = playerDesc.clipId_Floating;
		characterDesc.clipId_Idle = playerDesc.clipId_Idle;
		characterDesc.clipId_Walking = playerDesc.clipId_Walking;

		m_pCharacters[index] = AddChild(new Character(characterDesc, m_pGrid.get()));

		m_pCharacters[index]->GetTransform()->Translate(nodePos.x, 2 * playerHeight, nodePos.y);
		m_pCharacters[index]->GetTransform()->Rotate(0.f, angle, 0.f);
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

		float offsetX{ 20.f };
		float spritePivotX{}, spriteOffsetX{ offsetX }, spriteOffsetY{ 100 };

		if (m_NrPlayers % 2 == 0)
		{
			spritePivotX = 1.f;
			spriteOffsetX = m_SceneContext.windowWidth - offsetX;
		}

		if (m_NrPlayers > 2)
		{
			spriteOffsetY += 200;
		}

		pSprite->AddComponent(new SpriteComponent(playerDesc.spriteName, { spritePivotX, 0.f }, { 1.f,1.f,1.f,1.f }));

		pSprite->GetTransform()->Translate( spriteOffsetX, spriteOffsetY, 0.f);
		pSprite->GetTransform()->Scale(0.5f);
	}
}

int BombermanScene::ToInputId(int index, int basicInputId) const
{
	return index * m_NrPlayerInputs + basicInputId;
}

