
cbuffer Data : register(b0)
{
	float clipValue;
}

// Defines the input to this pixel shader
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float2 uv           : TEXCOORD0;
};

// Textures and such
Texture2D BasePixels	: register(t0);
SamplerState Sampler	: register(s0);

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 baseColor = BasePixels.Sample(Sampler, input.uv);
	
	float brightness = baseColor.r * 0.3f + baseColor.g * 0.59f + baseColor.b * 0.11f;

	clip(brightness - clipValue);

	// Average color
	return baseColor;
}