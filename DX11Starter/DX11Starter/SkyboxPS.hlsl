
// Struct representing the data we expect to receive from earlier pipeline stages
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 direction    : TEXTCOORD;
};

TextureCube skyboxTexture : register(t0);
SamplerState basicSampler : register(s0);

// The entry point (main method) for our pixel shader
float4 main(VertexToPixel input) : SV_TARGET
{
	return skyboxTexture.Sample(basicSampler, input.direction);
}