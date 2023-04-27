#include "stdafx.h"
#include "SpikyMaterial.h"

SpikyMaterial::SpikyMaterial() :
	Material<SpikyMaterial>(L"Effects/SpikyShader.fx")
{}

void SpikyMaterial::InitializeEffectVariables()
{
}

void SpikyMaterial::SetColor(const XMFLOAT4& color) const
{
	SetVariable_Vector(L"gColorDiffuse", color);
}

void SpikyMaterial::SetSpikeLength(float length) const
{
	SetVariable_Scalar(L"gSpikeLength", length);
}
