#include "..\Headers\GameObject.h"
#include "Management.h"

CGameObject::CGameObject(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Dev(pGraphic_Device)
{
	Safe_AddRef(m_pGraphic_Dev);

	ZeroMemory(&m_tObjParam, sizeof(OBJECT_PARAM));
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pGraphic_Dev(rhs.m_pGraphic_Dev)
{
	Safe_AddRef(m_pGraphic_Dev);
	ZeroMemory(&m_tObjParam, sizeof(OBJECT_PARAM));
}

HRESULT CGameObject::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Ready_GameObject(void * pArg)
{
	return S_OK;
}

_int CGameObject::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	Check_Dissolve(TimeDelta);

	COMPONENTS::iterator iter_begin = m_pmapComponents.begin();
	COMPONENTS::iterator iter_end = m_pmapComponents.end();

	for (; iter_begin != iter_end; ++iter_begin)
	{
		iter_begin->second->Update_Component();
	}

	return NO_EVENT;
}

_int CGameObject::Late_Update_GameObject(_double TimeDelta)
{
	return _int();
}

HRESULT CGameObject::LateInit_GameObject()
{
	HRESULT hr = E_FAIL;

	if (!m_bIsInit)
	{
		hr = this->LateInit_GameObject();
		m_bIsInit = true;
	}

	return hr;
}

HRESULT CGameObject::Render_GameObject()
{
	return S_OK;
}

HRESULT CGameObject::Render_GameObject_SetPass(CShader* pShader, _int iPass)
{
	return S_OK;
}

void CGameObject::Set_Dead()
{
	m_bIsDead = true;
}

void CGameObject::Start_Dissolve(_float fFxSpeed, _bool bFadeIn, _bool bReadyDead)
{
	m_iPass = 3;
	m_bFadeIn = bFadeIn;
	m_fFXSpeed = fFxSpeed;
	m_bReadyDead = bReadyDead;

	if (bFadeIn)
		m_fFXAlpha = 1.f;
	else
		m_fFXAlpha = 0.f;
}

HRESULT CGameObject::Add_Component(_uint iSceneID, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent** ppComponent, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CComponent_Manager*		pComponent_Manager = CComponent_Manager::Get_Instance();
	if (nullptr == pComponent_Manager)
		return E_FAIL;

	Safe_AddRef(pComponent_Manager);

	CComponent*	pComponent = pComponent_Manager->Clone_Component(iSceneID, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppComponent = pComponent;

	m_pmapComponents.insert(COMPONENTS::value_type(pComponentTag, pComponent));

	Safe_AddRef(pComponent);

	Safe_Release(pComponent_Manager);

	return NOERROR;
}

CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_pmapComponents.begin(), m_pmapComponents.end(), CTag_Finder(pComponentTag));

	if (iter == m_pmapComponents.end())
		return nullptr;

	return iter->second;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_pmapComponents.begin(), m_pmapComponents.end(), CTag_Finder(pComponentTag));

	if (iter == m_pmapComponents.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Compute_ViewZ(const _v3* pPos)
{
	_mat	matView;

	m_pGraphic_Dev->GetTransform(D3DTS_VIEW, &matView);
	D3DXMatrixInverse(&matView, NULL, &matView);

	_v3	vCamPos;
	memcpy(&vCamPos, &matView.m[3][0], sizeof(_v3));

	m_fViewZ = D3DXVec3Length(&(vCamPos - *pPos));
}

void CGameObject::Check_Dissolve(_double TimeDelta)
{
	if (m_iPass != 3)
		return;

	if(!m_bFadeIn)
	{
		m_fFXAlpha += _float(TimeDelta) * m_fFXSpeed;
		
		if (m_fFXAlpha >= 1.f)
		{
			if (m_bReadyDead)
				m_bIsDead = true;
			m_fFXAlpha = 1.f;
			m_iPass = 0;
		}
	}
	else
	{
		m_fFXAlpha -= _float(TimeDelta) * m_fFXSpeed;

		if (m_fFXAlpha <= 0.f)
		{
			m_fFXAlpha = 0.f;
			m_iPass = 0;
		}
	}
}

void CGameObject::Free()
{
	for (auto& Pair : m_pmapComponents)
		Safe_Release(Pair.second);

	m_pmapComponents.clear();

	Safe_Release(m_pGraphic_Dev);
}
