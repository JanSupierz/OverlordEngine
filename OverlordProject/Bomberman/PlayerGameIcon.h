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

	virtual void Draw(const SceneContext&) override;
	void InitPosition();

	void AddStar();
	std::string GetName() const;

	void SetPowerUpText(const std::string& text) { m_PowerUpText = text; }
private:
	SpriteFont* m_pFont{};

	XMFLOAT4 m_TextColor{ 1.f,1.f,1.f,1.f };
	XMFLOAT2 m_TextPosition{ -50.f, 30.f };
	std::string m_Text{};

	XMFLOAT4 m_PowerUpTextColor{ 1.f,1.f,1.f,1.f };
	XMFLOAT2 m_PowerUpTextPosition{};
	std::string m_PowerUpText{};

	float m_StarOffset{};
	const bool m_Flip{};
};
