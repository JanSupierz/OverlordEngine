#include "stdafx.h"
#include "SpriteComponent.h"

SpriteComponent::SpriteComponent(const std::wstring& spriteAsset, const XMFLOAT2& pivot, const XMFLOAT4& color):
	m_SpriteAsset(spriteAsset),
	m_Pivot(pivot),
	m_Color(color)
{}

void SpriteComponent::Initialize(const SceneContext& /*sceneContext*/)
{
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::SetTexture(const std::wstring& spriteAsset)
{
	m_SpriteAsset = spriteAsset;
	m_pTexture = ContentManager::Load<TextureData>(m_SpriteAsset);
}

void SpriteComponent::Draw(const SceneContext& sceneContext)
{
	if (!m_pTexture) return;

	const TransformComponent* pTransform{ GetTransform() };

	const XMFLOAT2& position{ pTransform->GetWorldPosition().x,pTransform->GetWorldPosition().y };
	const XMFLOAT2& scale{ pTransform->GetWorldScale().x,pTransform->GetWorldScale().y };
	const XMFLOAT3& rotation{ MathHelper::QuaternionToEuler(pTransform->GetWorldRotation()) };

	SpriteRenderer::Get()->AppendSprite(m_pTexture, position, m_Color, m_Pivot, scale, rotation.z, pTransform->GetPosition().z);
	SpriteRenderer::Get()->Draw(sceneContext, m_MoveDirection);
}
