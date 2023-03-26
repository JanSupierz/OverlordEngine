#include "stdafx.h"
#include "UberMaterialScene.h"
#include "Materials/UberMaterial.h"

void UberMaterialScene::Initialize()
{
	//ImGui
	m_SceneContext.settings.enableOnGUI = true;
	m_SceneContext.settings.drawGrid = false;

	//--- Chair ---
	m_pObject = new GameObject();

	//Model
	const auto pSphereModel{ new ModelComponent(L"Meshes/Sphere.ovm") };

	m_pUberMaterial = MaterialManager::Get()->CreateMaterial<UberMaterial>();
	pSphereModel->SetMaterial(m_pUberMaterial);

	m_pObject->AddComponent(pSphereModel);

	constexpr float scale{ 25.f };
	m_pObject->GetTransform()->Scale(XMFLOAT3{ scale,scale,scale });
	m_pObject->GetTransform()->Translate(XMFLOAT3{ 0.f,8.f,0.f });
	AddChild(m_pObject);
}

void UberMaterialScene::Update()
{
	constexpr float rotationSpeed{ 20.f };

	m_pObject->GetTransform()->Rotate(0.f, m_Angle, 0.f);
	m_Angle += m_SceneContext.pGameTime->GetElapsed() * rotationSpeed;
}

void UberMaterialScene::OnGUI()
{
	m_pUberMaterial->DrawImGui();
}
