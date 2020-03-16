#include "Terrain_Guide.h"

CTerrain_Guide::CTerrain_Guide(_Device pGraphic_Device)
	: CGameObject(pGraphic_Device)
{

}

CTerrain_Guide::CTerrain_Guide(const CTerrain_Guide & rhs)
	: CGameObject(rhs)
{

}

HRESULT CTerrain_Guide::Ready_GameObject_Prototype()
{

	Add_Component();

	m_pTransform->Set_Pos(V3_NULL);
	m_pTransform->Set_Scale(V3_ONE);

	// 생성 시, 오래 걸릴 수 있느 ㄴ작업\들을 수행한다.
	return NOERROR;
}

HRESULT CTerrain_Guide::Ready_GameObject(void * pArg)
{
	// 복제해서 생성 된 후, 추가적으로 필요한 데이터들을 셋팅하낟.
	if (FAILED(Add_Component()))
		return E_FAIL;

	return NOERROR;
}

_int CTerrain_Guide::Update_GameObject(_double TimeDelta)
{
	return NO_EVENT;
}

_int CTerrain_Guide::Late_Update_GameObject(_double TimeDelta)
{
	return NO_EVENT;
}

HRESULT CTerrain_Guide::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pBuffer)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	m_pShader->Begin_Pass(0);
	
	m_pBuffer->Render_VIBuffer();

	m_pShader->End_Pass();

	m_pShader->End_Shader();

	return NOERROR;
}

HRESULT CTerrain_Guide::Add_Component()
{
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_Terrain", L"Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Terrain", L"VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Collider", L"Collider", (CComponent**)&m_pCollider)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTerrain_Guide::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CManagement*		pManagement = CManagement::Get_Instance();
	if (nullptr == pManagement)
		return E_FAIL;

	Safe_AddRef(pManagement);

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_pTransform->Get_WorldMat(), sizeof(_mat))))
		return E_FAIL;

	//_mat		ViewMatrix = pManagement->Get_Transform(D3DTS_VIEW);
	//_mat		ProjMatrix = pManagement->Get_Transform(D3DTS_PROJECTION);
	_mat		ViewMatrix;
	_mat		ProjMatrix;

	m_pGraphic_Dev->GetTransform(D3DTS_VIEW, &ViewMatrix);
	m_pGraphic_Dev->GetTransform(D3DTS_PROJECTION, &ProjMatrix);

	if (FAILED(m_pShader->Set_Value("g_matView", &ViewMatrix, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &ProjMatrix, sizeof(_mat))))
		return E_FAIL;


	Safe_Release(pManagement);

	return NOERROR;
}

CTerrain_Guide * CTerrain_Guide::Create(_Device pGraphic_Device)
{
	CTerrain_Guide*	pInstance = new CTerrain_Guide(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX("Failed To Creating CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;

}

CGameObject * CTerrain_Guide::Clone_GameObject(void * pArg)
{
	CTerrain_Guide*	pInstance = new CTerrain_Guide(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
	{
		MSG_BOX("Failed To Cloned CMainApp");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTerrain_Guide::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pCollider);

	CGameObject::Free();
}
