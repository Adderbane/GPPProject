
static float blurWeights[16] = { 0.009033f,	0.018476f,	0.033851f,	0.055555f,	0.08167f,
0.107545f,	0.126854f,	0.134032f,	0.126854f,	0.107545f,
0.08167f,	0.055555f,	0.033851f,	0.018476f,	0.009033f, 0.0f };

cbuffer Data : register(b0)
{
	float2 passDir;
	float pixelWidth;
	float pixelHeight;
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

	for (int i = 0; i < 15; i++)
	{
		float2 uv = input.uv + (i-7) * float2(passDir.x * pixelWidth * 2.0f, passDir.y * pixelHeight * 2.0f);
		
		finalColor += blurWeights[i] * BasePixels.Sample(Sampler, uv);
	}

	// Average color
	return finalColor;
}