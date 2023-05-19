#pragma once
//Resharper Disable All

class PostChromaticAberration : public PostProcessingMaterial
{
public:
	PostChromaticAberration();
	~PostChromaticAberration() override = default;
	PostChromaticAberration(const PostChromaticAberration& other) = delete;
	PostChromaticAberration(PostChromaticAberration&& other) noexcept = delete;
	PostChromaticAberration& operator=(const PostChromaticAberration& other) = delete;
	PostChromaticAberration& operator=(PostChromaticAberration&& other) noexcept = delete;

protected:
	void Initialize(const GameContext& /*gameContext*/) override {}
};

