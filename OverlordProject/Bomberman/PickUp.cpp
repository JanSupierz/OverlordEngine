#include "stdafx.h"
#include "Scenes/BombermanScene.h"
#include "PickUp.h"
#include "Grid.h"

std::unordered_map<PickUpType, BaseMaterial*> PickUp::s_pPickUpMaterials{};
PxMaterial* PickUp::s_pStaticMaterial{ nullptr };

PickUp::PickUp(Node* pNode, Grid* pGrid, PickUpType type)
	:m_Type{ type }, m_pGrid{ pGrid }, m_pNode{ pNode }
{
	SetTag(L"PickUp");
}

PickUp::~PickUp()
{
}

void PickUp::Initialize(const SceneContext&)
{
	const float cellSize{ m_pGrid->GetCellSize() };
	XMFLOAT2 nodePos{ m_pNode->GetWorldPos() };

	GetTransform()->Translate(nodePos.x, cellSize, nodePos.y);
	GetTransform()->Scale(cellSize * 0.5f);

	std::cout << "spawned" << cellSize << '\n';

	const auto pModelObject{ AddChild(new GameObject) };
	pModelObject->GetTransform()->Translate(0.f, -48.5f, 0.f);
	auto pModel{ pModelObject->AddComponent(new ModelComponent(L"Meshes/Bomberman/PickUp.ovm")) };
	pModel->SetMaterial(s_pPickUpMaterials[PickUpType::None]);
	pModel->SetMaterial(s_pPickUpMaterials[m_Type], 1);
	pModel->SetMaterial(s_pPickUpMaterials[PickUpType::None], 0);

	pModel->GetAnimator()->SetAnimation(0);
	pModel->GetAnimator()->Play();

	//Particle System
	ParticleEmitterSettings settings{};
	settings.velocity = { 0.f,2.f,0.f };
	settings.minSize = 1.f;
	settings.maxSize = 1.f;
	settings.minEnergy = 1.f;
	settings.maxEnergy = 5.f;
	settings.minScale = 0.5f;
	settings.maxScale = 3.f;
	settings.minEmitterRadius = 1.f;
	settings.maxEmitterRadius = 2.5f;
	settings.color = { 1.f,1.f,1.f, .6f };
	
	const auto pVFX = AddChild(new GameObject);
	pVFX->GetTransform()->Translate(0.2f, 1.1f, 0.1f);
	pVFX->AddComponent(new ParticleEmitterComponent(L"Textures/Bomberman/Star.png", settings, 10));
	
	auto pRigid{ AddComponent(new RigidBodyComponent(false)) };
	
	const auto geo{ PxSphereGeometry{cellSize * 0.5f} };
	pRigid->AddCollider(geo, *s_pStaticMaterial, true, PxTransform{ 0.f,cellSize * 0.5f,0.f });

	pRigid->SetConstraint(RigidBodyConstraint::All, false);
}

void PickUp::SetPhysicsMaterial(PxMaterial* pStaticMaterial)
{
	s_pStaticMaterial = pStaticMaterial;
}

void PickUp::SetPickUpMaterial(PickUpType type, BaseMaterial* pMaterial)
{
	s_pPickUpMaterials[type] = pMaterial;
}

PickUpType PickUp::GetType() const
{
	return m_Type;
}
