#pragma once

class EndScene final : public GameScene
{
public:
	EndScene();
	~EndScene() override;

	EndScene(const EndScene& other) = delete;
	EndScene(EndScene&& other) noexcept = delete;
	EndScene& operator=(const EndScene& other) = delete;
	EndScene& operator=(EndScene&& other) noexcept = delete;

protected:
	void Initialize() override;
	void Update() override;
	void Draw() override;

	virtual void OnSceneActivated() override;
	virtual void OnSceneDeactivated() override;

private:
	enum InputIds
	{
		Start, Next, Previous
	};

	enum class Button
	{
		start, quit
	};

	Button m_CurrentButton{ Button::start };

	FixedCamera* m_pFixedCamera{};

	SpriteFont* m_pFont{};
	XMFLOAT4 m_TextColor{ 0.f,0.f,0.f,1.f };

	FMOD::Sound* m_pMusicSound{};
	FMOD::Sound* m_pSelectSound{};
	FMOD::Sound* m_pClickSound{};

	FMOD::Channel* m_pChannelBackground2D{ nullptr };
	FMOD::Channel* m_pChannelEffects2D{ nullptr };

	std::vector<SpriteComponent*> m_pSprites{};
	std::vector<std::string> m_ButtonTexts{};
	std::vector<XMFLOAT2> m_ButtonTextPositions{};
};


