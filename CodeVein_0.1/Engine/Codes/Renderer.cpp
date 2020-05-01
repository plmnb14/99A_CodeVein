#include "..\Headers\Renderer.h"
#include "PipeLine.h"
#include "Management.h"

//test
#include"TexEffect.h"
CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pLight_Manager(CLight_Manager::Get_Instance())
{
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pTarget_Manager);
}

HRESULT CRenderer::Ready_Component_Prototype()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;
	m_pGraphic_Dev->GetViewport(&ViewPort);

	// Target_Diffuse // 
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Diffuse", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Normal
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Depth
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Emissive
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Emissive", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 1.f, 0.f))))
		return E_FAIL;

	// Target_Velocity
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Velocity", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_NormalForRim
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_RimNormal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.f))))
		return E_FAIL;
	// Target_BloomPower
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BloomPower", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_DecalDepth
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_DecalDepth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;

	// 명암을 저장한다.
	// Target_Shade (명암. 내적. 실수값을.)
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Specular
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_SSAO
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_SSAO", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_SSAO
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_SSAO_Blur", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;


	// Target_ShadowMap
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_ShadowMap", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Shadow ( 원래 D3DFMT_A8R8G8B8 )
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Shadow", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Distortion
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Distortion", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.2f, 0.2f, 1.f, 0.f))))
		return E_FAIL;

	// Target_Blend
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Blend", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Blur
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Blur", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_BlurH
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BlurH", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;
	// Target_BlurV
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_BlurV", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_MotionBlurObj
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_MotionBlurObj", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_MotionBlurBlend
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_MotionBlurBlend", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_Bloom (BrightPass)
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Bloom", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_Rim
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Rim", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.f))))
		return E_FAIL;

	// Target_ToneMapping
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_ToneMapping", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// MRT : Multi Render Target 그룹을 지어놓은것. 

	// For.MRT_Deferred : Target_Diffuse + Target_Normal
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Emissive")))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_Velocity")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_RimNormal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_BloomPower")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Velocity", L"Target_DecalDepth")))
		return E_FAIL;

	// For.MRT_LightAcc
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;
	//if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_SSAO")))
	//	return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Rim")))
		return E_FAIL;

	// For.MRT_SSAO
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_SSAO", L"Target_SSAO")))
		return E_FAIL;

	// For.MRT_SSAO_Blur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_SSAO_Blur", L"Target_SSAO_Blur")))
		return E_FAIL;
	
	// For.MRT_Distortion
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Distortion", L"Target_Distortion")))
		return E_FAIL;

	// For.MRT_Blend
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blend", L"Target_Blend")))
		return E_FAIL;

	// For.MRT_MotionBlur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_MotionBlurObj", L"Target_MotionBlurObj")))
		return E_FAIL;

	// For.MRT_MotionBlurBlend
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_MotionBlurBlend", L"Target_MotionBlurBlend")))
		return E_FAIL;

	// For.MRT_BrightPass
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_BrightPass", L"Target_Bloom")))
		return E_FAIL;

	// For.MRT_Blur
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blur", L"Target_Blur")))
		return E_FAIL;

	// For.MRT_HDR
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_HDR", L"Target_ToneMapping")))
		return E_FAIL;

	// For.Shader_LightAcc
	m_pShader_LightAcc = CShader::Create(m_pGraphic_Dev, L"../ShaderFiles/Shader_LightAcc.fx");
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	// For.Shader_Blend
	m_pShader_Blend = CShader::Create(m_pGraphic_Dev, L"../ShaderFiles/Shader_Blend.fx");
	if (nullptr == m_pShader_Blend)
		return E_FAIL;

	// For.Shader_Shadow
	m_pShader_Shadow = CShader::Create(m_pGraphic_Dev, L"../ShaderFiles/Shader_Shadow.fx");
	if (nullptr == m_pShader_Shadow)
		return E_FAIL;

	// For.Shader_Effect
	m_pShader_Effect = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_Effect"));
	if (nullptr == m_pShader_Effect)
		return E_FAIL;

	// For.Shader_Blur
	m_pShader_Blur = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_Blur"));
	if (nullptr == m_pShader_Blur)
		return E_FAIL;

	// For.SSAO
	m_pShader_SSAO = static_cast<CShader*>(CManagement::Get_Instance()->Clone_Component(SCENE_STATIC, L"Shader_SSAO"));
	if (nullptr == m_pShader_SSAO)
		return E_FAIL;
	
	// For.Buffer_ViewPort
	m_pViewPortBuffer = CBuffer_ViewPort::Create(m_pGraphic_Dev, 0.f - 0.5f, 0.f - 0.5f, (_float)ViewPort.Width, (_float)ViewPort.Height);
	if (nullptr == m_pViewPortBuffer)
		return E_FAIL;

	//m_pViewPortBufferForBlur = CBuffer_ViewPort::Create(m_pGraphic_Dev, 0.f, 0.f, (_float)ViewPort.Width / 4, (_float)ViewPort.Height / 4);

	// For.m_pSSAOTexture
	m_pSSAOTexture = CTexture::Create(m_pGraphic_Dev, CTexture::TYPE_GENERAL, L"../../Client/Resources/Texture/Effect/Normal/Normal_4.tga");
	// static_cast<CTexture*>(CComponent_Manager::Get_Instance()->Clone_Component(SCENE_STATIC, L"Tex_Noise", nullptr));

	m_iInstanceCnt = 200;
	m_pInstanceData = new INSTANCEDATA[m_iInstanceCnt];

	m_pGraphic_Dev->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);


#ifdef _DEBUG

	_float fTargetSize = 150.f;
	// For.Target_Diffuse`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Diffuse", 0.0f, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Normal`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Normal", 0.0f, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;
	
	// For.Target_Depth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Depth", 0.0f, fTargetSize * 2, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Depth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Emissive", 0.0f, fTargetSize * 3, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Depth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Velocity", 0.0f, fTargetSize * 4, fTargetSize, fTargetSize)))
		return E_FAIL;
	// For.Target_DecalDepth`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_DecalDepth", fTargetSize * 2, fTargetSize * 4, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Shade`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shade", fTargetSize, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Specular`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Specular", fTargetSize, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_SSAO`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_SSAO", fTargetSize * 6, fTargetSize * 0, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_SSAO_Blur`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_SSAO_Blur", fTargetSize * 6, fTargetSize * 1, fTargetSize, fTargetSize)))
		return E_FAIL;


	// For.Target_Rim`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Rim", fTargetSize, fTargetSize * 4, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_ShadowMap`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_ShadowMap", fTargetSize * 4, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Shadow`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shadow", fTargetSize * 4, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Blend`s Debug Buffer // 톤매핑 직전
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Blend", fTargetSize * 3, 0.f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Bloom`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Bloom", fTargetSize, fTargetSize * 3, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Distortion`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Distortion", fTargetSize * 2, 0.f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Blur`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Blur", fTargetSize * 2, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_MotionBlur`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_MotionBlurObj", fTargetSize * 2, fTargetSize * 2, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_ToneMapping`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_ToneMapping", fTargetSize * 3, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

#endif


	return NOERROR;
}

HRESULT CRenderer::Ready_Component(void * pArg)
{

	return NOERROR;
}

HRESULT CRenderer::Add_RenderList(RENDERID eGroup, CGameObject * pGameObject)
{
	if (RENDER_END <= eGroup ||
		nullptr == pGameObject)
		return E_FAIL;

	m_RenderList[eGroup].push_back(pGameObject);

	Safe_AddRef(pGameObject);

	return NOERROR;
}

HRESULT CRenderer::Draw_RenderList()
{
	// 디퓨즈, 노멀타겟에 필요한 정보를 그려놓느낟.
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;

	if (FAILED(Render_SSAO()))
		return E_FAIL;

	// VelocityMap, NormalMap For Rim-light
	if (FAILED(Render_MotionBlurTarget()))
		return E_FAIL;

	//if (FAILED(Render_ShadowMap()))
	//	return E_FAIL;

	// 노멀타겟과 빛정보를 이용하여 셰이드타겟에 값을 그리낟.
	if (FAILED(Render_LightAcc()))
		return E_FAIL;

	// 디퓨즈, 셰이드 두 타겟을 혼합하여 백버퍼에 찍는다.
	// With Skybox(priority), With Alpha
	if (FAILED(Render_Blend()))
		return E_FAIL;

	// Bloom Target을 계산
	if (FAILED(Render_BrightPass()))
		return E_FAIL;

	// Blur
	if (FAILED(Render_Blur()))
		return E_FAIL;

	// 모션 블러 계산
	if (FAILED(Render_MotionBlurObj()))
		return E_FAIL;

	// 블렌드 + 모션블러
	if (FAILED(Render_MotionBlur()))
		return E_FAIL;

	// HDR
	if (FAILED(Render_ToneMapping()))
		return E_FAIL;

	// Distortion 타겟에 그림 // 순서 상관X
	if (FAILED(Render_Distortion()))
		return E_FAIL;

	// 후처리
	if (FAILED(Render_After()))
		return E_FAIL;
	if (FAILED(Render_3dUI()))
		return E_FAIL;
	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG

	if (CInput_Device::Get_Instance()->Key_Down((_ubyte)DIKEYBOARD_9))
		m_bOnRenderTarget = !m_bOnRenderTarget;

	if (m_bOnRenderTarget)
	{
		if (nullptr == m_pTarget_Manager)
			return E_FAIL;

		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Deferred");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Velocity");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_LightAcc");
		//m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_ShadowMap");
		//m_pTarget_Manager->Render_Debug_Buffer_Single(L"Target_Shadow");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Blend");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Distortion");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Blur");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_MotionBlurObj");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_HDR");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_BrightPass");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_SSAO");
		m_pTarget_Manager->Render_Debug_Buffer(L"MRT_SSAO_Blur");

		m_pGraphic_Dev->SetTexture(0, nullptr);
	}

#endif

	return NOERROR;
}

HRESULT CRenderer::Render_Priority()
{
	for (auto& pGameObject : m_RenderList[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_PRIORITY].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_NonAlpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	// 백버퍼를 뺀다. 
	// 디퓨즈타겟과 노멀타겟을 장치에 셋한다. == MRT_Deferred를 비긴한다.

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Deferred")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_NONALPHA].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Deferred")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_ShadowMap()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pTarget_Manager->New_Stencil(L"Target_ShadowMap");
	m_pGraphic_Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	_mat matWorld, matView, matProj;
	
	_v3 vLightPos = _v3(5.f, 8.f, -5.f);
	_v3 vLookAt = V3_NULL;
	//_v3 vLookAt = m_vLookAtPos;

	D3DXMatrixLookAtLH(&matView, &vLightPos, &vLookAt, &WORLD_UP);
	D3DXMatrixOrthoLH(&matProj, 20.f, 20.f, 0.1f, 100.f);

	m_pShader_Shadow->Set_Value("g_matLightView", &matView, sizeof(_mat));
	m_pShader_Shadow->Set_Value("g_matLightProj", &matProj, sizeof(_mat));
	m_pShader_Shadow->Set_Value("g_LightPos", &vLightPos, sizeof(_v3));

	m_pShader_Shadow->Begin_Shader();

	for (auto& pGameObject : m_RenderList[RENDER_SHADOWTARGET])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetPass(m_pShader_Shadow, 0))) // 그림자 맵 패스로 렌더
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
		}
	}

	m_pShader_Shadow->End_Shader();

	m_pTarget_Manager->Origin_Stencil(L"Target_ShadowMap");

	Render_Shadow();

	return NOERROR;
}

HRESULT CRenderer::Render_Shadow()
{
	m_pTarget_Manager->New_Stencil(L"Target_Shadow");
	m_pGraphic_Dev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DXCOLOR(0.f, 0.f, 0.f, 0.f), 1.f, 0);

	_float fOffsetX = 0.5f + (0.5f / 1280.f);
	_float fOffsetY = 0.5f + (0.5f / 720.f);

	_mat matScaleBias;
	D3DXMatrixIdentity(&matScaleBias);


	matScaleBias._11 = 0.5f;
	matScaleBias._22 = -0.5f;
	matScaleBias._33 = 1.f;
	matScaleBias._41 = fOffsetX;
	matScaleBias._42 = fOffsetY;
	matScaleBias._44 = 1.f;

	m_pShader_Shadow->Set_Value("g_matBias", matScaleBias, sizeof(_mat));

	_mat matWorld, matView, matProj;

	_v3 vLightPos = _v3(5.f, 8.f, -5.f);
	_v3 vLookAt = V3_NULL;
	//_v3 vLookAt = m_vLookAtPos;

	
	D3DXMatrixLookAtLH(&matView, &vLightPos, &vLookAt, &WORLD_UP);
	D3DXMatrixOrthoLH(&matProj, 20.f, 20.f, 0.1f, 100.f);

	m_pShader_Shadow->Set_Value("g_matLightView", &matView, sizeof(_mat));
	m_pShader_Shadow->Set_Value("g_matLightProj", &matProj, sizeof(_mat));
	m_pShader_Shadow->Set_Value("g_LightPos", &vLightPos, sizeof(_v3));

	m_pShader_Shadow->Set_Texture("g_ShadowMapTexture", m_pTarget_Manager->Get_Texture(L"Target_ShadowMap"));

	m_pShader_Shadow->Begin_Shader();

	_uint a = 0;

	for (auto& pGameObject : m_RenderList[RENDER_SHADOWTARGET])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetPass(m_pShader_Shadow, 1))) // 그림자 맵 패스로 렌더
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}

			Safe_Release(pGameObject);
		}
	}
	m_RenderList[RENDER_SHADOWTARGET].clear();

	m_pShader_Shadow->End_Shader();

	m_pTarget_Manager->Origin_Stencil(L"Target_Shadow");
	

	return NOERROR;
}

HRESULT CRenderer::Render_MotionBlurTarget()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Velocity")))
		return E_FAIL;

	m_pShader_Blur->Begin_Shader();

	for (auto& pGameObject : m_RenderList[RENDER_MOTIONBLURTARGET])
	{

		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetPass(m_pShader_Blur, 0)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}

	}
	
	m_pShader_Blur->End_Shader();

	m_RenderList[RENDER_MOTIONBLURTARGET].clear();


	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Velocity")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Distortion()
{
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Distortion")))
		return E_FAIL;

	m_pShader_Effect->Begin_Shader();

	for (auto& pGameObject : m_RenderList[RENDER_DISTORTION])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetShader(m_pShader_Effect)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_DISTORTION].clear();

	m_pShader_Effect->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Distortion")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Alpha()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_ALPHA].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_Effect()
{
	m_pShader_Effect->Begin_Shader();

	for (auto& pGameObject : m_RenderList[RENDER_EFFECT])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject_SetShader(m_pShader_Effect)))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_EFFECT].clear();

	m_pShader_Effect->End_Shader();

	Render_Instance();

	return NOERROR;
}


HRESULT CRenderer::Render_Instance()
{
	m_pShader_Effect->Begin_Shader();

	_int iIdx = 0;
	_ULonglong iSizeCheck = m_RenderList[RENDER_INSTANCE].size();
	ZeroMemory(m_pInstanceData, sizeof(INSTANCEDATA) * m_iInstanceCnt);

	for (auto& pGameObject : m_RenderList[RENDER_INSTANCE])
	{
		if (nullptr != pGameObject)
		{
			CTexEffect* pEff = static_cast<CTexEffect*>(pGameObject);
			m_pInstanceData[iIdx] = pEff->Get_InstanceData();
			Safe_Release(pGameObject);
			++iIdx;
			--iSizeCheck;

			if (iIdx >= m_iInstanceCnt || iSizeCheck == 0)
			{
				CBuffer_RcTex* pBuffer = static_cast<CBuffer_RcTex*>(pGameObject->Get_Component(L"Com_VIBuffer"));
				pBuffer->Render_Before_Instancing(m_pInstanceData, iIdx);

				_mat		ViewMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_VIEW);
				_mat		ProjMatrix = CManagement::Get_Instance()->Get_Transform(D3DTS_PROJECTION);

				if (FAILED(m_pShader_Effect->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
					return E_FAIL;
				if (FAILED(m_pShader_Effect->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
					return E_FAIL;

				D3DXMatrixInverse(&ViewMatrix, nullptr, &ViewMatrix);
				D3DXMatrixInverse(&ProjMatrix, nullptr, &ProjMatrix);

				if (FAILED(m_pShader_Effect->Set_Value("g_matProjInv", &ProjMatrix, sizeof(_mat))))
					return E_FAIL;
				if (FAILED(m_pShader_Effect->Set_Value("g_matViewInv", &ViewMatrix, sizeof(_mat))))
					return E_FAIL;
				m_pShader_Effect->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"));

				pEff->SetUp_ConstantTable_Instance(m_pShader_Effect);

				m_pShader_Effect->Begin_Pass(3);

				m_pShader_Effect->Commit_Changes();

				pBuffer->Render_DrawPrimitive_Instancing();

				m_pShader_Effect->End_Pass();

				pBuffer->Render_After_Instancing();

				ZeroMemory(m_pInstanceData, sizeof(INSTANCEDATA) * m_iInstanceCnt);
				iIdx = 0;
			}
		}
	}

	m_RenderList[RENDER_INSTANCE].clear();

	m_pShader_Effect->End_Shader();

	return NOERROR;
}

_bool Compare_Z(CGameObject* pDest, CGameObject* pSour)
{
	return pDest->Get_ViewZ() > pSour->Get_ViewZ();
}

HRESULT CRenderer::Render_UI()
{
	m_RenderList[RENDER_UI].sort(Compare_Z);

	for (auto& pGameObject : m_RenderList[RENDER_UI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_UI].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_3dUI()
{
	for (auto& pGameObject : m_RenderList[RENDER_3DUI])
	{
		if (nullptr != pGameObject)
		{
			if (FAILED(pGameObject->Render_GameObject()))
			{
				Safe_Release(pGameObject);
				return E_FAIL;
			}
			Safe_Release(pGameObject);
		}
	}

	m_RenderList[RENDER_3DUI].clear();

	return NOERROR;
}

HRESULT CRenderer::Render_SSAO()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_SSAO)
		return E_FAIL;

	m_pSSAOTexture->SetUp_OnShader("g_RandomTexture", m_pShader_SSAO, 0);

	if (FAILED(m_pShader_SSAO->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pShader_SSAO->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_Texture(L"Target_Normal"))))
		return E_FAIL;
	//

	CPipeLine*		pPipeLine = CPipeLine::Get_Instance();
	if (nullptr == pPipeLine)
		return E_FAIL;

	Safe_AddRef(pPipeLine);

	m_pShader_SSAO->Set_Value("g_matProjInv", &pPipeLine->Get_Transform_Inverse(D3DTS_PROJECTION), sizeof(_mat));
	m_pShader_SSAO->Set_Value("g_matViewInv", &pPipeLine->Get_Transform_Inverse(D3DTS_VIEW), sizeof(_mat));

	//
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_SSAO")))
		return E_FAIL;

	m_pShader_SSAO->Begin_Shader();
	m_pShader_SSAO->Begin_Pass(0);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_SSAO->End_Pass();
	m_pShader_SSAO->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_SSAO")))
		return E_FAIL;

	//
	Safe_Release(pPipeLine);


	// SSAO 블러
	if (FAILED(m_pShader_SSAO->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_Texture(L"Target_SSAO"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_SSAO_Blur")))
		return E_FAIL;

	m_pShader_SSAO->Begin_Shader();
	m_pShader_SSAO->Begin_Pass(1);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_SSAO->End_Pass();
	m_pShader_SSAO->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_SSAO_Blur")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
	if (nullptr == m_pTarget_Manager ||
		nullptr == m_pLight_Manager)
		return E_FAIL;

	CPipeLine*		pPipeLine = CPipeLine::Get_Instance();
	if (nullptr == pPipeLine)
		return E_FAIL;

	Safe_AddRef(pPipeLine);

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	m_pShader_LightAcc->Set_Texture("g_NormalTexture", m_pTarget_Manager->Get_Texture(L"Target_Normal"));
	m_pShader_LightAcc->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"));
	m_pShader_LightAcc->Set_Texture("g_ShadowMapTexture", m_pTarget_Manager->Get_Texture(L"Target_Shadow"));
	m_pShader_LightAcc->Set_Texture("g_RimNormalTexture", m_pTarget_Manager->Get_Texture(L"Target_RimNormal"));
	
	m_pShader_LightAcc->Set_Value("g_matProjInv", &pPipeLine->Get_Transform_Inverse(D3DTS_PROJECTION), sizeof(_mat));
	m_pShader_LightAcc->Set_Value("g_matViewInv", &pPipeLine->Get_Transform_Inverse(D3DTS_VIEW), sizeof(_mat));


	_v3 CamPos = pPipeLine->Get_CamPosition();
	m_pShader_LightAcc->Set_Value("g_vCamPosition", &_v4(CamPos, 1.f), sizeof(_v4));

	m_pShader_LightAcc->Begin_Shader();

	m_pLight_Manager->Render_Light(m_pShader_LightAcc);

	m_pShader_LightAcc->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_LightAcc")))
		return E_FAIL;

	Safe_Release(pPipeLine);

	return NOERROR;
}

HRESULT CRenderer::Render_Blend()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Diffuse"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_Texture(L"Target_Shade"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_SpecularTexture", m_pTarget_Manager->Get_Texture(L"Target_Specular"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_EmissiveTexture", m_pTarget_Manager->Get_Texture(L"Target_Emissive"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_RimTexture", m_pTarget_Manager->Get_Texture(L"Target_Rim"))))
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_Texture(L"Target_SSAO_Blur"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Blend")))
		return E_FAIL;

	if (FAILED(Render_Priority()))
		return E_FAIL;


	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(0);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	// Alpha
	if (FAILED(Render_Alpha()))
		return E_FAIL;

	// Effect
	if (FAILED(Render_Effect()))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Blend")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_BrightPass()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Blend"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_BloomTexture", m_pTarget_Manager->Get_Texture(L"Target_BloomPower"))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_BrightPass")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(3);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_BrightPass")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Blur()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	// Blur H ==================================================
	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Bloom"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_SSAOTexture", m_pTarget_Manager->Get_Texture(L"Target_SSAO"))))
		return E_FAIL;

	m_pTarget_Manager->Begin_Render_Target(L"Target_BlurH");

	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(2);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	m_pTarget_Manager->End_Render_Target(L"Target_BlurH");
	// Blur H ==================================================

	for (_int i = 0; i < 13; ++i) // 홀수만
	{
		if (i % 2 == 0)
		{
			m_pTarget_Manager->Begin_Render_Target(L"Target_BlurV");

			if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurH"))))
				return E_FAIL;
			m_pShader_Blend->Begin_Shader();
			m_pShader_Blend->Begin_Pass(8);

			m_pViewPortBuffer->Render_VIBuffer();

			m_pShader_Blend->End_Pass();
			m_pShader_Blend->End_Shader();
			m_pTarget_Manager->End_Render_Target(L"Target_BlurV");
		}
		else
		{
			// Blur H ==================================================
			if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurV"))))
				return E_FAIL;

			m_pTarget_Manager->Begin_Render_Target(L"Target_BlurH");

			m_pShader_Blend->Begin_Shader();
			m_pShader_Blend->Begin_Pass(7);

			m_pViewPortBuffer->Render_VIBuffer();

			m_pShader_Blend->End_Pass();
			m_pShader_Blend->End_Shader();

			m_pTarget_Manager->End_Render_Target(L"Target_BlurH");
			// Blur H ==================================================

		}
	}
	
	// Blur ==================================================
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Blur")))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_BlurV"))))
		return E_FAIL;
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(2);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();
	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Blur")))
		return E_FAIL;
	// Blur ==================================================

	return NOERROR;
}

HRESULT CRenderer::Render_MotionBlurObj()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat		matWVP;
	matWVP = ViewMatrix * ProjMatrix;
	//D3DXMatrixInverse(&matWVP, nullptr, &matWVP);
	//if (FAILED(m_pShader_Blend->Set_Value("g_matInvVP", &matWVP, sizeof(_mat))))
	//	return E_FAIL;

	Safe_Release(pManagement);

	// 오브젝트 단위의 모션블러, 속도맵 ==============================================================================
	m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Blend"));
	m_pShader_Blend->Set_Texture("g_ShadeTexture", m_pTarget_Manager->Get_Texture(L"Target_Velocity"));
	m_pShader_Blend->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"));

	//_float fCurFrame = _float(CFrameMgr::Get_Instance()->Get_Frame());
	//_float fTargetFrame = 60.f;
	//m_pShader_Blend->Set_Value("g_fCurFrame", &fCurFrame, sizeof(_float));
	//m_pShader_Blend->Set_Value("g_fTargetFrame", &fTargetFrame, sizeof(_float));
	
	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_MotionBlurObj")))
		return E_FAIL;

	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(6);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_MotionBlurObj")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_MotionBlur()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_MotionBlurBlend")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(5);

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Blend"))))
		return E_FAIL;
	m_pShader_Blend->Commit_Changes();
	m_pViewPortBuffer->Render_VIBuffer();

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_MotionBlurObj"))))
		return E_FAIL;
	m_pShader_Blend->Commit_Changes();
	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_MotionBlurBlend")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_ToneMapping()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_MotionBlurBlend")))) //Target_Blend 임시로 모션블러 꺼둠. 키려면 Target_MotionBlurBlend
		return E_FAIL;

	// Blur
	if (FAILED(m_pShader_Blend->Set_Texture("g_BloomTexture", m_pTarget_Manager->Get_Texture(L"Target_Blur"))))
		return E_FAIL;

	static _int iIdx = 5;
	if (GetAsyncKeyState(VK_F1) & 0x8000)
		iIdx = 0;
	if (GetAsyncKeyState(VK_F2) & 0x8000)
		iIdx = 1;
	if (GetAsyncKeyState(VK_F3) & 0x8000)
		iIdx = 2;
	if (GetAsyncKeyState(VK_F4) & 0x8000)
		iIdx = 3;
	if (GetAsyncKeyState(VK_F5) & 0x8000)
		iIdx = 4;
	if (GetAsyncKeyState(VK_F6) & 0x8000)
		iIdx = 5;

	// Tone index
	if (FAILED(m_pShader_Blend->Set_Value("g_iToneIndex", &iIdx, sizeof(_int))))
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_HDR")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(1);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_HDR")))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderer::Render_After()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_ToneMapping"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DistortionTexture", m_pTarget_Manager->Get_Texture(L"Target_Distortion"))))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(4);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	return S_OK;
}


CRenderer * CRenderer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRenderer*	pInstance = new CRenderer(pGraphic_Device);

	if (FAILED(pInstance->Ready_Component_Prototype()))
	{
		MSG_BOX("Failed To Creating CRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CComponent * CRenderer::Clone_Component(void * pArg)
{
	// 복제를 하려 들면 새로운객체를 만드는것이아니라, 
	// 원형객체의 주소를 그대로 리턴하겠다.

	Safe_AddRef(this);

	// this는 멤버함수안에서만 사용할 수 있으며, 
	// 멤버함수는 호출시에 반드시 객체를 필요로한다.
	// 해서 this는 멤버함수를 호출한 객체의 주소를 의미하낟.
	return this;
}

void CRenderer::Free()
{
	Safe_Delete_Array(m_pInstanceData);

	Safe_Release(m_pSSAOTexture);
	Safe_Release(m_pViewPortBuffer);

	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pShader_LightAcc);
	Safe_Release(m_pShader_Shadow);
	Safe_Release(m_pShader_Effect);
	Safe_Release(m_pShader_Blur);
	Safe_Release(m_pShader_SSAO);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	CComponent::Free();
}