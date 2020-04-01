#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CBlackWolf final : public CGameObject
{
protected:
	explicit CBlackWolf(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBlackWolf(const CBlackWolf& rhs);
	virtual ~CBlackWolf() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();

public:
	static CBlackWolf* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	CBT_Composite_Node* Frisbee(); 	// 1. 프리스비
	CBT_Composite_Node* JumpDodgeL_And_Bite(); // 2. 좌회피 후 물기
	CBT_Composite_Node* JumpDodgeR_And_Bite(); // 3. 우회피 후 물기
	CBT_Composite_Node* Bite_LeftRightLeft(); 	// 4. 좌 우 좌

	CBT_Composite_Node* Random_Attack(); //4개중 1개의 랜덤 공격
	CBT_Composite_Node* Fov_Check(); //시야각 각도와 거리 체크
	//CBT_Composite_Node* Check_Dist(); //감지
									 

	CBT_Composite_Node* Walk(); //22 걷기 탐색상태
	CBT_Composite_Node* Run(); //0 달리기 추적상태
	CBT_Composite_Node* Dodge(); //22 회피

	CBT_Composite_Node* Death_Normal(); //24 죽음
	CBT_Composite_Node* Death_Strong(); //23 강하게 죽음
	CBT_Composite_Node* Dmg_Front(); //25 앞에서 맞음
	CBT_Composite_Node* Dmg_Back(); //26 뒤에서 맞음
	CBT_Composite_Node* Hit_DownStrong(); //19 20 21 강하게 날아감
	CBT_Composite_Node* Hit_DownWeak(); //16 17 18 약하게 날아감

	CBT_Composite_Node* Idle_Eat(); //14 11 15 식사 도중 발견, 경계상태
	CBT_Composite_Node* Idle_Sit(); //12 8 15 앉아서 쉬는 도중 발견, 경계상태 
	CBT_Composite_Node* Idle_Stand(); //서있는 도중 발견, 경계상태

	//Near
	//시야각 이내인 경우 == 4개중 1개 공격
	//시야각 밖인 경우 == 회전

	//Far
	//시야각 이내인 경우 == 추적
	//시야각 밖인 경우 == 일상행동

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;
	_v3					m_vHeadTeeth = _v3(0.f, 0.f, 0.f);	//머리
	_v3					m_vBody = _v3(0.f, 0.f, 0.f);	//척추


};

END