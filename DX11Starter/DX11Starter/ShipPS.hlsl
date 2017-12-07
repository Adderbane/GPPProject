
// Struct representing the data we expect to receive from earlier pipeline stages
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal       : NORMAL;
	float2 uv           : TEXTCOORD;
	float3 worldPos     : POSITION;
	float3 tangent		: TANGENT;
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
cbuffer externalData : register(b0)
{
	DirectionalLight dirLight;
	PointLight lightList[64];
	int pointLightCount;
	float4 cameraPosition;
};

Texture2D diffuseTexture  : register(t0);
Texture2D normalMap       : register(t1);
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

float3 calcNormal(float3 normalFromTexture, float3 normalFromVS, float3 tangentFromVS) {
	//Unpack normal from texture sample
	float3 unpackedNormal = normalFromTexture * 2.0f - 1.0f;
	//Create the TBN matrix
	float3 N = normalize(normalFromVS); //From model in C++
	float3 T = normalize(tangentFromVS - dot(tangentFromVS, N) * N); //Calculated from UVs in C++
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	//transform normal from map
	float3 finalNormal = mul(unpackedNormal, TBN);

	return finalNormal;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float3 normal = calcNormal(normalMap.Sample(basicSampler, input.uv).xyz, input.normal, input.tangent);

	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	float4 light = 0.0f;

	light += calcDirLight(dirLight, normal);

	for (int i = 0; i < pointLightCount; i++)
	{
		light += calcPointLight(lightList[i], input.worldPos, normal);
	}

	return surfaceColor * light;
}