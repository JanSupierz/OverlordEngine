#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	delete[] m_ParticlesArray;
	m_pVertexBuffer->Release();
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//It is static, so check if already exists
	if (!m_pParticleMaterial)
	{
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	}

	CreateVertexBuffer(sceneContext);
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	//Release if already exists
	SafeRelease(m_pVertexBuffer);

	//Create vertex buffer
	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = m_ParticleCount * sizeof(VertexParticle);
	vertexBufferDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	//Check errors
	HANDLE_ERROR(sceneContext.d3dContext.pDevice->CreateBuffer(&vertexBufferDesc, nullptr, &m_pVertexBuffer));
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//Calculate particle interval
	const float averageEnergy{ (m_EmitterSettings.minEnergy + m_EmitterSettings.maxEnergy) * 0.5f };
	const float particleInterval{ averageEnergy / m_ParticleCount };

	//Update last spawn time
	const float elapsedSec{ sceneContext.pGameTime->GetElapsed() };
	m_LastParticleSpawn += elapsedSec;

	//Use DeviceContext::Map to map our vertexbuffer
	D3D11_MAPPED_SUBRESOURCE resource{};
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);

	VertexParticle* pBuffer{ reinterpret_cast<VertexParticle*>(resource.pData) };

	//Iterate the Particle Array(You only want to iterate ‘m_ParticleCount’ number of times)
	m_ActiveParticles = 0;
	for (UINT index{}; index < m_ParticleCount; ++index)
	{
		Particle& currentParticle{ m_ParticlesArray[index] };

		if (currentParticle.isActive)
		{
			UpdateParticle(currentParticle, elapsedSec);
		}

		//Spawn if enough time elapsed since last spawn
		//Respawn immediately, if the current particle became inactive 
		if (!currentParticle.isActive && m_LastParticleSpawn >= particleInterval)
		{
			SpawnParticle(currentParticle);
			m_LastParticleSpawn -= particleInterval;
		}

		//Add to the buffer, if active
		if (currentParticle.isActive)
		{
			pBuffer[m_ActiveParticles] = currentParticle.vertexInfo;
			++m_ActiveParticles;
		}
	}

	//Unmap the vertex buffer
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	if (!p.isActive) return;

	//Update energy
	p.currentEnergy -= elapsedTime;

	if (p.currentEnergy < 0)
	{
		p.isActive = false;
		return;
	}

	//Update position
	p.vertexInfo.Position.x += m_EmitterSettings.velocity.x * elapsedTime;
	p.vertexInfo.Position.y += m_EmitterSettings.velocity.y * elapsedTime;
	p.vertexInfo.Position.z += m_EmitterSettings.velocity.z * elapsedTime;

	//Update color
	const float lifePercent{ p.currentEnergy / p.totalEnergy };
	constexpr float delayFadeOut{ 2.f };

	p.vertexInfo.Color = m_EmitterSettings.color;
	p.vertexInfo.Color.w *= lifePercent * delayFadeOut;

	//Update size
	const float initialSize{ p.initialSize };
	p.vertexInfo.Size = initialSize + initialSize * (p.sizeChange - 1.f) * (1.f - lifePercent);
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	p.isActive = true;

	//Energy
	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;

	//Position --- XMVector3TransformNormal is ambiguous
	XMVECTOR randomDirection
	{ 
		MathHelper::randF(-1.f, 1.f),
		MathHelper::randF(-1.f, 1.f),
		MathHelper::randF(-1.f, 1.f)
	};

	const XMVECTOR& offset{ XMVector3Normalize(randomDirection) * MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius) };
	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&GetTransform()->GetWorldPosition()) + offset);

	//Size
	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = p.vertexInfo.Size;

	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	//Rotation
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	//Color
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//Set variables
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection() );
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture->GetShaderResourceView());

	//Get contexts
	const auto& techniqueContext{ m_pParticleMaterial->GetTechniqueContext() };
	ID3D11DeviceContext* pDeviceContext{ sceneContext.d3dContext.pDeviceContext };

	//Input layout
	pDeviceContext->IASetInputLayout(techniqueContext.pInputLayout);

	//Topology
	pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//Vertex buffer
	constexpr UINT offset{}, vertexSize{ sizeof(VertexParticle) };
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &vertexSize, &offset);

	//Apply the pass
	D3DX11_TECHNIQUE_DESC techniqueDesc{};
	techniqueContext.pTechnique->GetDesc(&techniqueDesc);

	for (uint32_t pass{}; pass < techniqueDesc.Passes; ++pass)
	{
		techniqueContext.pTechnique->GetPassByIndex(pass)->Apply(0, pDeviceContext);
		pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}