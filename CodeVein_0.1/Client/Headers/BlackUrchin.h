#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CBlackUrchin final : public CGameObject
{
protected:
	explicit CBlackUrchin(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBlackUrchin(const CBlackUrchin& rhs);
	virtual ~CBlackUrchin() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass);

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

public:
	static CBlackUrchin* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END