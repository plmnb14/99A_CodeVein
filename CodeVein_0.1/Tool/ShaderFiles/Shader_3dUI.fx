
matrix		g_matWorld, g_matView, g_matProj;
float		g_fAlpha;
float		g_fSpeed;
texture		g_DiffuseTexture;

sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_MaskTexture;

sampler		MaskSampler = sampler_state
{
	texture = g_MaskTexture;
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

	float2		vTexUV;

	vTexUV.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
	vTexUV.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

	vector		vDepthInfo = tex2D(DepthSampler, vTexUV);
	float		fViewZ = vDepthInfo.y * 300.f;

	//Out.vColor.a = Out.vColor.a * saturate(fViewZ - In.vProjPos.w);
	Out.vColor.a *= Out.vColor.r;

	return Out;
}

PS_OUT PS_MAIN2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_MAIN3(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.a *= Out.vColor.r;

	return Out;
}

PS_OUT PS_3dUI_FADE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.a *= g_fAlpha;

	return Out;
}

PS_OUT PS_UI_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 DiffuseUV = In.vTexUV;
	DiffuseUV.y += g_fSpeed;

	Out.vColor = tex2D(DiffuseSampler, DiffuseUV);

	Out.vColor *= tex2D(MaskSampler, In.vTexUV).r;

	return Out;
}

PS_OUT PS_BLOODSKILL_NO_RELEASE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.r = 0.541176f;
	Out.vColor.g = 0.541176f;
	Out.vColor.b = 0.541176f;

	return Out;
}


technique Default_Technique
{
	pass Default_Rendering //0
	{
		ZwriteEnable = true;
		AlphablendEnable = false;
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN2();
	}
	pass Default_Rendering2 //1
	{
		ZwriteEnable = true;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN2();
	}
	pass Default_Rendering3 //2
	{
		ZwriteEnable = false;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN3();
	}

	pass Fade_Rendering // 3
	{
		ZwriteEnable = false;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_3dUI_FADE();
	}
	pass Masking_Rendering // 4
	{
		ZwriteEnable = false;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_UI_MASK();
	}
	pass NoReleaseSkill_Rendering // 5
	{
		ZwriteEnable = false;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BLOODSKILL_NO_RELEASE();
	}
	pass Zwrite_Rendering //6
	{
		ZwriteEnable = false;
		AlphablendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN2();
	}
}