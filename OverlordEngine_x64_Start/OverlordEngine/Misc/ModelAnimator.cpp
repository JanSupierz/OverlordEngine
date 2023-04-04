#include "stdafx.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator(MeshFilter* pMeshFilter):
	m_pMeshFilter{pMeshFilter}
{
	SetAnimation(0);
}

void ModelAnimator::Update(const SceneContext& sceneContext)
{
	//We only update the transforms if the animation is running and the clip is set
	if (m_IsPlaying && m_ClipSet)
	{
		//1. 
		//Calculate the passedTicks (see the lab document)
		auto passedTicks = sceneContext.pGameTime->GetElapsed() * m_CurrentClip.ticksPerSecond * m_AnimationSpeed;

		//Make sure that the passedTicks stay between the m_CurrentClip.Duration bounds (fmod)
		if (passedTicks > m_CurrentClip.duration)
		{
			passedTicks = fmodf(passedTicks, m_CurrentClip.duration);
		}

		//2. 
		//IF m_Reversed is true
		if (m_Reversed)
		{
			//Subtract passedTicks from m_TickCount
			m_TickCount -= passedTicks;

			//If m_TickCount is smaller than zero
			if (m_TickCount < 0)
			{
				//Add m_CurrentClip.Duration to m_TickCount
				m_TickCount += m_CurrentClip.duration;
			}
		}
		else
		{
			//Add passedTicks to m_TickCount
			m_TickCount += passedTicks;

			//If m_TickCount is bigger than the clip duration
			if (m_TickCount > m_CurrentClip.duration)
			{
				//Subtract the duration from m_TickCount
				m_TickCount -= m_CurrentClip.duration;
			}
		}

		//3.
		//Find the enclosing keys
		AnimationKey keyA{}, keyB{};

		//Iterate all the keys of the clip and find the following keys:
		auto pKeyB{ std::find_if(m_CurrentClip.keys.begin(), m_CurrentClip.keys.end(), [&](const AnimationKey& key) {return key.tick > m_TickCount; }) };

		if (pKeyB != m_CurrentClip.keys.end())
		{
			//keyA > Closest Key with Tick before/smaller than m_TickCount
			keyA = *(pKeyB - 1);

			//keyB > Closest Key with Tick after/bigger than m_TickCount
			keyB = *pKeyB;
		}

		//4.
		//Interpolate between keys
		//Figure out the BlendFactor (See lab document)
		const float blendFactor{ ((m_TickCount - keyA.tick) / (keyB.tick - keyA.tick)) };

		//Clear the m_Transforms vector
		m_Transforms.clear();

		//FOR every boneTransform in a key (So for every bone)
		for (int index{}; index < m_pMeshFilter->GetBoneCount(); ++index)
		{
			//Retrieve the transform from keyA (transformA)
			const auto& transformA{ keyA.boneTransforms[index] };

			//Retrieve the transform from keyB (transformB)
			const auto& transformB{ keyB.boneTransforms[index] };

			//Decompose both transforms
			XMVECTOR scaleA{}, scaleB{};
			XMVECTOR rotationA, rotationB{};
			XMVECTOR positionA{}, positionB{};

			XMMatrixDecompose(&scaleA, &rotationA, &positionA, XMLoadFloat4x4(&transformA));
			XMMatrixDecompose(&scaleB, &rotationB, &positionB, XMLoadFloat4x4(&transformB));

			//Lerp between all the transformations (Position, Scale, Rotation)
			const XMVECTOR scaleLerp{ XMVectorLerp(scaleA, scaleB, blendFactor) };
			const XMVECTOR rotationSlerp{ XMQuaternionSlerp(rotationA, rotationB, blendFactor) };
			const XMVECTOR translationLerp{ XMVectorLerp(positionA, positionB, blendFactor) };

			//Compose a transformation matrix with the lerp-results
			const XMMATRIX transform{ XMMatrixScalingFromVector(scaleLerp) * XMMatrixRotationQuaternion(rotationSlerp) * XMMatrixTranslationFromVector(translationLerp) };

			//Add the resulting matrix to the m_Transforms vector
			m_Transforms.emplace_back();
			XMStoreFloat4x4(&m_Transforms[index], transform);
		}
	}
}

void ModelAnimator::SetAnimation(const std::wstring& clipName)
{
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Iterate the m_AnimationClips vector and search for an AnimationClip with the given name (clipName)

	auto pClip{ std::find_if(m_pMeshFilter->GetAnimationClips().begin(), m_pMeshFilter->GetAnimationClips().end(), [&](const AnimationClip& clip) {return clip.name == clipName; })};

	//If found,
	if(pClip != m_pMeshFilter->m_AnimationClips.end())
	{ 
		//Call SetAnimation(Animation Clip) with the found clip
		SetAnimation(*pClip);
	}
	else
	{
		//Call Reset
		Reset();

		//Log a warning with an appropriate message
		Logger::LogWarning(L"Animation clip with with name: " + clipName + L" not found!");
	}


}

void ModelAnimator::SetAnimation(UINT clipNumber)
{
	//Set m_ClipSet to false
	m_ClipSet = false;

	//Check if clipNumber is smaller than the actual m_AnimationClips vector size
	if (clipNumber < m_pMeshFilter->GetAnimationClips().size() && clipNumber >= 0)
	{
		//Retrieve the AnimationClip from the m_AnimationClips vector based on the given clipNumber
		//Call SetAnimation(AnimationClip clip)
		SetAnimation(m_pMeshFilter->GetAnimationClips()[clipNumber]);
	}
	else
	{
		//Call Reset
		Reset();

		//Log a warning with an appropriate message
		Logger::LogWarning(L"Animation clip with with name: " + std::to_wstring(clipNumber) + L" not found!");
	}
}

void ModelAnimator::SetAnimation(const AnimationClip& clip)
{
	//Set m_ClipSet to true
	m_ClipSet = true;
	
	//Set m_CurrentClip
	m_CurrentClip = clip;

	//Call Reset(false)
	Reset(false);
}

void ModelAnimator::Reset(bool pause)
{
	//If pause is true, set m_IsPlaying to false
	if (pause)
	{
		m_IsPlaying = false;
	}

	//Set m_TickCount to zero
	m_TickCount = 0.f;

	//Set m_AnimationSpeed to 1.0f
	m_AnimationSpeed = 1.f;

	//If m_ClipSet is true
	if (m_ClipSet)
	{
		//Retrieve the BoneTransform from the first Key from the current clip (m_CurrentClip)
		//Refill the m_Transforms vector with the new BoneTransforms (have a look at vector::assign)
		m_Transforms = m_CurrentClip.keys[0].boneTransforms;
	}
	else
	{
		//Create an IdentityMatrix 
		XMFLOAT4X4 identity{};

		memset(&identity, 0, sizeof(XMFLOAT4X4));
		identity._11 = 1.0f;
		identity._22 = 1.0f;
		identity._33 = 1.0f;
		identity._44 = 1.0f;

		//Refill the m_Transforms vector with this IdenityMatrix (Amount = BoneCount) (have a look at vector::assign)
		m_Transforms = std::vector<XMFLOAT4X4>(m_pMeshFilter->GetBoneCount(), identity);
	}


}
