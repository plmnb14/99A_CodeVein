#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)
class CMaterialCollectionUI final : public CUI
{
private:
	explicit CMaterialCollectionUI(_Device pDevice);
	explicit CMaterialCollectionUI(const CMaterialCollectionUI& rhs);
	virtual ~CMaterialCollectionUI() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	HRESULT Add_Component();
	void	SetUp_Default();
	void	Update_SubUI();
	void	Click_SubUI();

private:
	CTransform*				m_pTransformCom = nullptr;

private:
	vector<CMaterial_Slot*> m_vecSlot;

public:
	static CMaterialCollectionUI*	Create(_Device pGraphic_Device);
	virtual CGameObject*			Clone_GameObject(void* pArg);
	virtual void					Free();
};
END
