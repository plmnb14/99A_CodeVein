

matrix		g_MatrixPalette[35];
int			g_iNumBoneInfluences = 2;

struct VS_IN_Dynamic_Skinned
{
	float4		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float3		vTangent	: TANGENT;
	float3		vBinormal	: BINORMAL;
	float2		vTexUV		: TEXCOORD0;
	float4		weights		: BLENDWEIGHT0;
	int4		boneIndices : BLENDINDICES0;
};

struct VS_OUT
{
	float4		vPosition	: POSITION;
	float3		vNormal		: NORMAL;
	float3		vTangent	: TANGENT;
	float3		vBinormal	: BINORMAL;
	float2		vTexUV		: TEXCOORD0;
};

VS_OUT VS_Dynamic_Skinned(VS_IN_Dynamic_Skinned In)
{
	VS_OUT			Out = (VS_OUT)0;

	float4 p = float4(0.f, 0.f, 0.f, 1.f);
	float3 vNormal = float3(0.f, 0.f, 0.f);
	float lastWeight = 0.f;
	int n = g_iNumBoneInfluences - 1;

	float3 vInNormal = normalize(In.vNormal);
	float3 vInTangent = normalize(In.vTangent);
	float3 vInBinormal = normalize(In.vBinormal);

	for (int i = 0; i < n; ++i)
	{
		lastWeight += In.weights[i];
		p += In.weights[i] * mul(In.vPosition, g_MatrixPalette[In.boneIndices[n]]);

		Out.vNormal = In.weights[i] * mul(vInNormal, g_MatrixPalette[In.boneIndices[n]]);
		Out.vTangent = In.weights[i] * mul(vInTangent, g_MatrixPalette[In.boneIndices[n]]);
		Out.vBinormal = In.weights[i] * mul(vInBinormal, g_MatrixPalette[In.boneIndices[n]]);
	}

	lastWeight = 1.f - lastWeight;
	p += lastWeight * mul(In.vPosition, g_MatrixPalette[In.boneIndices[n]]);

	Out.vNormal = lastWeight * mul(vInNormal, g_MatrixPalette[In.boneIndices[n]]);
	Out.vTangent = lastWeight * mul(vInTangent, g_MatrixPalette[In.boneIndices[n]]);
	Out.vBinormal = lastWeight * mul(vInBinormal, g_MatrixPalette[In.boneIndices[n]]);
	p.w = 1.f;

	Out.vTexUV = In.vTexUV;

	return Out;
}

technique Default_Technique
{
	pass Skinned_Update
	{	
		VertexShader = compile vs_3_0 VS_Dynamic_Skinned();
	}
}

