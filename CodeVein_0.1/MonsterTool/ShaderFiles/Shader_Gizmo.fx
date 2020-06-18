matrix		g_matWorld, g_matView, g_matProj;

texture		g_DiffuseTexture;

float		g_fAlpha = 1.f;

sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

float4 g_GizmoColor = float4(0.f, 1.f, 0.f, 1.f);

struct VS_IN
{
	float3		vPos : POSITION;
};

struct VS_OUT
{
	float4		vPos : POSITION;
	float4		vProjPos : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPos = mul(float4(In.vPos, 1.f), matWVP);

	Out.vProjPos = Out.vPos;

	return Out;
}

VS_OUT VS_MAIN_NoWorld(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	Out.vPos = (float4)(In.vPos, 1.f);

	return Out;
}

struct PS_IN
{
	float4		vPos : POSITION;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vDiffuse	: COLOR0;
	vector		vNormal		: COLOR1;
	vector		vDepth		: COLOR2;
	vector		vEmissive	: COLOR3;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_GizmoColor;
	Out.vDiffuse.a = g_fAlpha;

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);

	Out.vEmissive = g_GizmoColor;
	Out.vEmissive.a = g_fAlpha;

	return Out;
}

PS_OUT PS_MAIN_Navi(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = g_GizmoColor;
	Out.vDiffuse.a = g_fAlpha;

	//Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.5f, 0.2f);

	//Out.vEmissive = g_GizmoColor;
	//Out.vEmissive.a = g_fAlpha;

	return Out;
}

technique Default_Technique
{
	// 0
	pass Default_Rendering_WireFrame
	{
		FillMode = WireFrame;
		cullmode = none;

		AlphablendEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	// 1
	pass Default_Rendering_Non_WireFrame
	{
		AlphablendEnable = false;
		cullmode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	// 2
	pass AlphaBlending
	{
		cullmode = ccw;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = DestAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_Navi();
	}

	// 3
	pass Default_Rendering_WireFrame_Navi
	{
		FillMode = WireFrame;
		cullmode = none;

		AlphablendEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN_Navi();
	}
}

