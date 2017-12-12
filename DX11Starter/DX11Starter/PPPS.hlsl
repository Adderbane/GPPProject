
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
Texture2D LightBloom    : register(t1);
Texture2D RadialBlur    : register(t2);
SamplerState Sampler	: register(s0);

// Entry point for this pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{

	float4 baseColor = RadialBlur.Sample(Sampler, input.uv);
	float4 bloomColor = LightBloom.Sample(Sampler, input.uv);

	float4 finalColor = baseColor + (bloomColor - (baseColor * bloomColor));

	// Average color
	return finalColor;
}