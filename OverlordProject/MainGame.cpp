#include "stdafx.h"
#include "MainGame.h"

#define Bomberman

#pragma region Lab/Milestone Includes

#ifdef Bomberman
#include "Scenes/MainMenuScene.h"
#include "Scenes/JoinMenuScene.h"
#include "Scenes/BombermanScene.h"
#include "Scenes/PauseScene.h"
#include "Scenes/EndScene.h"
#endif

#pragma endregion

//Game is preparing
void MainGame::OnGamePreparing(GameContext& gameContext)
{
	//Here you can change some game settings before engine initialize
	gameContext.windowTitle = L"GP2 - Exam Project (2023) | (2DAE15) Supierz Jan";
}

void MainGame::Initialize()
{

#ifdef Bomberman
	SceneManager::Get()->AddGameScene(new MainMenuScene());
	SceneManager::Get()->AddGameScene(new JoinMenuScene());
	SceneManager::Get()->AddGameScene(new BombermanScene());
	SceneManager::Get()->AddGameScene(new PauseScene());
	SceneManager::Get()->AddGameScene(new EndScene());
#endif

}

LRESULT MainGame::WindowProcedureHook(HWND /*hWnd*/, UINT message, WPARAM wParam, LPARAM lParam)
{

	if(message == WM_KEYUP)
	{
		if ((lParam & 0x80000000) != 0x80000000)
			return -1;

		//[F1] Toggle Scene Info Overlay
		if(wParam == VK_F1)
		{
			const auto pScene = SceneManager::Get()->GetActiveScene();
			pScene->GetSceneSettings().Toggle_ShowInfoOverlay();
		}

		//[F2] Toggle Debug Renderer (Global)
		if (wParam == VK_F2)
		{
			DebugRenderer::ToggleDebugRenderer();
			return 0;

		}

		//[F3] Previous Scene
		if (wParam == VK_F3)
		{
			SceneManager::Get()->PreviousScene();
			return 0;

		}

		//[F4] Next Scene
		if (wParam == VK_F4)
		{
			SceneManager::Get()->NextScene();
			return 0;
		}

		//[F5] If PhysX Framestepping is enables > Next Frame	
		if (wParam == VK_F6)
		{
			const auto pScene = SceneManager::Get()->GetActiveScene();
			pScene->GetPhysxProxy()->NextPhysXFrame();
		}
	}
	

	return -1;
}
