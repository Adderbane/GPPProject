
// Struct representing the data we expect to receive from earlier pipeline stages
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 normal       : NORMAL;
	float2 uv           : TEXTCOORD;
	float3 worldPos     : POSITION;
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
	PointLight bulletLight;
	float4 cameraPosition;
};

float4 calcPointLight(PointLight light, float3 position, float3 normal, float3 camera)
{
	float3 lightDist = light.position - position;
	float3 lightNormDir = normalize(lightDist);

	//Diffuse
	float nDotL = saturate(dot(normal, lightNormDir));

	//Specular
	float3 cameraDir = normalize(camera - position);
	float3 halfway = normalize(lightNormDir + cameraDir);

	float nDotH = saturate(dot(normal, halfway));

	float specAmt = pow(nDotH, 128);

	return ((specAmt * light.specularColor) + (light.diffuseColor * nDotL) + light.ambientColor) * (light.radius / (.04 * dot(lightDist, lightDist)));
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float4 surfaceColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	float4 light = 0.0f;

	light += calcPointLight(bulletLight, input.worldPos, input.normal, cameraPosition.xyz);

	return surfaceColor * light;
}