#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI final : public CUI
{
private:
	explicit CPlayerFontUI(_Device pDevice);
	explicit CPlayerFontUI(const CPlayerFontUI& rhs);
	virtual ~CPlayerFontUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	virtual void Set_ScaleUp(_float _fSizeMultiply = 4.f);
	virtual void Update_Scale();
	virtual void Set_Alpha(_float _fAlpha) { m_fTimerAlpha = _fAlpha; }

public:
	virtual void Set_ParentMatrix(_mat* _pParentMatrix) { m_pMatParent = _pParentMatrix; }

public:
	virtual void Update_NumberValue(_float _fValue) { m_fValue = _fValue; }

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CTexture*				m_pTexture = nullptr;
	CShader*				m_pShader = nullptr;
	CBuffer_RcTex*			m_pBuffer = nullptr;

private:
	_mat*					m_pMatParent = nullptr;

private:
	_v3						m_vOldScale;

private:
	_short					m_arrDigitIdx[5] = {};
	_float					m_fValue = 0.f;
	_float					m_fTimerAlpha = 1.f;
	_bool					m_bNegative = false;
	_bool					m_bDecreScale = false;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

private:
	_int	Calc_Interval(_float _fValue);
	_int	Calc_LoopCnt(_float _fValue);
	void	Calc_NumTexIdx(_float _fValue, _int _iMaxLoopCnt);
	void	Calc_BillBoard(_int iLoopCnt, _float _fInterval);

private:
	HRESULT Update_Matrix();

public:
	static CPlayerFontUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END