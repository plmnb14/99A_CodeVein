

matrix		g_matWorld, g_matView, g_matProj;

texture		g_DiffuseTexture;

sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
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


// POSITION시멘틱을 가진 멤버변수에 대해서 W값으로 XYZW를 나누는 연산을 수행.(원근 투영)
// 투영스페이스 상에 존재하는 정점(-1, 1 ~ 1, -1)을 뷰포트영역상의 정점(0, 0 ~ WINCX, WINCY)으로 변환한다.
// 래스터라이즈 : 세개 정점에 둘러쌓여진 영역안에 존재하는 픽셀의 정보를 정점정보를 기반하여 생성한다.

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

// 픽셀의 색을 결정한다.
PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = tex2D(DiffuseSampler, In.vTexUV);

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
		DestBlend = DestAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}


}

