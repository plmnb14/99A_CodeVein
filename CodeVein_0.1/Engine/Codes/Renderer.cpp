#include "..\Headers\Renderer.h"
#include "PipeLine.h"

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
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Diffuse", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(1.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Normal
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Normal", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 1.f, 0.f, 1.f))))
		return E_FAIL;	

	// Target_Depth
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Depth", ViewPort.Width, ViewPort.Height, D3DFMT_A32B32G32R32F, D3DXCOLOR(0.f, 1.f, 0.f, 0.f))))
		return E_FAIL;

	// 명암을 저장한다.
	// Target_Shade (명암. 내적. 실수값을.)
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Shade", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	// Target_Specular
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Specular", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Bloom
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Bloom", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// Target_Distortion
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Distortion", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.2f, 0.2f, 1.f, 0.f))))
		return E_FAIL;

	// Blend에 찍기 위한 Alpha 타겟
	// Target_Alpha
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Alpha", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;

	// 후처리 할 최종 Blend를 저장
	// Target_Blend
	if (FAILED(m_pTarget_Manager->Add_Render_Target(m_pGraphic_Dev, L"Target_Blend", ViewPort.Width, ViewPort.Height, D3DFMT_A16B16G16R16F, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;


	// MRT : Multi Render Target 그룹을 지어놓은것. 

	// For.MRT_Deferred : Target_Diffuse + Target_Normal
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Diffuse")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Normal")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Depth")))
		return E_FAIL;

	// For.MRT_LightAcc
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Shade")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_LightAcc", L"Target_Specular")))
		return E_FAIL;

	// For.MRT_Distortion
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Distortion", L"Target_Distortion")))
		return E_FAIL;

	// For.MRT_Alpha
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Alpha", L"Target_Alpha")))
		return E_FAIL;

	// For.MRT_Blend
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blend", L"Target_Blend")))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Blend", L"Target_Bloom")))
		return E_FAIL;

	// For.Shader_LightAcc
	m_pShader_LightAcc = CShader::Create(m_pGraphic_Dev, L"../Bin/ShaderFiles/Shader_LightAcc.fx");
	if (nullptr == m_pShader_LightAcc)
		return E_FAIL;

	// For.Shader_LightAcc
	m_pShader_Blend = CShader::Create(m_pGraphic_Dev, L"../Bin/ShaderFiles/Shader_Blend.fx");
	if (nullptr == m_pShader_Blend)
		return E_FAIL;


	// For.Buffer_ViewPort
	m_pViewPortBuffer = CBuffer_ViewPort::Create(m_pGraphic_Dev, 0.f - 0.5f, 0.f - 0.5f, (_float)ViewPort.Width, (_float)ViewPort.Height);
	if (nullptr == m_pViewPortBuffer)
		return E_FAIL;

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

	// For.Target_Shade`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Shade", fTargetSize, 0.0f, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Specular`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Specular", fTargetSize, fTargetSize, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Bloom`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Bloom", fTargetSize, fTargetSize * 2, fTargetSize, fTargetSize)))
		return E_FAIL;

	// For.Target_Distortion`s Debug Buffer
	if (FAILED(m_pTarget_Manager->Ready_Debug_Buffer(L"Target_Distortion", fTargetSize * 2, 0.f, fTargetSize, fTargetSize)))
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
	if (FAILED(Render_Priority()))
		return E_FAIL;


	// 디퓨즈, 노멀타겟에 필요한 정보를 그려놓느낟.
	if (FAILED(Render_NonAlpha()))
		return E_FAIL;

	// 노멀타겟과 빛정보를 이용하여 셰이드타겟에 값을 그리낟.
	if (FAILED(Render_LightAcc()))
		return E_FAIL;	

	// 디퓨즈, 셰이드 두 타겟을 혼합하여 백버퍼에 찍는다.
	if (FAILED(Render_Blend()))
		return E_FAIL;

	// Distortion
	if (FAILED(Render_Distortion()))
		return E_FAIL;
	
	// 후처리
	if (FAILED(Render_After()))
		return E_FAIL;

	// Alpha
	if (FAILED(Render_Alpha()))
		return E_FAIL;

	if (FAILED(Render_UI()))
		return E_FAIL;

#ifdef _DEBUG

	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Deferred");
	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_LightAcc");
	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Blend");
	m_pTarget_Manager->Render_Debug_Buffer(L"MRT_Distortion");

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

HRESULT CRenderer::Render_Distortion()
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Distortion")))
		return E_FAIL;

	for (auto& pGameObject : m_RenderList[RENDER_DISTORTION])
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

	m_RenderList[RENDER_DISTORTION].clear();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Distortion")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_Alpha()
{
	//if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Alpha")))
	//	return E_FAIL;

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

	//if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Alpha")))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_UI()
{
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

	m_pShader_LightAcc->Set_Value("g_matProjInv", &pPipeLine->Get_Transform_Inverse(D3DTS_PROJECTION), sizeof(_mat));
	m_pShader_LightAcc->Set_Value("g_matViewInv", &pPipeLine->Get_Transform_Inverse(D3DTS_VIEW), sizeof(_mat));

	m_pShader_LightAcc->Set_Value("g_vCamPosition", &_v4(pPipeLine->Get_CamPosition(), 1.f), sizeof(_v4));
	

	// 셰이더 비긴.
	m_pShader_LightAcc->Begin_Shader();

	// 셰이드 타겟과 같은 사이즈를 가진 사각형버퍼를 화면에 렌더링한다.
	m_pLight_Manager->Render_Light(m_pShader_LightAcc);

	m_pShader_LightAcc->End_Shader();


	// 셰)이더 엔드.

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
	//if (FAILED(m_pShader_Blend->Set_Texture("g_AlphaTexture", m_pTarget_Manager->Get_Texture(L"Target_Alpha"))))
	//	return E_FAIL;

	if (FAILED(m_pTarget_Manager->Begin_MRT(L"MRT_Blend")))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(0);

	m_pViewPortBuffer->Render_VIBuffer();

	m_pShader_Blend->End_Pass();
	m_pShader_Blend->End_Shader();

	if (FAILED(m_pTarget_Manager->End_MRT(L"MRT_Blend")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Render_After()
{
	if (nullptr == m_pViewPortBuffer ||
		nullptr == m_pShader_Blend)
		return E_FAIL;

	if (FAILED(m_pShader_Blend->Set_Texture("g_DiffuseTexture", m_pTarget_Manager->Get_Texture(L"Target_Blend"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DepthTexture", m_pTarget_Manager->Get_Texture(L"Target_Depth"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_BloomTexture", m_pTarget_Manager->Get_Texture(L"Target_Bloom"))))
		return E_FAIL;
	if (FAILED(m_pShader_Blend->Set_Texture("g_DistortionTexture", m_pTarget_Manager->Get_Texture(L"Target_Distortion"))))
		return E_FAIL;

	// 장치에 백버퍼가 셋팅되어있다.	
	m_pShader_Blend->Begin_Shader();
	m_pShader_Blend->Begin_Pass(1);

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

	Safe_Release(m_pViewPortBuffer);

	Safe_Release(m_pShader_Blend);
	Safe_Release(m_pShader_LightAcc);

	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pTarget_Manager);

	CComponent::Free();
}
