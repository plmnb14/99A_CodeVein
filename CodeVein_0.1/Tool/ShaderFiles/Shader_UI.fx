float		g_fSpeed;
float		g_fPosX, g_fSizeX;
float		g_fPercentage;
float		g_fAlpha;
float		g_fSparkle;
float2		g_vNoiseDir;

matrix		g_matWorld, g_matView, g_matProj;

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

// 정점세개를 그리낟.

// 각가의 정점을 vsmain함수의 인자로 던진다.

// 이때 전달된 정점은 정점버퍼에 들어가있었던 정점의 정보를 그대로 전달받아온다.

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		vWorldPos : TEXCOORD1;
};

struct VS_OUT2
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		vWorldPos : TEXCOORD1;
	float		fTexPercent : TEXCOORD2;
};


struct VS_OUT3
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

// 정점의 기초적인 변환을 수행한다.
// 정점의 구성 요소를 변형할 수 있다.
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	// 월드변환, 뷰변환, 투영행렬변환.
	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vTexUV = In.vTexUV;

	return Out;		
}

// 버텍스 쉐이더
VS_OUT		VS_2D_UV_CTRL(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.x - mul(mul(In.vTexUV.x, 1.f - g_fPercentage), 1.f), In.vPosition.yz, 1.f), matWVP);
	Out.vTexUV.y = In.vTexUV.y;
	Out.vTexUV.x = mul(In.vTexUV.x, g_fPercentage);

	return Out;
}

VS_OUT2		VS_SKILL_COOL(VS_IN In)
{
	VS_OUT2	Out = (VS_OUT2)0;

	matrix	matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition.x, In.vPosition.y, In.vPosition.z, 1.f), matWVP);
	Out.vTexUV.x = In.vTexUV.x;
	Out.vTexUV.y = In.vTexUV.y;

	Out.fTexPercent = g_fPercentage;
	
	return Out;
}

VS_OUT3 VS_3D_UI(VS_IN In)
{
	VS_OUT3			Out = (VS_OUT3)0;

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = Out.vPosition;

	return Out;
}

// POSITION시멘틱을 가진 멤버변수에 대해서 W값으로 XYZW를 나누는 연산을 수행.(원근 투영)
// 투영스페이스 상에 존재하는 정점(-1, 1 ~ 1, -1)을 뷰포트영역상의 정점(0, 0 ~ WINCX, WINCY)으로 변환한다.
// 래스터라이즈 : 세개 정점에 둘러쌓여진 영역안에 존재하는 픽셀의 정보를 정점정보를 기반하여 생성한다.

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		vWorldPos : TEXCOORD1;
};



struct PS_IN2
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		vWorldPos : TEXCOORD1;
	float		fTexPercent : TEXCOORD2;
};

struct PS_IN3
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};


// 픽셀의 색을 결정한다.
PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;

	
	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_TRANSLATION_TEX_UV(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV.x += g_fSpeed;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_UI_DOWN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;


	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.r = 0.f;
	Out.vColor.g = 1.f;
	Out.vColor.b = 0.f;

	return Out;
}

PS_OUT PS_UI_FADE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.a *= g_fAlpha;

	return Out;
}

PS_OUT PS_SKILL_UI(PS_IN2 In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	if (In.vTexUV.y < 1.f - In.fTexPercent)
		Out.vColor.a = 0.f;
	
	return Out;
}

PS_OUT PS_UI_MASK(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor *= Out.vColor.a;

	return Out;
}

PS_OUT PS_UI_MASK2(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 DiffuseUV = In.vTexUV;
	DiffuseUV.y += g_fSpeed;

	Out.vColor = tex2D(DiffuseSampler, DiffuseUV);

	Out.vColor *= tex2D(MaskSampler, In.vTexUV).r;

	return Out;
}

PS_OUT PS_UI_MASK3(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.a *= Out.vColor.r;

	return Out;
}

PS_OUT PS_HPBar_Noise(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	In.vTexUV += g_vNoiseDir;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

	Out.vColor.a *= Out.vColor.r;

	Out.vColor.r = 1.f;
	Out.vColor.g = 0.5f;
	Out.vColor.b = 0.036f;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering	// 0
	{
		// RenderState		
		AlphablendEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlending		// 1
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		destblend = invsrcalpha;
		
		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	
	pass	UI_HPBar_Rendering	// 2
	{
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_2D_UV_CTRL();
		pixelshader = compile ps_3_0 PS_TRANSLATION_TEX_UV();
	}

	pass	UI_STBar_Rendering	// 3
	{
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_2D_UV_CTRL();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass	UI_HPBar_Update	// 4
	{
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_UI_DOWN();
	}

	pass	UI_Fade_InOut	// 5
	{
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_UI_FADE();
	}
	pass	Logo_BackUI_Rendering	// 6
	{
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_TRANSLATION_TEX_UV();
	}

	pass	Skill_UI_Rendering	// 7
	{
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_SKILL_COOL();
		pixelshader = compile ps_3_0 PS_SKILL_UI();
	}

	pass	UI_Masking_Rendering	// 8
	{
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_UI_MASK();
	}
	pass	UI_R_Masking_Rendering	// 9
	{
		zEnable = false;
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_UI_MASK2();
	}
	pass	UI_HP_Noise	// 10
	{
		zEnable = true;
		AlphaBlendEnable = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_2D_UV_CTRL();
		pixelshader = compile ps_3_0 PS_HPBar_Noise();
	}
}
