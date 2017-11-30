
// Constant Buffer
cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
	float3 position		: POSITION;
	float3 normal		: NORMAL;
	float2 uv           : TEXTCOORD;
	float3 tangent      : TANGENT;
};

// Struct representing the data we're sending down the pipeline
struct VertexToPixel
{
	float4 position		: SV_POSITION;
	float3 direction    : TEXTCOORD;
};

// The entry point (main method) for our vertex shader
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	//Get stationary veiw matrix
	matrix viewStatic = view;
	viewStatic._41 = 0;
	viewStatic._42 = 0;
	viewStatic._43 = 0;

	//Calc and set position
	output.position = mul(mul(float4(input.position, 1.0f), viewStatic), projection);

	output.position.z = output.position.w;

	//Set direction
	output.direction = input.position;

	return output;
}