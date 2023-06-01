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
	m_SceneContext.settings.enableOnGUI = false;

	auto pBackGroundImage{ AddChild(new GameObject()) };
	pBackGroundImage->AddComponent(new SpriteComponent(L"Textures/Menu/StartScreenStatic.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,1.f }));

	pBackGroundImage->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .9f);
	pBackGroundImage->GetTransform()->Scale(0.7f, 0.7f, 1.f);

	auto pFrontMovingImage{ AddChild(new GameObject()) };
	auto pSprite{ pFrontMovingImage->AddComponent(new SpriteComponent(L"Textures/Menu/StartScreenMoving2.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,1.f })) };
	pSprite->SetMoveDirection(XMFLOAT2{ 0.02f, 0.f });

	pFrontMovingImage->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .8f);
	pFrontMovingImage->GetTransform()->Scale(0.7f, 0.7f, 1.f);

	pFrontMovingImage = AddChild(new GameObject());
	pSprite = pFrontMovingImage->AddComponent(new SpriteComponent(L"Textures/Menu/StartScreenMoving1.png", { 0.5f,0.5f }, { 1.f,1.f,1.f,1.f }));
	pSprite->SetMoveDirection(XMFLOAT2{ -0.03f, 0.f });

	pFrontMovingImage->GetTransform()->Translate(m_SceneContext.windowWidth / 2.f, m_SceneContext.windowHeight / 2.f, .7f);
	pFrontMovingImage->GetTransform()->Scale(0.7f, 0.7f, 1.f);

	m_pFixedCamera = new FixedCamera();
	m_pFixedCamera->GetTransform()->Translate(0.f, 0.f, 0.f);
	m_pFixedCamera->GetTransform()->Rotate(0.f, 0.f, 0.f);
	AddChild(m_pFixedCamera);

	SetActiveCamera(m_pFixedCamera->GetComponent<CameraComponent>());

	//Button Start
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	auto pButtonStart{ AddChild(new GameObject()) };
	pSprite = pButtonStart->AddComponent(new SpriteComponent(L"Textures/Menu/ButtonSelected.png", { 0.5f,0.f }, { 1.f,1.f,1.f,1.f }));
	pButtonStart->GetTransform()->Translate(450.f, 500.f, 0.f);
	pButtonStart->GetTransform()->Scale(0.5f);

	m_pSprites.emplace_back(pSprite);
	m_ButtonTexts.emplace_back("Start Game");
	m_ButtonTextPositions.emplace_back(XMFLOAT2{380.f,505.f});

	//Button Quit
	auto pButtonQuit{ AddChild(new GameObject()) };
	pSprite = pButtonQuit->AddComponent(new SpriteComponent(L"Textures/Menu/Button.png", { 0.5f,0.0f }, { 1.f,1.f,1.f,1.f }));
	pButtonQuit->GetTransform()->Translate(850.f, 500.f, 0.f);
	pButtonQuit->GetTransform()->Scale(0.5f);

	m_pSprites.emplace_back(pSprite);
	m_ButtonTexts.emplace_back("Quit Game");
	m_ButtonTextPositions.emplace_back(XMFLOAT2{ 790.f,505.f });

	//Input
	auto inputAction{ InputAction(Start, InputState::pressed, -1, -1, XINPUT_GAMEPAD_A) };
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Next, InputState::pressed, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Previous, InputState::pressed, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Sound 2D
	const auto pFmod = SoundManager::Get()->GetSystem();

	pFmod->createStream("Resources/Sounds/Menu.mp3", FMOD_2D | FMOD_LOOP_NORMAL, nullptr, &m_pMusicSound);
	pFmod->createStream("Resources/Sounds/AUD_Click.wav", FMOD_2D | FMOD_LOOP_OFF, nullptr, &m_pClickSound);
	pFmod->createStream("Resources/Sounds/AUD_Select.wav", FMOD_2D | FMOD_LOOP_OFF, nullptr, &m_pSelectSound);
}

void MainMenuScene::Update()
{
	if (m_SceneContext.pInput->IsActionTriggered(Start))
	{
		if (m_CurrentButton == Button::start)
		{
			SceneManager::Get()->NextScene();
		}
		else
		{
			SceneManager::Get()->Quit();
		}

		SoundManager::Get()->GetSystem()->playSound(m_pClickSound, nullptr, false, &m_pChannelEffects2D);
	}
	else if (m_SceneContext.pInput->IsActionTriggered(Next) || m_SceneContext.pInput->IsActionTriggered(Previous))
	{
		if (m_CurrentButton == Button::start)
		{
			m_CurrentButton = Button::quit;
			m_pSprites[0]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[1]->SetTexture(L"Textures/Menu/ButtonSelected.png");
		}
		else
		{
			m_CurrentButton = Button::start;
			m_pSprites[1]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[0]->SetTexture(L"Textures/Menu/ButtonSelected.png");
		}

		SoundManager::Get()->GetSystem()->playSound(m_pSelectSound, nullptr, false, &m_pChannelEffects2D);
	}
}

void MainMenuScene::Draw()
{
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_ButtonTexts[0]), m_ButtonTextPositions[0], m_TextColor);
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_ButtonTexts[1]), m_ButtonTextPositions[1], m_TextColor);
}

void MainMenuScene::OnSceneActivated()
{
	SoundManager::Get()->GetSystem()->playSound(m_pMusicSound, nullptr, false, &m_pChannelBackground2D);
	m_pChannelBackground2D->setVolume(0.2f);
}

void MainMenuScene::OnSceneDeactivated()
{
	m_pChannelBackground2D->stop();
}
