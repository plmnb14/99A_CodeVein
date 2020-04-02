#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CWeapon;
class CSwordGenji final : public CGameObject
{
protected:
	explicit CSwordGenji(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CSwordGenji(const CSwordGenji& rhs);
	virtual ~CSwordGenji() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

private:	//패턴들
	// 근거리
	// 1. 일반베기1, 이동거리 : 1
	CBT_Composite_Node* Normal_Cut1();
	// 2. 일반베기2, 이동거리 : 1
	CBT_Composite_Node* Normal_Cut2();
	// 3. 일반베기3, 이동거리 : 1
	CBT_Composite_Node* Normal_Cut3();
	// 4. 강하게 오른쪽으로 베기, 이동거리 : 1
	CBT_Composite_Node* Strong_RightCut();
	// 5. 강하게 왼쪽으로 베기, 이동거리 : 1
	CBT_Composite_Node* Strong_LeftCut();
	// 6. 3연속 베기, 이동거리 : 1
	CBT_Composite_Node* ThreeCombo_Cut();
	// 7. 수평베기, 이동거리 : 1
	CBT_Composite_Node* Horizon_Cut();
	// 8. 찌르기, 이동거리 : 1
	CBT_Composite_Node* Sting_Attack();
	// 9. 2연속 베기, 이동거리 : 1 + 1
	CBT_Composite_Node* TwoCombo_Cut();


	// 원거리
	// 1. 던지기1
	CBT_Composite_Node* Throwing1();
	// 2. 던지기2
	CBT_Composite_Node* Throwing2();
	// 3. 백스탭 던지기, 이동거리 : 1
	CBT_Composite_Node* Throwing_BackStep();
	// 4. 달리면서 직선베기, 이동거리 : 7
	CBT_Composite_Node* Run_Straight_Cut();


	CBT_Composite_Node*		Start_Game();

	CBT_Composite_Node*		RotationAndNearAttack();
	CBT_Composite_Node*		ChaseAndNearAttack();
	CBT_Composite_Node*		Chase();
	CBT_Composite_Node*		NearAttack();	// 랜덤 근거리 공격

	CBT_Composite_Node*		LookPlayer_FarAttack();	//플레이어 바라본 후 랜덤 원거리 공격
	CBT_Composite_Node*		FarAttack();	// 랜덤 원거리 공격

	////////////// 시연회용

	CBT_Composite_Node* Show_ChaseAndNeqrAttack();

	CBT_Composite_Node* Show_NearAttack();


	//////////////

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	CWeapon*			m_pSword = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();
	HRESULT Ready_Weapon();

public:
	static CSwordGenji* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END