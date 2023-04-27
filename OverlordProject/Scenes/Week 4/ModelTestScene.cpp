#include "stdafx.h"
#include "ModelTestScene.h"
#include "Materials/DiffuseMaterial.h"

void ModelTestScene::Initialize()
{
	auto& physX = PxGetPhysics();
	auto pMaterial = physX.createMaterial(0.5f, 0.5f, 0.5f);
	
	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pMaterial);

	//ImGui
	m_SceneContext.settings.enableOnGUI = true;

	//--- Chair ---
	m_pChair = new GameObject();

	//Model
	const auto pChairModel{ new ModelComponent(L"Meshes/Chair.ovm") };

	m_pChairMaterial = MaterialManager::Get()->CreateMaterial<DiffuseMaterial>();
	m_pChairMaterial->SetDiffuseMaterial(L"Textures/Chair_Dark.dds");
	pChairModel->SetMaterial(m_pChairMaterial);
	
	m_pChair->AddComponent(pChairModel);

	//PhysX
	const auto geometry{ PxConvexMeshGeometry{ContentManager::Load<PxConvexMesh>(L"Meshes/Chair.ovpc")} };
	const auto pRigidBody = m_pChair->AddComponent(new RigidBodyComponent());
	pRigidBody->AddCollider(geometry, *pMaterial);

	m_pChair->GetTransform()->Translate(0.f, 5.f, 0.f);
	AddChild(m_pChair);
}

void ModelTestScene::Update()
{
}

void ModelTestScene::OnGUI()
{
	m_pChairMaterial->DrawImGui();
}
