#include "stdafx.h"
#include "MainMenuScene.h"
#include "BombermanScene.h"
#include "Bomberman/PlayerJoinIcon.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/ColorMaterial_Shadow_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Materials/Shadow/ColorMaterial_Shadow.h"

MainMenuScene::MainMenuScene()
	:GameScene(L"MainMenuScene")
{
}

MainMenuScene::~MainMenuScene()
{
}

void MainMenuScene::Initialize()
{
	m_SceneContext.settings.drawGrid = false;
	m_SceneContext.settings.drawPhysXDebug = false;
	m_SceneContext.settings.enableOnGUI = true;

	auto pBackGroundImage{ AddChild(new GameObject()) };
	pBackGroundImage->AddComponent(new SpriteComponent(L"Textures/StartScreenStatic.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,1.f }));

	pBackGroundImage->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .9f);
	pBackGroundImage->GetTransform()->Scale(0.7f, 0.7f, 1.f);

	auto pFrontMovingImage{ AddChild(new GameObject()) };
	auto pSprite{ pFrontMovingImage->AddComponent(new SpriteComponent(L"Textures/StartScreenMoving2.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,1.f })) };
	pSprite->SetMoveDirection(XMFLOAT2{ 0.02f, 0.f });

	pFrontMovingImage->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .8f);
	pFrontMovingImage->GetTransform()->Scale(0.7f, 0.7f, 1.f);

	pFrontMovingImage = AddChild(new GameObject());
	pSprite = pFrontMovingImage->AddComponent(new SpriteComponent(L"Textures/StartScreenMoving1.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,1.f }));
	pSprite->SetMoveDirection(XMFLOAT2{ -0.03f, 0.f });

	pFrontMovingImage->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .7f);
	pFrontMovingImage->GetTransform()->Scale(0.7f, 0.7f, 1.f);

	m_pFixedCamera = new FixedCamera();
	m_pFixedCamera->GetTransform()->Translate(0.f, 0.f, 0.f);
	m_pFixedCamera->GetTransform()->Rotate(0.f, 0.f, 0.f);
	AddChild(m_pFixedCamera);

	SetActiveCamera(m_pFixedCamera->GetComponent<CameraComponent>());

	m_PlayerTextureNames.emplace_back(L"Textures/Bomberman/Icon_White.png");
	m_PlayerTextureNames.emplace_back(L"Textures/Bomberman/Icon_Black.png");
	m_PlayerTextureNames.emplace_back(L"Textures/Bomberman/Icon_Red.png");
	m_PlayerTextureNames.emplace_back(L"Textures/Bomberman/Icon_Blue.png");

	m_Texts.emplace_back("P1");
	m_Texts.emplace_back("P2");
	m_Texts.emplace_back("P3");
	m_Texts.emplace_back("P4");

	m_PlayerTexts.emplace_back("");
	m_PlayerTexts.emplace_back("");
	m_PlayerTexts.emplace_back("");
	m_PlayerTexts.emplace_back("");


	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	for (int index{}; index < 4; ++index)
	{
		GamepadIndex gamepadIndex{ static_cast<GamepadIndex>(index) };

		auto inputAction = InputAction(Join + (m_NumberInputIds * index), InputState::pressed, -1, -1, XINPUT_GAMEPAD_START, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(Start + (m_NumberInputIds * index), InputState::pressed, -1, -1, XINPUT_GAMEPAD_A, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(Next + (m_NumberInputIds * index), InputState::pressed, -1, -1, XINPUT_GAMEPAD_DPAD_DOWN, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		inputAction = InputAction(Previous + (m_NumberInputIds * index), InputState::pressed, -1, -1, XINPUT_GAMEPAD_DPAD_UP, gamepadIndex);
		m_SceneContext.pInput->AddInputAction(inputAction);

		//Add 4 icons
		auto pPlayerIconObject{ AddChild(new PlayerJoinIcon(m_PlayerTextureNames[index], m_pFont)) };

		constexpr float scale{ 0.5f };
		const float height{ m_SceneContext.windowHeight / 6.f };

		pPlayerIconObject->GetTransform()->Translate(9.2f * m_SceneContext.windowWidth / 10.f, height * (index + 1), .8f);
		pPlayerIconObject->GetTransform()->Scale(scale, scale, 1.f);

		pPlayerIconObject->InitPosition();

		m_pIcons.emplace_back(pPlayerIconObject);
	}

	for (int index{}; index < m_pIcons.size() - 1; ++index)
	{
		m_pIcons[index]->SetNext(m_pIcons[index + 1]);
	}

	m_pIcons[m_pIcons.size() - 1]->SetNext(m_pIcons[0]);
}

void MainMenuScene::Update()
{
	for (int index{}; index < 4; ++index)
	{
		int actionStartIndex{ (m_NumberInputIds * index) };

		if (!m_Joined[index] && m_SceneContext.pInput->IsActionTriggered(Join + actionStartIndex))
		{
			AddPlayer(index);

			for (PlayerJoinIcon* pIcon : m_pIcons)
			{
				if (pIcon->IsFree())
				{
					pIcon->SetPlayer(index, m_PlayerTexts[index]);
					break;
				}
			}
		}
		else if (m_Joined[index])
		{
			if (m_SceneContext.pInput->IsActionTriggered(Next + actionStartIndex))
			{
				//Loop to find your current icon
				for (PlayerJoinIcon* pIcon : m_pIcons)
				{
					if (pIcon->GetPlayerIndex() == index)
					{
						//Remove yourself
						pIcon->SetPlayer(-1);
						
						//Get next
						PlayerJoinIcon* pNext{ pIcon->GetNext() };

						//If the next node is not free, check the next one
						//Stop if you are the next node
						while (pNext != pIcon && !pNext->IsFree())
						{
							pNext = pNext->GetNext();
						}

						pNext->SetPlayer(index, m_PlayerTexts[index]);

						break;
					}
				}
			}
			else if(m_SceneContext.pInput->IsActionTriggered(Previous + actionStartIndex))
			{
				//Loop to find your current icon
				for (PlayerJoinIcon* pIcon : m_pIcons)
				{
					if (pIcon->GetPlayerIndex() == index)
					{
						//Remove yourself
						pIcon->SetPlayer(-1);

						//Get previous
						PlayerJoinIcon* pPrevious{ pIcon->GetPrevious() };

						//If the previous node is not free, check the previous one
						//Stop if you are the previous node
						while (pPrevious != pIcon && !pPrevious->IsFree())
						{
							pPrevious = pPrevious->GetPrevious();
						}

						pPrevious->SetPlayer(index, m_PlayerTexts[index]);

						break;
					}
				}
			}
			else if (m_SceneContext.pInput->IsActionTriggered(Start + actionStartIndex))
			{
				//Init materials
				const auto pBlackMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
				pBlackMaterial->SetColor(DirectX::Colors::Black);

				const auto pWhiteMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
				pWhiteMaterial->SetColor(DirectX::Colors::White);

				const auto pPinkMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
				pPinkMaterial->SetColor(DirectX::Colors::HotPink);

				const auto pBlueMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
				pBlueMaterial->SetColor(DirectX::Colors::Blue);

				const auto pRedMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
				pRedMaterial->SetColor(DirectX::Colors::Red);

				const auto pGoldMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
				pGoldMaterial->SetColor(DirectX::Colors::Gold);

				const auto pSkinMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial_Shadow_Skinned>() };
				pSkinMaterial->SetColor(DirectX::Colors::Bisque);

				auto& physX = PxGetPhysics();
				auto pDefaultMaterial = physX.createMaterial(0.5f, 0.5f, 0.1f);

				//Init selected players
				BombermanScene* pCurrent{ BombermanScene::GetCurrent() };

				if (!m_pIcons[0]->IsFree())
				{
					PlayerDesc playerDesc{ m_pIcons[0]->GetPlayerIndex(),pDefaultMaterial };
					playerDesc.pShirtMaterial = pBlueMaterial;
					playerDesc.pMainMaterial = pWhiteMaterial;
					playerDesc.pBlackMaterial = pBlackMaterial;
					playerDesc.pGoldMaterial = pGoldMaterial;
					playerDesc.pGlovesMaterial = pPinkMaterial;
					playerDesc.pSkinMaterial = pSkinMaterial;
					playerDesc.spriteName = m_PlayerTextureNames[0];

					pCurrent->InitPlayer(playerDesc);
				}

				if (!m_pIcons[1]->IsFree())
				{
					PlayerDesc playerDesc{ m_pIcons[1]->GetPlayerIndex(),pDefaultMaterial };
					playerDesc.pShirtMaterial = pBlackMaterial;
					playerDesc.pMainMaterial = pBlackMaterial;
					playerDesc.pBlackMaterial = pBlackMaterial;
					playerDesc.pGoldMaterial = pGoldMaterial;
					playerDesc.pGlovesMaterial = pPinkMaterial;
					playerDesc.pSkinMaterial = pSkinMaterial;
					playerDesc.spriteName = m_PlayerTextureNames[1];

					pCurrent->InitPlayer(playerDesc);
				}

				if (!m_pIcons[2]->IsFree())
				{
					PlayerDesc playerDesc{ m_pIcons[2]->GetPlayerIndex(),pDefaultMaterial };
					playerDesc.pShirtMaterial = pRedMaterial;
					playerDesc.pMainMaterial = pRedMaterial;
					playerDesc.pBlackMaterial = pBlackMaterial;
					playerDesc.pGoldMaterial = pGoldMaterial;
					playerDesc.pGlovesMaterial = pPinkMaterial;
					playerDesc.pSkinMaterial = pSkinMaterial;
					playerDesc.spriteName = m_PlayerTextureNames[2];

					pCurrent->InitPlayer(playerDesc);
				}

				if (!m_pIcons[3]->IsFree())
				{
					PlayerDesc playerDesc{ m_pIcons[3]->GetPlayerIndex(),pDefaultMaterial };
					playerDesc.pShirtMaterial = pBlueMaterial;
					playerDesc.pMainMaterial = pBlueMaterial;
					playerDesc.pBlackMaterial = pBlackMaterial;
					playerDesc.pGoldMaterial = pGoldMaterial;
					playerDesc.pGlovesMaterial = pPinkMaterial;
					playerDesc.pSkinMaterial = pSkinMaterial;
					playerDesc.spriteName = m_PlayerTextureNames[3];

					pCurrent->InitPlayer(playerDesc);
				}

				SceneManager::Get()->NextScene();
			}
		}
	}
}

void MainMenuScene::Draw()
{

}

void MainMenuScene::OnGUI()
{
}

void MainMenuScene::AddPlayer(int gamepadIndex)
{
	m_PlayerTexts[gamepadIndex] = m_Texts[m_NrPlayers];
	m_Joined[gamepadIndex] = true;

	++m_NrPlayers;
}
