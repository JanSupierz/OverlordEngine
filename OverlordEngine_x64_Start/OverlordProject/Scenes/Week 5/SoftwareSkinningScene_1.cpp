#include "stdafx.h"
#include "SoftwareSkinningScene_1.h"
#include "Prefabs/BoneObject.h"
#include "Materials/ColorMaterial.h"

void SoftwareSkinningScene_1::Initialize()
{
	//Create a ColorMaterial that will be used for both BoneObjects
	ColorMaterial* pMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };

	//Create a new GameObject which will act as the root for our Bone Hierarchy 
	GameObject* pRoot{ new GameObject() };

	//Initialize the first bone, use the premade ColorMaterial and a length of 15.f.
	m_pBone0 = new BoneObject(pMaterial, 15.f);

	//This bone is then added to the pRoot as a child.
	pRoot->AddChild(m_pBone0);

	//Initialize the second bone, same parameters as m_pBone0.
	m_pBone1 = new BoneObject(pMaterial, 15.f);

	//This bone is added as a child to m_pBone0 BUT use the AddBone function instead of the AddChild function!
	//(The AddBone function will add this bone as a child and translate it to the end of the parent bone)
	m_pBone0->AddBone(m_pBone1);

	//Make sure to add pRoot to the scenegraph using AddChild
	AddChild(pRoot);

	m_SceneContext.settings.enableOnGUI = true;
}

void SoftwareSkinningScene_1::Update()
{
	if (m_AutoRotate)
	{
		constexpr float rotationSpeed{ 45.f };

		if (m_BoneRotation >= 45.f)
		{
			m_RotationSign = -1;
		}
		else if (m_BoneRotation <= -45.f)
		{
			m_RotationSign = 1;
		}

		m_BoneRotation += static_cast<float>(m_RotationSign) * rotationSpeed * m_SceneContext.pGameTime->GetElapsed();

		m_RotationBone0 = { 0.f,0.f,m_BoneRotation };
		m_RotationBone1 = { 0.f,0.f, -m_BoneRotation * 2.f };
	}

	m_pBone0->GetTransform()->Rotate(m_RotationBone0);
	m_pBone1->GetTransform()->Rotate(m_RotationBone1);
}

void SoftwareSkinningScene_1::OnGUI()
{
	ImGui::BeginDisabled(m_AutoRotate);
	ImGui::DragFloat3("Bone 0 - ROT", reinterpret_cast<float*>(&m_RotationBone0));
	ImGui::DragFloat3("Bone 1 - ROT", reinterpret_cast<float*>(&m_RotationBone1));
	ImGui::EndDisabled();

	ImGui::Checkbox("Auto Rotate", &m_AutoRotate);
}