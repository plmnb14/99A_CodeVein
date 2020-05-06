#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CSkillSlot;
class CInventory_Icon;
class CFighterBlood final : public CUI
{
private:
	explicit CFighterBlood(_Device pDevice);
	explicit CFighterBlood(const CFighterBlood& rhs);
	virtual ~CFighterBlood() = default;

public:
	vector<Skill_ID> Get_FighterBloodData() { return m_vecData; }

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
	void					SetUp_SlotPos();
	void					Click_Icon();
	void					Reset_SlotState();

public:
	void					Add_FighterBlood(Skill_ID eSkill_ID);

private:
	CBuffer_RcTex*			m_pBufferCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;

private:
	vector<CSkillSlot*>		m_vecSlot;
	CInventory_Icon*		m_pExitIcon = nullptr;
	vector<Skill_ID>		m_vecData;

public:
	static CFighterBlood*		Create(_Device pGraphic_Device);
	virtual CGameObject*		Clone_GameObject(void* pArg);
	virtual void				Free();
};
END
