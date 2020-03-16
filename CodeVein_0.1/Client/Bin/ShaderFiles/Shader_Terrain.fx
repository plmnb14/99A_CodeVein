
matrix		g_matWorld, g_matView, g_matProj;

struct VS_IN
{
	float3		vPosition : POSITION;
};

struct VS_OUT
{
	float4		vPosition : POSITION;
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


	return Out;		
}


// POSITION�ø�ƽ�� ���� ��������� ���ؼ� W������ XYZW�� ������ ������ ����.(���� ����)
// ���������̽� �� �����ϴ� ����(-1, 1 ~ 1, -1)�� ����Ʈ�������� ����(0, 0 ~ WINCX, WINCY)���� ��ȯ�Ѵ�.
// �����Ͷ����� : ���� ������ �ѷ��׿��� �����ȿ� �����ϴ� �ȼ��� ������ ���������� ����Ͽ� �����Ѵ�.

struct PS_IN
{
	float4		vPosition : POSITION;
};

struct PS_OUT
{
	vector		vDiffuse : COLOR0; 
};

// �ȼ��� ���� �����Ѵ�.
PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;
	
	Out.vDiffuse = 1.f;

	return Out;
}

technique Default_Technique
{
	pass Default_Rendering
	{
		AlphablendEnable = false;

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


}

