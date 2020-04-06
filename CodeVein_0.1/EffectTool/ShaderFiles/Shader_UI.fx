float		g_fSpeed;
float		g_fPosX, g_fSizeX;
float		g_fPercentage;
float		g_fAlpha;
float		g_fSparkle;

matrix		g_matWorld, g_matView, g_matProj;

texture		g_DiffuseTexture;

sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};


// ���������� �׸���.

// ������ ������ vsmain�Լ��� ���ڷ� ������.

// �̶� ���޵� ������ �������ۿ� ���־��� ������ ������ �״�� ���޹޾ƿ´�.

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

// ������ �������� ��ȯ�� �����Ѵ�.
// ������ ���� ��Ҹ� ������ �� �ִ�.
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	// ���庯ȯ, �亯ȯ, ������ĺ�ȯ.
	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);
	
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vTexUV = In.vTexUV;

	return Out;		
}

// ���ؽ� ���̴�
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


// POSITION�ø�ƽ�� ���� ��������� ���ؼ� W������ XYZW�� ������ ������ ����.(���� ����)
// ���������̽� �� �����ϴ� ����(-1, 1 ~ 1, -1)�� ����Ʈ�������� ����(0, 0 ~ WINCX, WINCY)���� ��ȯ�Ѵ�.
// �����Ͷ����� : ���� ������ �ѷ��׿��� �����ȿ� �����ϴ� �ȼ��� ������ ���������� ����Ͽ� �����Ѵ�.

struct PS_IN
{
	float4		vPosition : POSITION;
	float2		vTexUV : TEXCOORD0;
	float3		vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR0;
};

// �ȼ��� ���� �����Ѵ�.
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

technique Default_Technique
{
	pass Default_Rendering
	{
		// RenderState		
		AlphablendEnable = false;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlending
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		destblend = invsrcalpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	
	pass	UI_HPBar_Rendering
	{
		AlpHaBlENdEnaBLE = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_2D_UV_CTRL();
		pixelshader = compile ps_3_0 PS_TRANSLATION_TEX_UV();
	}

	pass	UI_STBar_Rendering
	{
		AlpHaBlENdEnaBLE = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_2D_UV_CTRL();
		pixelshader = compile ps_3_0 PS_MAIN();
	}

	pass	UI_HPBar_Update
	{
		AlpHaBlENdEnaBLE = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_UI_DOWN();
	}

	pass	UI_Fade_InOut
	{
		AlpHaBlENdEnaBLE = true;
		srcblend = srcalpha;
		destblend = invsrcalpha;

		vertexshader = compile vs_3_0 VS_MAIN();
		pixelshader = compile ps_3_0 PS_UI_FADE();
	}
}