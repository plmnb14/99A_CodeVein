#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CStageUI;
class CStageSelectUI final : public CUI
{
private:
	explicit CStageSelectUI(_Device pDevice);
	explicit CStageSelectUI(const CStageSelectUI& rhs);
	virtual ~CStageSelectUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);

private:
	HRESULT Add_Component();

private:
	CTransform*				m_pTransformCom = nullptr;
	vector<CStageUI*>		m_vecStageUI;
	_uint					m_iSelectIndex = 0;

public:
	static CStageSelectUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};
END
