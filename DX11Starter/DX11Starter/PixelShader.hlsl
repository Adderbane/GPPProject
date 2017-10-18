
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal       : NORMAL;
	float2 uv           : TEXTCOORD;
};

struct DirectionalLight {
	float4 ambientColor;
	float4 diffuseColor;
	float3 direction;
};

// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{
	DirectionalLight dirLight1;
	DirectionalLight dirLight2;
	float4 cameraPosition;
};

Texture2D diffuseTexture : register(t0);
SamplerState basicSampler : register(s0);

float4 calcDirLight(DirectionalLight light, float3 normal)
{
	float3 lightNormDir = normalize(-light.direction);

	float nDotL = saturate(dot(normal, lightNormDir));

	return (light.diffuseColor * nDotL) + light.ambientColor;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	float4 light1 = calcDirLight(dirLight1, input.normal);
	float4 light2 = calcDirLight(dirLight2, input.normal);

	return surfaceColor * (light1 + light2);
}