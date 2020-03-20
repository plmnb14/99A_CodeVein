#pragma once

#include "Management.h"
#include "GameObject.h"
#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CUI : public CGameObject
{
public:
	typedef struct tagUI_DESC
	{
		_float					fPosX, fPosY;
		_float					fSizeX, fSizeY;
	}UI_DESC;

protected:
	explicit CUI(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	void Set_UI_Pos(_float fPosX, _float fPosY);
	void Set_UI_Size(_float fSizeX, _float fSizeY);

public:
	virtual HRESULT Ready_GameObject_Prototype(); // ������ü ���� ��, ȣ��� �Լ�.
	virtual HRESULT Ready_GameObject(void* pArg); // �ΰ��� ��ü ���� ��, ȣ��� �Լ�.
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

protected:
	_mat					m_matProj;
	_mat					m_matWorld;
	_mat					m_matView;
	_mat					m_matOldView, m_matOldProj;

	_float					m_fPosX, m_fPosY;
	_float					m_fSizeX, m_fSizeY;
	
	UI_DESC*				m_pUIDesc = nullptr;

public:
	virtual CGameObject* Clone_GameObject(void* pArg) = 0; // ���簴ü ����.
	virtual void	Free();
};

END