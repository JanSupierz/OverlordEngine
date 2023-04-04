#include "stdafx.h"
#include "SpikyScene.h"
#include <Materials/SpikyMaterial.h>

void SpikyScene::Initialize()
{
	//ImGui
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	m_pObject = new GameObject{};

	//Model
	const auto pSphereModel{ new ModelComponent(L"Meshes/OctaSphere.ovm") };

	m_pMaterial = MaterialManager::Get()->CreateMaterial<SpikyMaterial>();

	m_pMaterial->SetColor(m_Color);
	m_pMaterial->SetSpikeLength(m_SpikeLength);

	pSphereModel->SetMaterial(m_pMaterial);

	m_pObject->AddComponent(pSphereModel);

	constexpr float scale{ 15.f };
	m_pObject->GetTransform()->Scale(XMFLOAT3{ scale,scale,scale });

	AddChild(m_pObject);
}

void SpikyScene::Update()
{
	constexpr float rotationSpeed{ 20.f };

	m_pObject->GetTransform()->Rotate(0.f, m_Angle, 0.f);
	m_Angle += m_SceneContext.pGameTime->GetElapsed() * rotationSpeed;
}

void SpikyScene::OnGUI()
{
	ImGui::ColorEdit4("Color", reinterpret_cast<float*>(&m_Color));
	ImGui::SliderFloat("Spike Length", &m_SpikeLength, 0.f, 1.f);

	m_pMaterial->SetColor(m_Color);
	m_pMaterial->SetSpikeLength(m_SpikeLength);
}