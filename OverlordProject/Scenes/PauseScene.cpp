#include "stdafx.h"
#include "PauseScene.h"
#include "BombermanScene.h"
#include "Bomberman/PlayerJoinIcon.h"

#include "Materials/Shadow/DiffuseMaterial_Shadow.h"
#include "Materials/DiffuseMaterial.h"
#include "Materials/DiffuseMaterial_Skinned.h"
#include "Materials/Shadow/ColorMaterial_Shadow_Skinned.h"
#include "Materials/Shadow/DiffuseMaterial_Shadow_Skinned.h"
#include "Materials/Shadow/ColorMaterial_Shadow.h"

PauseScene::PauseScene()
	:GameScene(L"PauseScene")
{
}

PauseScene::~PauseScene()
{
}

void PauseScene::Initialize()
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

	//Button Resume
	m_pFont = ContentManager::Load<SpriteFont>(L"SpriteFonts/Consolas_32.fnt");

	auto pButtonResume{ AddChild(new GameObject()) };
	pSprite = pButtonResume->AddComponent(new SpriteComponent(L"Textures/Menu/ButtonSelected.png", { 0.5f,0.f }, { 1.f,1.f,1.f,1.f }));
	pButtonResume->GetTransform()->Scale(0.5f);

	m_pSprites.emplace_back(pSprite);

	//Button Restart
	auto pButtonRestart{ AddChild(new GameObject()) };
	pSprite = pButtonRestart->AddComponent(new SpriteComponent(L"Textures/Menu/Button.png", { 0.5f,0.0f }, { 1.f,1.f,1.f,1.f }));
	pButtonRestart->GetTransform()->Scale(0.5f);

	m_pSprites.emplace_back(pSprite);

	//Button Quit
	auto pButtonQuit{ AddChild(new GameObject()) };
	pSprite = pButtonQuit->AddComponent(new SpriteComponent(L"Textures/Menu/Button.png", { 0.5f,0.0f }, { 1.f,1.f,1.f,1.f }));
	pButtonQuit->GetTransform()->Scale(0.5f);

	m_pSprites.emplace_back(pSprite);

	m_ButtonTexts.emplace_back("Resume");
	pButtonResume->GetTransform()->Translate(350.f, 500.f, 0.f);
	m_ButtonTextPositions.emplace_back(XMFLOAT2{ 305.f,505.f });

	m_ButtonTexts.emplace_back("Restart");
	pButtonRestart->GetTransform()->Translate(650.f, 500.f, 0.f);
	m_ButtonTextPositions.emplace_back(XMFLOAT2{ 600.f,505.f });

	m_ButtonTexts.emplace_back("Quit");
	pButtonQuit->GetTransform()->Translate(950.f, 500.f, 0.f);
	m_ButtonTextPositions.emplace_back(XMFLOAT2{ 920.f,505.f });


	//Input
	auto inputAction{ InputAction(Start, InputState::pressed, -1, -1, XINPUT_GAMEPAD_A) };
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Previous, InputState::pressed, -1, -1, XINPUT_GAMEPAD_DPAD_LEFT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	inputAction = InputAction(Next, InputState::pressed, -1, -1, XINPUT_GAMEPAD_DPAD_RIGHT);
	m_SceneContext.pInput->AddInputAction(inputAction);

	//Sound 2D
	const auto pFmod = SoundManager::Get()->GetSystem();

	pFmod->createStream("Resources/Sounds/Menu.mp3", FMOD_2D | FMOD_LOOP_NORMAL, nullptr, &m_pMusicSound);
	pFmod->createStream("Resources/Sounds/AUD_Click.wav", FMOD_2D | FMOD_LOOP_OFF, nullptr, &m_pClickSound);
	pFmod->createStream("Resources/Sounds/AUD_Select.wav", FMOD_2D | FMOD_LOOP_OFF, nullptr, &m_pSelectSound);
}

void PauseScene::Update()
{
	if (m_SceneContext.pInput->IsActionTriggered(Start))
	{
		switch (m_CurrentButton)
		{
		case Button::resume:
			SceneManager::Get()->SetActiveGameScene(L"BombermanScene");
			break;
		case Button::restart:
			SceneManager::Get()->RemoveGameScene(BombermanScene::GetCurrent(), true);
			SceneManager::Get()->SetActiveGameScene(L"JoinMenuScene");
			break;
		case Button::quit:
			SceneManager::Get()->Quit();
			break;
		}

		SoundManager::Get()->GetSystem()->playSound(m_pClickSound, nullptr, false, &m_pChannelEffects2D);
	}
	else if (m_SceneContext.pInput->IsActionTriggered(Next))
	{
		switch (m_CurrentButton)
		{
		case Button::resume:
			m_pSprites[0]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[1]->SetTexture(L"Textures/Menu/ButtonSelected.png");
			m_pSprites[2]->SetTexture(L"Textures/Menu/Button.png");

			m_CurrentButton = Button::restart;
			break;
		case Button::restart:
			m_pSprites[0]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[1]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[2]->SetTexture(L"Textures/Menu/ButtonSelected.png");

			m_CurrentButton = Button::quit;
			break;
		case Button::quit:
			m_pSprites[0]->SetTexture(L"Textures/Menu/ButtonSelected.png");
			m_pSprites[1]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[2]->SetTexture(L"Textures/Menu/Button.png");

			m_CurrentButton = Button::resume;
			break;
		}

		SoundManager::Get()->GetSystem()->playSound(m_pSelectSound, nullptr, false, &m_pChannelEffects2D);
	}
	else if (m_SceneContext.pInput->IsActionTriggered(Previous))
	{
		switch (m_CurrentButton)
		{
		case Button::resume:
			m_pSprites[0]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[1]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[2]->SetTexture(L"Textures/Menu/ButtonSelected.png");

			m_CurrentButton = Button::quit;
			break;
		case Button::restart:
			m_pSprites[0]->SetTexture(L"Textures/Menu/ButtonSelected.png");
			m_pSprites[1]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[2]->SetTexture(L"Textures/Menu/Button.png");

			m_CurrentButton = Button::resume;
			break;
		case Button::quit:
			m_pSprites[0]->SetTexture(L"Textures/Menu/Button.png");
			m_pSprites[1]->SetTexture(L"Textures/Menu/ButtonSelected.png");
			m_pSprites[2]->SetTexture(L"Textures/Menu/Button.png");

			m_CurrentButton = Button::restart;
			break;
		}

		SoundManager::Get()->GetSystem()->playSound(m_pSelectSound, nullptr, false, &m_pChannelEffects2D);
	}
}

void PauseScene::Draw()
{
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_ButtonTexts[0]), m_ButtonTextPositions[0], m_TextColor);
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_ButtonTexts[1]), m_ButtonTextPositions[1], m_TextColor);
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_ButtonTexts[2]), m_ButtonTextPositions[2], m_TextColor);
}

void PauseScene::OnSceneActivated()
{
	SoundManager::Get()->GetSystem()->playSound(m_pMusicSound, nullptr, false, &m_pChannelBackground2D);
	m_pChannelBackground2D->setVolume(0.2f);
}

void PauseScene::OnSceneDeactivated()
{
	m_pChannelBackground2D->stop();
}
