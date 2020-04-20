#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CMonster abstract : public CGameObject
{
protected:
	explicit CMonster(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() PURE;
	virtual HRESULT Ready_GameObject(void* pArg) PURE;
	virtual _int Update_GameObject(_double TimeDelta) PURE;
	virtual _int Late_Update_GameObject(_double TimeDelta) PURE;
	virtual HRESULT LateInit_GameObject();
	virtual HRESULT Render_GameObject() PURE;
	virtual HRESULT Render_GameObject_SetShader(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

protected:	// DJ
	_tchar m_pLayerTag_Of_Target[256] = { L"Layer_Player" };

protected:
	void Set_Target_To_Player();
	void Set_Target_To_Colleague();

	_bool Is_InFov(_float fDegreeOfFov, CTransform* pSelfTransform, _v3 vTargetPos);

	HRESULT Draw_Collider();




protected:



protected:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free();
};

END