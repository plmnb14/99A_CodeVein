#pragma once

// Ŭ���̾�Ʈ���� �����Ҵ��Ͽ� ����� ������ �θ� �Ǵ� Ŭ������.
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CScene abstract : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CScene() = default;
public:
	virtual HRESULT Ready_Scene();
	virtual _int Update_Scene(_double TimeDelta);
	virtual HRESULT Render_Scene();
protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;

protected:
	_bool						m_bLoadStaticMesh = false;

public:
	virtual void Free();
};

END