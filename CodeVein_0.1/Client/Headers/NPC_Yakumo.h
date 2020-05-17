#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)
class CNPC_InteractionUI;
class CScriptUI;
class CWeaponShopUI;
class CNPC_Yakumo final : public CGameObject
{
public:
	typedef struct tagNPCInfo
	{
		tagNPCInfo(_v3 vPos, _float fYAngle)
			: vPos(vPos) , fYAngle(fYAngle)
		{}

		_v3 vPos = {};
		_float fYAngle = 0.f;

	}NPC_INFO;

public:
	enum NPC_ANI
	{
		Shrug,		// ¼Õ³»¹Ð¸ç À¸¾³
		Positive,	// ²ô´ö
		Negative,	// Àý·¹Àý·¹
		LookDown,
		Gloomy,
		Idle,
	};

	enum BONE_TYPE
	{
		Bone_Range,
		Bone_Body,
		Bone_Head,
		Bone_End
	};

protected:
	explicit CNPC_Yakumo(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CNPC_Yakumo(const CNPC_Yakumo& rhs);
	virtual ~CNPC_Yakumo() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT LateInit_GameObject();
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);
private:
	void Update_Collider();
	void Render_Collider();

	void Check_Dist();
	void Check_Anim();
	void Check_Bye();

private:
	HRESULT Add_Component(void * pArg);
	HRESULT SetUp_ConstantTable(CShader * pShader);
	HRESULT Ready_Collider();
	HRESULT Ready_BoneMatrix();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CCollider*			m_pColliderCom = nullptr;
	CBattleAgent*		m_pBattleAgentCom = nullptr;
	COptimization*		m_pOptimizationCom = nullptr;

public:
	static CNPC_Yakumo* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	CWeaponShopUI*			m_pWeaponShopUI = nullptr;
	CScriptUI*				m_pScriptUI = nullptr;
	CNPC_InteractionUI*		m_pInteractionButton = nullptr;

	_mat*					m_matBone[Bone_End];
	NPC_ANI					m_eState;

	CGameObject*			m_pPlayer = nullptr;

	_double					m_dAniPlayMul = 0.0;

	_bool					m_bCanActive = false;
	_bool					m_bActive = false;
	_bool					m_bByeCheck = false;
};

END