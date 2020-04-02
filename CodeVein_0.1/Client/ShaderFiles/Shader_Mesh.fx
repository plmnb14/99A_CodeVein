

matrix		g_matWorld, g_matView, g_matProj, g_matLastWVP;// , g_matInvVP, g_matLastVP;
float		g_fFxAlpha;
texture		g_DiffuseTexture;

sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

texture		g_FXTexture;
sampler		FXSampler = sampler_state
{
	texture = g_FXTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

struct VS_IN
{
	float3		vPosition : POSITION;
	float3		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPos : TEXCOORD2;
};

struct VS_BLUROUT
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
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
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;
	Out.vLocalPos = float4(In.vPosition.xyz, 1.f);

	return Out;
}

VS_BLUROUT VS_MOTIONBLUR(VS_IN In)
{
	VS_BLUROUT	Out = (VS_BLUROUT)0;

	// ���庯ȯ, �亯ȯ, ������ĺ�ȯ.
	matrix		matWV, matWVP, matVP;

	matWV = mul(g_matWorld, g_matView);
	matVP = mul(g_matView, g_matProj);
	matWVP = mul(matWV, g_matProj);

	//==============================================================================

	float4 currentPos = mul(float4(In.vPosition.xyz, 1.f), matWVP);

	// Use the world position, and transform by the previous view-projection matrix.    
	float4 previousPos = mul(float4(In.vPosition.xyz, 1.f), g_matLastWVP);

	Out.vPosition = currentPos;
	Out.vProjPos = Out.vPosition;
	Out.vLastPos = previousPos;

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	return Out;
}

// POSITION�ø�ƽ�� ���� ��������� ���ؼ� W������ XYZW�� ������ ������ ����.(���� ����)
// ���������̽� �� �����ϴ� ����(-1, 1 ~ 1, -1)�� ����Ʈ�������� ����(0, 0 ~ WINCX, WINCY)���� ��ȯ�Ѵ�.
// �����Ͷ����� : ���� ������ �ѷ��׿��� �����ȿ� �����ϴ� �ȼ��� ������ ���������� ����Ͽ� �����Ѵ�.

struct PS_IN
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
	float4		vLocalPos : TEXCOORD2;
};

struct PS_BLURIN
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vLastPos : TEXCOORD1;
	float4		vProjPos : TEXCOORD2;
};

struct PS_OUT
{
	vector		vDiffuse : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
	vector		vVelocity : COLOR3;
};

// �ȼ��� ���� �����Ѵ�.
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	//Out.vDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);

	Out.vVelocity = 0;

	return Out;
}

PS_OUT PS_MOTIONBLUR(PS_BLURIN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float2 a = (In.vProjPos.xy / In.vProjPos.w) * 0.5 + 0.5;
	float2 b = (In.vLastPos.xy / In.vLastPos.w) * 0.5 + 0.5;
	//float2 velocity = pow(abs(a - b), 1 / 3.0)*sign(a - b) * 0.5 + 0.5;
	float2 velocity = (a - b) * 0.5 + 0.5;
	velocity = pow(velocity, 3.0);

	Out.vDiffuse = pow(tex2D(DiffuseSampler, In.vTexUV), 2.2);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vVelocity = vector(velocity.xy, Out.vDepth.x, 1.f);

	//Out.vVelocity = In.vVelocity;

	return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	float4 vColor = (float4)0.f;

	//float4 fxColor = pow(tex2D(FXSampler, In.vTexUV), 2.2);
	float4 fxColor = tex2D(FXSampler, In.vTexUV);

	if (vColor.a == 0.f)
		clip(-1);

	//if (In.vLocalPos.y < g_fFxAlpha * 2.f) // �ؿ������� ������ �����
	{
		if (fxColor.r >= g_fFxAlpha)
			vColor.a = 1;
		else
			vColor.a = 0;

		if (fxColor.r >= g_fFxAlpha - 0.01 && fxColor.r <= g_fFxAlpha + 0.01)
			vColor = pow(float4(0.9, 0.75, 0.65, 1), 2.2); //
		else
			;

		//if (fxColor.r >= g_fFxAlpha - 0.007 && fxColor.r <= g_fFxAlpha + 0.007)
		//	vColor = pow(float4(0.9, 0.87, 0.8, 0.5), 2.2); // 
		//else
		//	;

		//if (fxColor.r >= g_fFxAlpha - 0.001 && fxColor.r <= g_fFxAlpha + 0.001)
		//	vColor = float4(1, 1, 1, 1); // ��
		//else
		//	;
	}


	Out.vDiffuse = vColor;
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vVelocity = 0;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		// RenderState		
		AlphablendEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass AlphaBlending
	{
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = DestAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass MotionBlur
	{
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile  vs_3_0 VS_MOTIONBLUR();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR();
	}

	pass Dissolve
	{
		AlphaTestEnable = true;
		AlphaRef = 0;
		AlphaFunc = Greater;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_DISSOLVE();
	}
}

