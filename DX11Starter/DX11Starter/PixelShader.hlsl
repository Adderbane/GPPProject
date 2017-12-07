
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
	float2 uv           : TEXTCOORD;
	float3 worldPos     : POSITION;

};

struct DirectionalLight {
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float3 direction;
};

struct PointLight {
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float3 position;
	float  radius;
};

// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{

};

Texture2D diffuseTexture : register(t0);
SamplerState basicSampler : register(s0);

float4 calcDirLight(DirectionalLight light, float3 normal)
{
	float3 lightNormDir = normalize(-light.direction);

	float nDotL = saturate(dot(normal, lightNormDir));

	return (light.diffuseColor * nDotL) + light.ambientColor;
}

float4 calcPointLight(PointLight light, float3 position, float3 normal)
{
	float3 lightDist = light.position - position;
	float3 lightNormDir = normalize(lightDist);

	float nDotL = saturate(dot(normal, lightNormDir));

	return ((light.diffuseColor * nDotL) + light.ambientColor) * (light.radius / (.04 * dot(lightDist, lightDist)));
}

float4 calcNormal(float3 normalFromTexture, float3 normalFromVS, float3 tangentFromVS) {
	//Unpack normal from texture sample
	float3 unpackedNormal = normalFromTexture * 2.0f - 1.0f;
	//Create the TBN matrix
	float3 N = normalize(normalFromVS); //From model in C++
	float3 T = normalize(tangentFromVS - dot(tangentFromVS, N) * N); //Calculated from UVs in C++
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	//transform normal from map
	float3 finalNormal = mul(unpackedNormal, TBN);
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

	return surfaceColor;
}