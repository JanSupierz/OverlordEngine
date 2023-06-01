#pragma once
class PlayerGameIcon final : public GameObject
{
public:
	PlayerGameIcon(const std::wstring& spritePath, SpriteFont* pFont, float spritePivotX, bool flip, const std::string& text);
	virtual ~PlayerGameIcon();

	PlayerGameIcon(const PlayerGameIcon& other) = delete;
	PlayerGameIcon(PlayerGameIcon&& other) noexcept = delete;
	PlayerGameIcon& operator=(const PlayerGameIcon& other) = delete;
	PlayerGameIcon& operator=(PlayerGameIcon&& other) noexcept = delete;

	virtual void Initialize(const SceneContext&) override;
	virtual void Update(const SceneContext& sceneContext) override;
	virtual void Draw(const SceneContext&) override;
	void InitPosition();
	int GetPlayerIndex() const { return m_PlayerIndex; }

	void AddStar();
private:
	SpriteFont* m_pFont{};
	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };
	XMFLOAT2 m_TextPosition{ -50.f, 30.f };
	std::string m_Text{};

	float m_StarOffset{};
	int m_PlayerIndex{ -1 };

	bool m_Flip{};
};
