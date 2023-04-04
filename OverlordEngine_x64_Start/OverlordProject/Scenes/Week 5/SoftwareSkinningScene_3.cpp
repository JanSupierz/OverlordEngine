#include "stdafx.h"
#include "SoftwareSkinningScene_3.h"
#include "Prefabs/BoneObject.h"
#include "Materials/ColorMaterial.h"

void SoftwareSkinningScene_3::Initialize()
{
	constexpr float length{ 15.f };

	//Create a ColorMaterial that will be used for both BoneObjects
	ColorMaterial* pMaterial{ MaterialManager::Get()->CreateMaterial<ColorMaterial>() };

	//Create a new GameObject which will act as the root for our Bone Hierarchy 
	GameObject* pRoot{ new GameObject() };

	//Initialize the first bone, use the premade ColorMaterial and a length of 15.f.
	m_pBone0 = new BoneObject(pMaterial, length);

	//This bone is then added to the pRoot as a child.
	pRoot->AddChild(m_pBone0);

	//Initialize the second bone, same parameters as m_pBone0.
	m_pBone1 = new BoneObject(pMaterial, length);

	//This bone is added as a child to m_pBone0 BUT use the AddBone function instead of the AddChild function!
	//(The AddBone function will add this bone as a child and translate it to the end of the parent bone)
	m_pBone0->AddBone(m_pBone1);

	//Make sure to add pRoot to the scenegraph using AddChild
	AddChild(pRoot);

	m_SceneContext.settings.enableOnGUI = true;

	//Part 2
	//First, we need to calculate our BindPose for Bone_0, this can easily be achieved by calling the CalculateBindPose on that bone
	m_pBone0->CalculateBindPose();

	//Second, add an empty GameObject to the scene
	GameObject* pBoxDrawer{ new GameObject() };
	AddChild(pBoxDrawer);

	//Add to this object a MeshDrawComponent and store the pointer in m_pMeshDrawer
	m_pMeshDrawer = pBoxDrawer->AddComponent(new MeshDrawComponent(24, true));
	//Finally, call the InitializeVertices function 
	InitializeVertices(length);
}

void SoftwareSkinningScene_3::Update()
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

	//Part 2
	//Remove current triangles from the MeshDrawer
	m_pMeshDrawer->RemoveTriangles();

	//Iterate the SkinnedVertices vector and append each set of 4 vertices as a Quad to the MeshDrawer
	QuadPosNormCol quad{};

	for (size_t index{ 0 }; index < m_SkinnedVertices.size(); index += 4)
	{
		quad.Vertex1 = m_SkinnedVertices[index].transformedVertex;
		quad.Vertex2 = m_SkinnedVertices[index + 1].transformedVertex;
		quad.Vertex3 = m_SkinnedVertices[index + 2].transformedVertex;
		quad.Vertex4 = m_SkinnedVertices[index + 3].transformedVertex;
		m_pMeshDrawer->AddQuad(quad);
	}

	//Calculate the boneTransforms for each bone.
	//This is the result of a multiplication between the BindPose of the bone and the WorldMatrix of the bone.
	const auto boneTransform0{ XMMatrixMultiply(XMLoadFloat4x4(&m_pBone0->GetBindPose()), XMLoadFloat4x4(&m_pBone0->GetTransform()->GetWorld())) };
	const auto boneTransform1{ XMMatrixMultiply(XMLoadFloat4x4(&m_pBone1->GetBindPose()), XMLoadFloat4x4(&m_pBone1->GetTransform()->GetWorld())) };

	//Iterate the vector of SkinnedVertices and transform the position of every originalVertex with the corresponding boneTransform.
	const unsigned int triangleCapacity{ m_pMeshDrawer->GetTriangleCapacity() };
	for (size_t index{ 0 }; index < m_SkinnedVertices.size(); ++index)
	{
		auto& vertex{ m_SkinnedVertices[index] };

		if (vertex.blendWeight > 0.f && vertex.blendWeight < 1.f)
		{
			const XMVECTOR transformedPosition0{ XMVector3TransformCoord(XMLoadFloat3(&vertex.originalVertex.Position), boneTransform0) };
			const XMVECTOR transformedPosition1{ XMVector3TransformCoord(XMLoadFloat3(&vertex.originalVertex.Position), boneTransform1) };

			//Store this transformed position in the transformedVertex
			XMStoreFloat3(&vertex.transformedVertex.Position, XMVectorLerp(transformedPosition1, transformedPosition0, vertex.blendWeight));
		}
		else
		{
			const XMVECTOR transformedPosition{ XMVector3TransformCoord(XMLoadFloat3(&vertex.originalVertex.Position), index < triangleCapacity ? boneTransform0 : boneTransform1) };

			//Store this transformed position in the transformedVertex
			XMStoreFloat3(&vertex.transformedVertex.Position, transformedPosition);
		}
	}

	//Update the buffer of the MeshDrawer after appending all Quads
	m_pMeshDrawer->UpdateBuffer();
}

void SoftwareSkinningScene_3::OnGUI()
{
	ImGui::BeginDisabled(m_AutoRotate);
	ImGui::DragFloat3("Bone 0 - ROT", reinterpret_cast<float*>(&m_RotationBone0));
	ImGui::DragFloat3("Bone 1 - ROT", reinterpret_cast<float*>(&m_RotationBone1));
	ImGui::EndDisabled();

	ImGui::Checkbox("Auto Rotate", &m_AutoRotate);
}

void SoftwareSkinningScene_3::InitializeVertices(float length)
{
	auto pos = XMFLOAT3(length / 2.f, 0.f, 0.f);
	const auto offset = XMFLOAT3(length / 2.f, 2.5f, 2.5f);
	auto col = XMFLOAT4(1.f, 0.f, 0.f, 0.5f);

#pragma region BOX 1

	//FRONT
	XMFLOAT3 norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);

	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);

	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);

	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);

	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 1.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 1.f);

	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);

#pragma endregion

	col = { 0.f, 1.f, 0.f, 0.5f };
	pos = { 22.5f, 0.f, 0.f };

#pragma region BOX 2

	//FRONT
	norm = { 0, 0, -1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);

	//BACK
	norm = { 0, 0, 1 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);

	//TOP
	norm = { 0, 1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);

	//BOTTOM
	norm = { 0, -1, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);

	//LEFT
	norm = { -1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.5f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ -offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.5f);

	//RIGHT
	norm = { 1, 0, 0 };
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, offset.z + pos.z }, norm, col, 0.f);
	m_SkinnedVertices.emplace_back(XMFLOAT3{ offset.x + pos.x, -offset.y + pos.y, -offset.z + pos.z }, norm, col, 0.f);

#pragma endregion
}