#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CExpendables final : public CGameObject
{
public:
	enum EXPEND_TYPE
	{
		EXPEND_1,
		EXPEND_2,
		EXPEND_3,
		EXPEND_4,
		EXPEND_END
	};

private:
	explicit CExpendables(_Device pDevice);
	explicit CExpendables(const CExpendables& rhs);
	virtual ~CExpendables() = default;

public:
	void Set_Type(EXPEND_TYPE eType) { m_eType = eType; }

public:
	EXPEND_TYPE Get_Type() { return m_eType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	EXPEND_TYPE m_eType;

public:
	static CExpendables*	Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END

