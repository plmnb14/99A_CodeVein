#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CArmor final : public CGameObject
{
public:
	enum ARMOR_TYPE
	{
		ARMOR_1,
		ARMOR_2,
		ARMOR_3,
		ARMOR_4,
		ARMOR_END
	};

private:
	explicit CArmor(_Device pDevice);
	explicit CArmor(const CArmor& rhs);
	virtual ~CArmor() = default;

public:
	ARMOR_TYPE Get_Type() { return m_eType; }

public:
	void Set_Type(ARMOR_TYPE eType) { m_eType = eType; }

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);

public:
	virtual _int	Update_GameObject(_double TimeDelta);

private:
	ARMOR_TYPE				m_eType;

public:
	static CArmor*			Create(_Device pGraphic_Device);
	virtual CGameObject*	Clone_GameObject(void* pArg);
	virtual void			Free();
};

END