#include "stdafx.h"
#include "PlayerGameIcon.h"
#include "Scenes/BombermanScene.h"

PlayerGameIcon::PlayerGameIcon(const std::wstring& spritePath, SpriteFont* pFont, float spritePivotX, bool flip, const std::string& text)
	:m_pFont{ pFont }, m_Text{ text }, m_Flip{ flip }
{
	AddComponent(new SpriteComponent(spritePath, { spritePivotX,0.5f }, { 1.f,1.f,1.f,1.f }));

	const auto pSprite = AddChild(new GameObject);
	m_TextPosition.y = 56.f;

	if (flip)
	{
		pSprite->AddComponent(new SpriteComponent(L"Textures/GameUI/Player_UI_Flipped.png", { spritePivotX,0.f }, { 1.f,1.f,1.f,1.f }));

		const float textOffset{ -46.f };
		m_TextPosition.x = textOffset;
		m_PowerUpTextPosition.x -= 190.f;
	}
	else
	{
		pSprite->AddComponent(new SpriteComponent(L"Textures/GameUI/Player_UI.png", { spritePivotX,0.f }, { 1.f,1.f,1.f,1.f }));

		const float textOffset{ 15.f };
		m_TextPosition.x = textOffset;
		m_PowerUpTextPosition.x += 90.f;
	}

	pSprite->GetTransform()->Translate(0.f, 120.f, 0.02f);
}

PlayerGameIcon::~PlayerGameIcon()
{
}

void PlayerGameIcon::Initialize(const SceneContext&)
{
}

void PlayerGameIcon::Update(const SceneContext&)
{

}

void PlayerGameIcon::Draw(const SceneContext&)
{
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Text), m_TextPosition, m_TextColor);
	TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_PowerUpText), m_PowerUpTextPosition, m_PowerUpTextColor);
}

void PlayerGameIcon::InitPosition()
{
	XMFLOAT3 position{ GetTransform()->GetPosition() };

	m_TextPosition.x += position.x;
	m_TextPosition.y += position.y;

	m_PowerUpTextPosition.x += position.x;
	m_PowerUpTextPosition.y += position.y;
}

void PlayerGameIcon::AddStar()
{
	const auto pSprite = AddChild(new GameObject);

	if (m_Flip)
	{
		pSprite->AddComponent(new SpriteComponent(L"Textures/GameUI/UI_Star.png", { 1.f,0.f }, { 1.f,1.f,1.f,1.f }));
		pSprite->GetTransform()->Translate(-170.f + m_StarOffset, 122.f, 0.02f);
		m_StarOffset -= 60.f;
	}
	else
	{
		pSprite->AddComponent(new SpriteComponent(L"Textures/GameUI/UI_Star.png", { 0.f,0.f }, { 1.f,1.f,1.f,1.f }));
		pSprite->GetTransform()->Translate(170.f + m_StarOffset, 122.f, 0.02f);

		m_StarOffset += 60.f;
	}

	pSprite->GetTransform()->Scale(0.2f);
}

std::string PlayerGameIcon::GetName() const
{
	return m_Text.substr(1);
}


