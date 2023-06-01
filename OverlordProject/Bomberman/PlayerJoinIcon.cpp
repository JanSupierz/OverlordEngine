#include "stdafx.h"
#include "PlayerJoinIcon.h"

PlayerJoinIcon::PlayerJoinIcon(const std::wstring& spritePath, SpriteFont* pFont)
	:m_pFont{ pFont }
{
	AddComponent(new SpriteComponent(spritePath, { 0.5f,0.5f }, { 1.f,1.f,1.f,1.f }));
}

PlayerJoinIcon::~PlayerJoinIcon()
{
}

void PlayerJoinIcon::Initialize(const SceneContext&)
{
}

void PlayerJoinIcon::Update(const SceneContext&)
{

}

void PlayerJoinIcon::Draw(const SceneContext&)
{
	if (!IsFree())
	{
		TextRenderer::Get()->DrawText(m_pFont, StringUtil::utf8_decode(m_Text), m_TextPosition, m_TextColor);
	}
}

void PlayerJoinIcon::InitPosition()
{
	XMFLOAT3 position{ GetTransform()->GetPosition() };

	m_TextPosition.x += position.x;
	m_TextPosition.y += position.y;
}

void PlayerJoinIcon::SetPlayer(int index, const std::string& text)
{
	m_PlayerIndex = index;
	m_Text = text;
}

PlayerJoinIcon* PlayerJoinIcon::GetNext() const
{
	return m_pNext;
}

PlayerJoinIcon* PlayerJoinIcon::GetPrevious() const
{
	return m_pPrevious;
}

void PlayerJoinIcon::SetNext(PlayerJoinIcon* pNext)
{
	m_pNext = pNext;
	m_pNext->SetPrevious(this);
}

void PlayerJoinIcon::SetPrevious(PlayerJoinIcon* pPrevious)
{
	m_pPrevious = pPrevious;
}
