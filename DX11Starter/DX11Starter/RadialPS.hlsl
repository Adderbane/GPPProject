
static float blurWeights[16] = { 0.132368, 0.125279, 0.106209, 0.080656,
								 0.054865, 0.033431, 0.018246, 0.008920,
								 0.003906, 0.001532, 0.000538, 0.000169,
								 0.000048, 0.000012, 0.000003, 0.000001 };

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
	float4 finalColor = float4(0,0,0,0);

	float2 vecToPoint = float2(0.5f, 0.5f) - input.uv;
	float2 factor = saturate(length(vecToPoint) - 0.25f);
	float2 passDir = factor * vecToPoint / 32.0f;
	for (int i = 0; i < 16; i++)
	{
		float2 uv = input.uv + (i) * float2(passDir.x, passDir.y);
		finalColor += 2.0f * blurWeights[i] * BasePixels.Sample(Sampler, uv);
	}

	// Average color
	return finalColor;
}