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

	m_pTransform->Set_Pos(_v3(0.f, 0.7f, 0.f));
	m_pTransform->Set_Scale(_v3(0.1f, 0.1f, 0.1f));

	m_vOldScale = m_pTransform->Get_Size();

	m_fPosX = 200.f;
	m_fPosY = 437.f;
	m_fSizeX = 1.71875f;
	m_fSizeY = 2.34375f;

	return NOERROR;
}

_int CUI_FontNum::Update_GameObject(_double TimeDelta)
{
	if (m_bIsDead)
		return DEAD_OBJ;

	CUI::Update_GameObject(TimeDelta);

	Update_Scale();

	m_pRenderer->Add_RenderList(RENDER_UI, this);

	if(Billboard_UI == m_eType)
	{ 
		_mat tmpMat;
		D3DXMatrixIdentity(&tmpMat);
		memcpy(&tmpMat._41, &m_pMatParent->_41, sizeof(_v3));
		m_pTransform->Calc_ParentMat(&tmpMat);

		_v3 vWorldPos;

		memcpy(vWorldPos, &m_pTransform->Get_WorldMat()._41, sizeof(_v3));
		Compute_ViewZ(&vWorldPos);
		//Compute_ViewZ(&m_pTransform->Get_Pos());
	}
	
	else
	{
		D3DXMatrixOrthoLH(&m_matProj, WINCX, WINCY, 0.f, 1.f);
	}

	return NO_EVENT;
}

_int CUI_FontNum::Late_Update_GameObject(_double TimeDelta)
{
	if(Orthgrahpuic_UI == m_eType)
	{
		D3DXMatrixIdentity(&m_matWorld);
		D3DXMatrixIdentity(&m_matView);

		m_matWorld._11 = m_fSizeX;
		m_matWorld._22 = m_fSizeY;
		m_matWorld._33 = 1.f;
		m_matWorld._41 = m_fPosX - WINCX * 0.5f;
		m_matWorld._42 = -m_fPosY + WINCY * 0.5f;
	}

	return NO_EVENT;
}

HRESULT CUI_FontNum::Render_GameObject()
{
	if (nullptr == m_pShader ||
		nullptr == m_pBuffer)
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;

	m_pShader->Begin_Shader();

	_int iLoopCnt = Calc_LoopCnt(m_fValue);
	_int iLoopIdx = 0;

	Calc_NumTexIdx(m_fValue, iLoopCnt);

	// 숫자만큼 반복
	for( _int i = iLoopCnt; i > 0; --i)
	{
		// i 번째 숫자에 따라 텍스쳐 변경
		m_iIndex = m_arrDigitIdx[i - 1];

		// 렌더 패스
		m_pShader->Begin_Pass(5);

		// 번호에 따라 넘버 텍스쳐 세팅
		m_pTexture->SetUp_OnShader("g_DiffuseTexture", m_pShader, m_iIndex);

		if (Billboard_UI == m_eType)
		{
			Calc_BillBoard(i, 1.f);

			_v3 matNormal = _v3(m_matWorld._11 , 0.f, m_matWorld._13);
			V3_NORMAL_SELF(&matNormal);

			m_matWorld._41 += (matNormal.x * 0.75f);
			m_matWorld._43 += (matNormal.z * 0.75f);

			if (i < iLoopCnt)
			{
				m_matWorld._41 -= (matNormal.x * iLoopIdx * m_pTransform->Get_Size().x);
				m_matWorld._43 -= (matNormal.z * iLoopIdx * m_pTransform->Get_Size().x);
			}

			m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat));
		}

		else
		{
			if(i < iLoopCnt)
				m_matWorld._41 -= m_fSizeX;

			m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat));
		}

		m_pShader->Commit_Changes();

		m_pBuffer->Render_VIBuffer();

		m_pShader->End_Pass();

		++iLoopIdx;
	}

	m_pShader->End_Shader();

	return NOERROR;
}

void CUI_FontNum::Set_ScaleUp(_float _fSizeMultiply)
{
	m_pTransform->Set_Scale(
		_v3(m_vOldScale.x * _fSizeMultiply,
			m_vOldScale.y * _fSizeMultiply,
			m_vOldScale.z * _fSizeMultiply));

	m_bDecreScale = true;
}

void CUI_FontNum::Update_Scale()
{
	if (m_bDecreScale)
	{
		_v3 tmpScale = m_pTransform->Get_Size();

		if (tmpScale.x > m_vOldScale.x)
		{
			tmpScale -= ((tmpScale - m_vOldScale) * 10.f + V3_ONE * 2.f)  * g_pTimer_Manager->Get_DeltaTime(L"Timer_Fps_60");

			if (tmpScale.x <= m_vOldScale.x)
			{
				tmpScale = m_vOldScale;
				m_bDecreScale = false;
			}
		}

		m_pTransform->Set_Scale(tmpScale);
	}
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
	if (FAILED(CGameObject::Add_Component(SCENE_STATIC, L"Tex_DamegeNum", L"Com_Texture", (CComponent**)&m_pTexture)))
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

	_mat matView, matProj;

	if (Orthgrahpuic_UI == m_eType)
	{
		g_pManagement->Set_Transform(D3DTS_VIEW, m_matView);
		g_pManagement->Set_Transform(D3DTS_PROJECTION, m_matProj);

		matView = m_matView;
		matProj = m_matProj;
	}

	else if (Billboard_UI == m_eType)
	{
		matView = g_pManagement->Get_Transform(D3DTS_VIEW);
		matProj = g_pManagement->Get_Transform(D3DTS_PROJECTION);
	}

	if (FAILED(m_pShader->Set_Value("g_fAlpha", &m_fTimerAlpha, sizeof(_mat))))
		return E_FAIL;

	if (FAILED(m_pShader->Set_Value("g_matWorld", &m_matWorld, sizeof(_mat))))
		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matView", &matView, sizeof(_mat))))

		return E_FAIL;
	if (FAILED(m_pShader->Set_Value("g_matProj", &matProj, sizeof(_mat))))
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

	m_bNegative = false;

	if (0.f > _fValue)
		m_bNegative = true;

	if (false == m_bNegative)
	{
		iOut =
			(_fValue >= 0 && _fValue < 10 ? 1 :
				_fValue >= 10 &&_fValue < 100 ? 2 :
				_fValue >= 100 && _fValue < 1000 ? 3 :
				_fValue >= 1000 && _fValue < 10000 ? 4 :
				_fValue >= 10000 && _fValue < 100000 ? 5 : 0);
	}

	else if (true == m_bNegative)
	{
		iOut =
			(_fValue < 0 && _fValue > -10 ? 1 :
				_fValue <= -10 && _fValue > -100 ? 2 :
				_fValue <= -100 && _fValue > -1000 ? 3 :
				_fValue <= -1000 && _fValue > -10000 ? 4 :
				_fValue <= -10000 && _fValue > -100000 ? 5 : 0);
	}

	return iOut;
}

void CUI_FontNum::Calc_NumTexIdx(_float _fValue, _int _iMaxLoopCnt)
{
	_int iOut = 0;

	_int iValue = (_int)_fValue;
	_int iLoopCnt = _iMaxLoopCnt - 1;
	_int iNum = 1;

	for (_int i = 0; i < iLoopCnt; ++i)
		iNum *= 10;
	
	for (_int i = 0; i <  _iMaxLoopCnt; ++i)
	{
		iOut = iValue / iNum;
		iValue -= iOut * iNum;

		if(_iMaxLoopCnt > 1)
			iNum /= 10;

		m_arrDigitIdx[i] = iOut;
	}
}

void CUI_FontNum::Calc_BillBoard(_int iLoopCnt , _float _fInterval)
{
	_mat		matWorld, matView, matInverse;

	matView = g_pManagement->Get_Transform(D3DTS_VIEW);

	D3DXMatrixIdentity(&matInverse);
	D3DXMatrixIdentity(&matWorld);

	matWorld = m_pTransform->Get_WorldMat();

	//matWorld._41 += matWorld._11 * iLoopCnt * _fInterval;
	//matWorld._43 += matWorld._13 * iLoopCnt * _fInterval;

	matInverse._11 = matView._11;
	matInverse._13 = matView._13;
	matInverse._31 = matView._31;
	matInverse._33 = matView._33;

	D3DXMatrixInverse(&matInverse, NULL, &matInverse);

	m_matWorld = matInverse * matWorld;
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
