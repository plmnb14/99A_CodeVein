#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CActive_Icon final : public CUI
{
public:
	enum ACTIVE_STATE
	{
		ACTIVE_USE, // 사용가능
		ACTIVE_NO_USE, // 사용 불가능
		ACTIVE_COOL, // 쿨타임
		ACTIVE_BUFF // 버프
	};
private:
	explicit CActive_Icon(_Device pDevice);
	explicit CActive_Icon(const CActive_Icon& rhs);
	virtual ~CActive_Icon() = default;

public:
	void Set_Skill_Index(Skill_Index eSkill_Index) {
		m_Skill_Index = eSkill_Index;
	}
	void Set_Active_State(ACTIVE_STATE eState) { m_eState = eState; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable(_uint iIndex);

private:
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CBuffer_RcTex*			m_pBufferCom = nullptr;

private:
	Skill_Index		m_Skill_Index = Skill_End;
	ACTIVE_STATE	m_eState = ACTIVE_COOL;
	_float			m_fPercentage = 0.f;
	_float			m_fMaxTime = 10.f;
	_float			m_fCurTime = 0.f;

public:
	static CActive_Icon*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
