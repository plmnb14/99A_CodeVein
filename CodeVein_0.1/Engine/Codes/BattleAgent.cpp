#include "BattleAgent.h"
#include "Management.h"
#include "Timer_Manager.h"

CBattleAgent::CBattleAgent(_Device _pGraphicDev)
	:CComponent(_pGraphicDev)
{
	ZeroMemory(&m_tRimParam, sizeof(RIM_PARAM));
}

CBattleAgent::CBattleAgent(const CBattleAgent& rhs)
	:CComponent(rhs)
{
	ZeroMemory(&m_tRimParam, sizeof(RIM_PARAM));
}

CBattleAgent::~CBattleAgent(void)
{

}

_int CBattleAgent::Update_Component()
{
	if (m_tRimParam.bDecreRim)
	{
		Update_RimLight();
	}

	return NO_EVENT;
}

void CBattleAgent::Set_RimChangeData(_bool _bOnRimLight)
{
	m_tRimParam.bDecreRim = _bOnRimLight ? true : false;
	m_tRimParam.vRimColor_Cur = _bOnRimLight ? m_tRimParam.vRimColor_Change : _v4(0.f, 0.f, 0.f, 1.f);
	m_tRimParam.fRimTimer_Cur = 0.f;
	m_tRimParam.fRimChangeValue = 0.f;
	m_tRimParam.fRimValue = _bOnRimLight ? 1.f : 0.f;
}

void CBattleAgent::Update_RimParam_OnShader(CShader * _pShader)
{
	_v4	vCamPos = CManagement::Get_Instance()->Get_CamPosition();

	_pShader->Set_Value("g_vCamPos", &vCamPos, sizeof(_v3));
	_pShader->Set_Value("g_fRimAlpha", &m_tRimParam.fRimValue, sizeof(_float));
	_pShader->Set_Value("g_vRimColor", &m_tRimParam.vRimColor_Cur, sizeof(_v4));
}

void CBattleAgent::Update_RimLight()
{
	m_tRimParam.fRimTimer_Cur += CTimer_Manager::Get_Instance()->Get_DeltaTime(L"Timer_Fps_60");
	m_tRimParam.fRimChangeValue = (m_tRimParam.fRimTimer_Cur / m_tRimParam.fRimTimer_Max);
	m_tRimParam.fRimValue = 1.f - ((m_tRimParam.fRimTimer_Cur / m_tRimParam.fRimTimer_Max) * 0.5f);

	D3DXVec4Lerp(&m_tRimParam.vRimColor_Cur, &m_tRimParam.vRimColor_Cur,
		&m_tRimParam.vRimColor_Old, m_tRimParam.fRimChangeValue);

	if (m_tRimParam.fRimChangeValue >= 1.f)
	{
		m_tRimParam.fRimChangeValue = 0.f;
		m_tRimParam.vRimColor_Cur = m_tRimParam.vRimColor_Old;
		m_tRimParam.fRimValue = 0.5f;
		m_tRimParam.fRimTimer_Cur = 0.f;
		m_tRimParam.bDecreRim = false;
	}
}

void CBattleAgent::Create_HitLight(_v3 vLightPos, _float fLightRange, _float fLifetime)
{
	NEW_LIGHT		LightDesc;
	ZeroMemory(&LightDesc, sizeof(NEW_LIGHT));

	float fAmbient = 0.1f;

	LightDesc.Type = D3DLIGHT_POINT;
	LightDesc.Diffuse = D3DXCOLOR(0.7f, 0.5f, 0.f, 1.f);
	LightDesc.Ambient = D3DXCOLOR(fAmbient, fAmbient, fAmbient, 1.f);
	LightDesc.Specular = LightDesc.Diffuse;
	LightDesc.Position = vLightPos;
	LightDesc.Range = fLightRange;

	LightDesc.bLifeTime = true;
	LightDesc.fLifeTime_Cur = fLifetime;
	LightDesc.fLifeTime_Max = fLifetime;
	LightDesc.fAlpha = 1.f;

	CManagement::Get_Instance()->Add_Light(m_pGraphic_Dev, LightDesc, CLight_Manager::Dynamic_Light);
}

HRESULT CBattleAgent::Ready_BattleAgent()
{
	m_tRimParam.fRimChangeValue = 0.f;
	m_tRimParam.fRimValue = 0.5f;
	m_tRimParam.fRimTimer_Cur = 0.f;
	m_tRimParam.fRimTimer_Max = 1.f;
	m_tRimParam.bDecreRim = false;

	m_tRimParam.vRimColor_Cur = _v4(0.8f, 0.8f, 0.8f, 1.f);	// 현재 림라이트 색상
	m_tRimParam.vRimColor_Change = _v4(1.f, 0.5f, 0.f, 1.f);	// 바뀌길 원하는 림라이트 색상
	m_tRimParam.vRimColor_Old = _v4(0.8f, 0.8f, 0.8f, 1.f);	// 최초 림라이트 색상

	return S_OK;
}

HRESULT CBattleAgent::Ready_BattleAgent_Prototype()
{
	return S_OK;
}

CBattleAgent* CBattleAgent::Create(_Device _pGraphicDev, _bool bChoice, _ulong dwCntX, _ulong dwCntZ)
{
	CBattleAgent*	pInstance = new CBattleAgent(_pGraphicDev);

	if (FAILED(pInstance->Ready_BattleAgent_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CComponent* CBattleAgent::Clone_Component(void* pArg)
{
	CBattleAgent*	pInstance = new CBattleAgent(*this);

	if (FAILED(pInstance->Ready_BattleAgent()))
		Safe_Release(pInstance);

	return pInstance;
}

void CBattleAgent::Free(void)
{
	CComponent::Free();
}

