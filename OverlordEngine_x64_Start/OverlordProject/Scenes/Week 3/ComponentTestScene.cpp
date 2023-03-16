#include "stdafx.h"
#include "ComponentTestScene.h"

#include "Prefabs/CubePrefab.h"
#include "Prefabs/SpherePrefab.h"

#include "Prefabs/TorusPrefab.h"

ComponentTestScene::ComponentTestScene() :
	GameScene(L"ComponentTestScene") {}

void ComponentTestScene::Initialize()
{
	auto& physX = PxGetPhysics();
	auto pBouncyMaterial = physX.createMaterial(0.5f, 0.5f, 1.f);

	//Ground plane
	GameSceneExt::CreatePhysXGroundPlane(*this, pBouncyMaterial);

	//Sphere Red (Group 0)
	auto pSphere = new SpherePrefab(1, 10, XMFLOAT4(Colors::Red));
	AddChild(pSphere);

	pSphere->GetTransform()->Translate(0, 30.f, 0);
	auto pRigidBody = pSphere->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group0);
	//pRigidBody->SetCollisionIgnoreGroups(CollisionGroup::Group1 | CollisionGroup::Group2);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);

	//Sphere Green (Group 1)
	pSphere = new SpherePrefab(1, 10, XMFLOAT4(Colors::Green));
	AddChild(pSphere);

	pSphere->GetTransform()->Translate(0.0f, 20.f, 0);
	pRigidBody = pSphere->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group1);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);

	//Sphere Blue (Group 2)
	pSphere = new SpherePrefab(1, 10, XMFLOAT4(Colors::Blue));
	AddChild(pSphere);

	pSphere->GetTransform()->Translate(0, 10.f, 0);
	pRigidBody = pSphere->AddComponent(new RigidBodyComponent(false));
	pRigidBody->AddCollider(PxSphereGeometry{ 1.f }, *pBouncyMaterial);
	pRigidBody->SetCollisionGroup(CollisionGroup::Group2);
	pRigidBody->SetConstraint(RigidBodyConstraint::TransX | RigidBodyConstraint::TransZ, false);
}

void ComponentTestScene::Update()
{
}

void ComponentTestScene::Draw()
{
	//Optional
}

void ComponentTestScene::OnGUI()
{
	ImGui::Text("This only activates if\n SceneSettings.enableOnGUI is True.\n\n");
	ImGui::Text("Use ImGui to add custom\n controllable scene parameters!");
	ImGui::ColorEdit3("Demo ClearColor", &m_SceneContext.settings.clearColor.x, ImGuiColorEditFlags_NoInputs);
}
