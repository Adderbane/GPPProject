
cbuffer Data : register(b0)
{
	
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
	// Some standard average stuff
	float4 finalColor = float4(0,0,0,0);

	finalColor += BasePixels.Sample(Sampler, input.uv);

	// Average color
	return finalColor;
}