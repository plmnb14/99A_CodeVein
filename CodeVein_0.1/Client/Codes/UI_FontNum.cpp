#include "stdafx.h"
#include "..\Headers\FontNumUI.h"
#include "..\Headers\UI_FontNum.h"


CUI_FontNum::CUI_FontNum(_Device pDevice)
	: CUI(pDevice)
{
}

CUI_FontNum::CUI_FontNum(const CUI_FontNum & rhs)
	: CUI(rhs)
{
}

HRESULT CUI_FontNum::Ready_GameObject_Prototype()
{
	CUI::Ready_GameObject_Prototype();

	return NOERROR;
}

HRESULT CUI_FontNum::Ready_GameObject(void * pArg)
{
	if (FAILED(Add_Component()))
		return E_FAIL;

	CUI::Ready_GameObject(pArg);


	return NOERROR;
}

_int CUI_FontNum::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	CUI::Update_GameObject(TimeDelta);

	m_pRenderer->Add_RenderList(RENDER_UI, this);

	D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);

	return NO_EVENT;
}

_int CUI_FontNum::Late_Update_GameObject(_double TimeDelta)
{
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);

	m_matWorld._11 = m_fSizeX;
	m_matWorld._22 = m_fSizeY;
	m_matWorld._33 = 1.f;
	m_matWorld._41 = m_fPosX - WINCX * 0.5f;
	m_matWorld._42 = -m_fPosY + WINCY * 0.5f;


	return NO_EVENT;
}

HRESULT CUI_FontNum::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pBuffer)
		return E_FAIL;

	g_pManagement->Set_Transform(D3DTS_WORLD, m_matWorld);

	g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
	g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);


	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	// 숫자만큼 반복
	for( _int i = 0; i < Calc_LoopCnt(m_fValue); ++i)
	{
		// i 번째 숫자에 따라 텍스쳐 변경
		m_iIndex = Calc_NumTexIdx(m_fValue ,  i);

		m_pShader->Begin_Pass(1);

		// 번호에 따라 넘버 텍스쳐 세팅
		m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, m_iIndex);

		m_pShader->Commit_Changes();

		m_pBuffer->Render_VIBuffer();

		m_pShader->End_Pass();
	}

	m_pShader->End_Shader();

	return NOERROR;
}

void CUI_FontNum::Update_Number()
{
}

HRESULT CUI_FontNum::Add_Component()
{
	// For.Com_Transform
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Transform", L"Com_Transform", (CComponent**)&m_pTransform)))
		return E_FAIL;

	// For.Com_Renderer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Renderer", L"Com_Renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;

	// For.Com_Texture
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_FontNumber", L"Com_Texture", (CComponent**)&m_pTexture)))
		return E_FAIL;

	// For.Com_Shader
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Shader_UI", L"Com_Shader", (CComponent**)&m_pShader)))
		return E_FAIL;

	// for.Com_VIBuffer
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"VIBuffer_Rect", L"Com_VIBuffer", (CComponent**)&m_pBuffer)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CUI_FontNum::SetUp_ConstantTable()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matView", &m_matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &m_matProj, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, m_iIndex)))
		return E_FAIL;

	return NOERROR;
}

_int CUI_FontNum::Calc_Interval(_float _fValue)
{
	return _int();
}

_int CUI_FontNum::Calc_LoopCnt(_float _fValue)
{
	// 소수점 출력은 안함

	_int iOut = 0;
	_bool bNegative = false;

	if (0.f > _fValue)
		bNegative = true;

	if (false == bNegative)
	{
		iOut =
			(_fValue < 10 ? 1 :
				_fValue < 100 ? 2 :
				_fValue < 1000 ? 3 :
				_fValue < 10000 ? 4 :
				_fValue < 100000 ? 5 : 0);
	}

	else if (true == bNegative)
	{
		iOut =
			(_fValue > -10.f ? 2 :
				_fValue > -100.f ? 3 :
				_fValue > -1000.f ? 4 :
				_fValue > -10000.f ? 5 :
				_fValue > -100000.f ? 6 : 0);
	}

	return iOut;
}

_int CUI_FontNum::Calc_NumTexIdx(_float _fValue, _int _iLoopCnt)
{
	// 소수점 출력은 안함

	_int iOut[8] = {};
	_bool bNegative = false;

	_int iValue = (_int)_fValue;

	//while ()
	//{
	//	iOut[8] = iValue % 10;
	//}


	return iOut[_iLoopCnt];
}

HRESULT CUI_FontNum::Update_Matrix()
{
	return S_OK;
}

CUI_FontNum * CUI_FontNum::Create(_Device pGraphic_Device)
{
	CUI_FontNum* pInstance = new CUI_FontNum(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
		Safe_Release(pInstance);

	return pInstance;
}

CGameObject * CUI_FontNum::Clone_GameObject(void * pArg)
{
	CUI_FontNum* pInstance = new CUI_FontNum(*this);

	if (FAILED(pInstance->Ready_GameObject(pArg)))
		Safe_Release(pInstance);

	return pInstance;
}

void CUI_FontNum::Free()
{
	Safe_Release(m_pTransform);
	Safe_Release(m_pBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pTexture);
	Safe_Release(m_pRenderer);

	CUI::Free();
}
