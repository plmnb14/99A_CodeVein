#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CSwordShieldGenji final : public CGameObject
{
public:
	enum Type { White, Jungle, Normal };

	typedef struct tagInitInfo
	{
		tagInitInfo(CSwordShieldGenji::Type _eType)
			: eType(_eType)
		{}

		CSwordShieldGenji::Type	eType = Normal;

	}INFO;

private:
	enum Ani {
		Ani_Idle = 42,
		Ani_Death = 64,
		Ani_Dmg01_FL = 122
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CSwordShieldGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSwordShieldGenji(const CSwordShieldGenji& rhs);
	virtual ~CSwordShieldGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:	//패턴들
	// 근거리
	// 1. 돌진 어퍼슬래쉬, 이동거리 : 4
	CBT_Composite_Node* Upper_Slash();
	// 2. 긴 딜레이 찌르기, 이동거리 : 1
	CBT_Composite_Node* LongDelay_Sting();
	// 3. 방패치기, 이동거리 : 0.5
	CBT_Composite_Node* Shield_Attack();
	// 4. 한바퀴 돌면서 베기, 이동거리 : 2
	CBT_Composite_Node* Turning_Cut();
	// 5. 짧은 딜레이 찌르기, 이동거리 : 0.6
	CBT_Composite_Node* ShortDelay_Sting();

	// 맞음
	// 1. 방패들고 약하게 맞음
	CBT_Composite_Node* Hit_Weakly();
	// 2. 방패들고 강하게 맞음
	CBT_Composite_Node* Hit_Strongly();

	// 가드
	CBT_Composite_Node* Guard(_double dGuardTime);

	// 방패들고 걷기	플레이어와 거리 계산 후 걷는다.

	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		RotationAndNearAttack();
	CBT_Composite_Node*		Chase_Guard_NearAttack();
	CBT_Composite_Node*		Chase();
	CBT_Composite_Node*		NearAttack();	// 랜덤 근접 공격



private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;

	CWeapon*			m_pSpear = nullptr;
	CWeapon*			m_pShied = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	//뼈다귀
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = false;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_Collider();

private:
	void Check_Collider();

	HRESULT Draw_Collider();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();

public:
	static CSwordShieldGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END