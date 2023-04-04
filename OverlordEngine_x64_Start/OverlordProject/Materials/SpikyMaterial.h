#pragma once
class SpikyMaterial final : public Material<SpikyMaterial>
{
public:
	SpikyMaterial();
	~SpikyMaterial() override = default;

	SpikyMaterial(const SpikyMaterial& other) = delete;
	SpikyMaterial(SpikyMaterial&& other) noexcept = delete;
	SpikyMaterial& operator=(const SpikyMaterial& other) = delete;
	SpikyMaterial& operator=(SpikyMaterial&& other) noexcept = delete;

	void SetColor(const XMFLOAT4& color) const;
	void SetSpikeLength(float length) const;

protected:
	void InitializeEffectVariables() override;
};

