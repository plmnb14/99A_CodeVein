#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CPlayerFontUI;
class CConditionUI final : public CUI
{
public:
	enum CONDITION_TYPE
	{
		CONDITION_HP,
		CONDITION_ST,
		CONDITION_BLOOD,
		CONDITION_ATT,
		CONDITION_DEF,
		CONDITION_END
	};
private:
	explicit CConditionUI(_Device pDevice);
	explicit CConditionUI(const CConditionUI& rhs);
	virtual ~CConditionUI() = default;

public:
	void Set_ConditionType(CONDITION_TYPE eType) { m_eType = eType; }
	void Set_Condition_Info(_float fCurValue, _float fMaxValue);

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);
	void	SetUp_State(_double TimeDelta);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	_float					m_fPercentage = 0.f;
	_float					m_fMaxValue = 100.f;
	_float					m_fCurValue = 0.f;
	CONDITION_TYPE			m_eType;
	OBJECT_PARAM			m_tObjParam;
	CPlayerFontUI*			m_pFontValue = nullptr;
	_v2						m_vOldSize;

public:
	static CConditionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
