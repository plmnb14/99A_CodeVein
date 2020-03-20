

matrix		g_matWorld, g_matView, g_matProj;
float		g_fAlpha;
vector		g_vColor;

texture		g_DiffuseTexture;
sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_GradientTexture;
sampler		GradientSampler = sampler_state
{
	texture = g_GradientTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_DepthTexture;
sampler		DepthSampler = sampler_state
{
	texture = g_DepthTexture;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;		
}

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	float fGradientUV = In.vTexUV + (g_fAlpha);
	vector vGradientMask = tex2D(GradientSampler, fGradientUV );
	Out.vColor *= vGradientMask;

	Out.vColor *= g_vColor;

	float2		vTexUV;
	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	float		fViewZ = vDepthInfo.y * 300.f;

	Out.vColor.a = (Out.vColor.a * saturate(fViewZ - In.vProjPos.w)) * g_fAlpha;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{	
		ZWriteEnable = false;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = One;
		//DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}	
}

