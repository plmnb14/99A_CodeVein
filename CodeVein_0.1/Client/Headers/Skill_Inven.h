#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CSkillSlot;
class CInventory_Icon;
class CSkill_Inven final : public CUI
{
public:
	explicit CSkill_Inven(_Device pDevice);
	explicit CSkill_Inven(const CSkill_Inven& rhs);
	virtual ~CSkill_Inven() = default;

public:
	vector<Skill_ID>	Get_Skill_Data() { return m_vecData; }

public:
	void Set_SkillUI_TotalInven(_uint iIndex);

public:
	virtual HRESULT			Ready_GameObject_Prototype();
	virtual HRESULT			Ready_GameObject(void* pArg);
	virtual _int			Update_GameObject(_double TimeDelta);
	virtual _int			Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT			Render_GameObject();

private:
	HRESULT					Add_Component();
	HRESULT					SetUp_ConstantTable();
	void					SetUp_Default();
	void					Click_SubUI();

public:
	void Add_Skill_Data(Skill_ID eSkillID);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	vector<CSkillSlot*>		m_vecSlot;
	_uint					m_iRegistIdx = 0;
	vector<Skill_ID>		m_vecData;
	CInventory_Icon*		m_pExitIcon = nullptr;

public:
	static CSkill_Inven*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
