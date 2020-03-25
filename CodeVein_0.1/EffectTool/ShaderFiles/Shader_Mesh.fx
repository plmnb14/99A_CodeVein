

matrix		g_matWorld, g_matView, g_matProj, g_matInvVP, g_matLastVP;

texture		g_DiffuseTexture;

sampler		DiffuseSampler = sampler_state
{
	texture = g_DiffuseTexture;
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
};

struct VS_BLUROUT
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vTex : TEXCOORD1;
	float2		vVelocity : TEXCOORD2;
	float4		vProjPos : TEXCOORD3;
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
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;

	Out.vProjPos = Out.vPosition;

	return Out;		
}

VS_BLUROUT VS_MOTIONBLUR(VS_IN In)
{
	VS_BLUROUT	Out = (VS_BLUROUT)0;

	// 월드변환, 뷰변환, 투영행렬변환.
	matrix		matWV, matWVP, matVP;

	matWV = mul(g_matWorld, g_matView);
	matVP = mul(g_matView, g_matProj);
	matWVP = mul(matWV, g_matProj);

	//==============================================================================

	float4 currentPos = mul(float4(In.vPosition.xyz, 1.f), matWVP);

	// Use the world position, and transform by the previous view-projection matrix.    
	float4 previousPos = mul(float4(In.vPosition.xyz, 1.f), g_matLastVP);
	// Convert to nonhomogeneous points [-1,1] by dividing by w.
	previousPos /= previousPos.w;
	// Use this frame's position and last frame's to compute the pixel    
	// velocity.   
	Out.vVelocity = (currentPos.xy - previousPos.xy) / 2.f;

	//==============================================================================
	//float4 x1 = mul(In.vPosition, matWV);
	//float4 x0 = mul(In.vPosition, g_matLastVP); // 1프레임 전의 뷰좌표
	//float3 v = x1.xyz  - x0.xyz; // 속도
	//float3 n = mul(In.vNormal, matWV); // 뷰 법선
	//
	////x1 = mul(In.vPosition, matWV);
	////x0 = mul(In.vPosition, g_matLastVP);
	//
	//bool bFront = (0 <= dot(n, v.xyz)); // 속도	방향으로 향하는가?
	//float4 x = bFront ? x1 : x0; // 방향에 따라 위치 정하기
	//
	//Out.vTex.x = Out.vPosition.x + Out.vPosition.w;
	//Out.vTex.y = -Out.vPosition.y + Out.vPosition.w;
	//Out.vTex.w = 2.0f * Out.vPosition.w;
	//
	//float4 s0 = mul(x0, matVP); s0 /= s0.w;
	//float4 s1 = mul(x1, matVP); s1 /= s1.w;
	//Out.vVelocity = s1 - s0;
	//Out.vVelocity.x *= 0.5f;
	//Out.vVelocity.y *= -0.5f;

	//Out.vPosition = mul(x, matVP);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);	
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_matWorld));
	Out.vTexUV = In.vTexUV;
	Out.vProjPos = mul(vector(In.vPosition.xyz, 1.f), matWVP);
	//Out.vProjPos = Out.vPosition;
	

	return Out;
}

// POSITION시멘틱을 가진 멤버변수에 대해서 W값으로 XYZW를 나누는 연산을 수행.(원근 투영)
// 투영스페이스 상에 존재하는 정점(-1, 1 ~ 1, -1)을 뷰포트영역상의 정점(0, 0 ~ WINCX, WINCY)으로 변환한다.
// 래스터라이즈 : 세개 정점에 둘러쌓여진 영역안에 존재하는 픽셀의 정보를 정점정보를 기반하여 생성한다.

struct PS_IN
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vProjPos : TEXCOORD1;
};

struct PS_BLURIN
{
	float4		vPosition : POSITION;
	float4		vNormal : NORMAL;
	float2		vTexUV : TEXCOORD0;
	float4		vTex : TEXCOORD1;
	float2		vVelocity : TEXCOORD2;
	float4		vProjPos : TEXCOORD3;
};

struct PS_OUT
{
	vector		vDiffuse : COLOR0;
	vector		vNormal : COLOR1;
	vector		vDepth : COLOR2;
	vector		vVelocity : COLOR3;
};

// 픽셀의 색을 결정한다.
PS_OUT PS_MAIN(PS_IN In) 
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vDiffuse = tex2D(DiffuseSampler, In.vTexUV);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);

	Out.vVelocity = 0;

	return Out;
}

PS_OUT PS_MOTIONBLUR(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	//matrix matWVP = g_matWorld * g_matView * g_matProj;
	//
	//float4 currentPos = mul(float4(In.vTexUV, 0, 1), matWVP);
	//
	//// Use the world position, and transform by the previous view-projection matrix.    
	//float4 previousPos = mul(float4(In.vTexUV, 0, 1), g_matLastVP);
	//// Convert to nonhomogeneous points [-1,1] by dividing by w.
	//previousPos /= previousPos.w;
	//// Use this frame's position and last frame's to compute the pixel    
	//// velocity.   
	//
	//Out.vVelocity = (currentPos - previousPos);// *0.00001f;
	

	//float4 NewPos = mul(In.vProjPos, matWVP);
	//float4 OldPos = mul(In.vProjPos, g_matLastVP);
	//float2 velocity = NewPos.xy / NewPos.w - OldPos.xy / OldPos.w;
	//Out.vVelocity.xy = velocity * 0.5f;
	//Out.vVelocity.y *= -1.0f;
	//Out.vVelocity.z *= In.vProjPos.z;
	//Out.vVelocity.w *= In.vProjPos.w;

	//float	zOverW = In.vProjPos.z / In.vProjPos.w;
	float	zOverW = 1;
	float4 H = float4(In.vTexUV.x * 2 - 1, (1 - In.vTexUV.y) * 2 - 1, zOverW, 1);
	//float4 H = mul(In.vProjPos, matWVP);
	
	// 월드( 로컬위치 * 월드)
	float4 D = mul(H, g_matInvVP);
	vector worldPos = D / D.w;
	float4 currentPos = H;
	float4 previousPos = mul(worldPos, g_matLastVP);
	previousPos /= previousPos.w;
	
	float2 velocity = (currentPos.xy - previousPos.xy) / 2.f;
	//velocity *= 0.01f;

	//for (int i = 0; i > 26; i++)
	//{
	//	float t = float(i + 1) / 26;
	//	Out.vVelocity += tex2D(DiffuseSampler, In.vTex / In.vTex.w + t * In.vVelocity);
	//}
	//Out.vVelocity /= 26;

	Out.vDiffuse = tex2D(DiffuseSampler, In.vTexUV);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w / 500.f, 0.f, 0.f);
	Out.vVelocity = vector(velocity.xy, 0.f, 1.f);
	//Out.vVelocity = vector(In.vVelocity.xy, 0.f, 0.f);

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

		VertexShader = compile  vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MOTIONBLUR();
	}
}

