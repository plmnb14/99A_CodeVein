#pragma once

#include "Monster.h"

BEGIN(Client)

class CWeapon;
class CBossHP;
class CQueensKnight final : public CMonster
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_float _fFov, _float _fMaxLength, _float _fMinLength)
			: fFov(_fFov), fMaxLength(_fMaxLength), fMinLength(_fMinLength)
		{}

		_float					fFov = 0.f;
		_float					fMaxLength = 0.f;
		_float					fMinLength = 0.f;
	}INFO;

private:
	enum Ani {
		Ani_Idle = 15, Ani_Death = 24, Ani_Appearance = 26, Ani_Down_Start = 16, Ani_Down_Loop = 17, Ani_Down_End = 18,
	
	};

private:
	enum BoneMatrix { Bone_Range, Bone_Body, Bone_Head, Bone_End };

protected:
	explicit CQueensKnight(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CQueensKnight(const CQueensKnight& rhs);
	virtual ~CQueensKnight() = default;

public:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass);

private:	// 패턴들
	// 1. 칼 가로로 휘두르기
	CBT_Composite_Node* Normal_HorizontalCut1();	// 충돌완
	// 2. 칼 세로로 내려치기
	CBT_Composite_Node* Normal_VerticalCut1();		//충돌완
	// 3. 칼 두번 휘두르기
	CBT_Composite_Node* TwoCombo_Cut();	//충돌완
	// 4. 칼 두번 휘두르고 방패치기
	CBT_Composite_Node* ThreeCombo_Cut();	// 충돌완
	// 5. 뒤로 빠지면서 공격
	CBT_Composite_Node* BackStep_Cut();	// 충돌완
	// 6. 찌르기
	CBT_Composite_Node* Sting();	// 충돌완
	// 7. 회전 날개치기
	CBT_Composite_Node* Wing_Attack();	//충돌완
	// 8. 돌진
	CBT_Composite_Node* Rush();	//충돌완
	// 9. 방패치기
	CBT_Composite_Node* Shield_Attack();	//충돌완

	// 10. 리크 필드
	CBT_Composite_Node* LeakField();

	// 점멸 패턴
	// 1. 점멸
	CBT_Composite_Node* Flash();

	// 1. 날개치기
	CBT_Composite_Node* Flash_Wing_Attack();	//충돌완
	// 2. 돌진
	CBT_Composite_Node* Flash_Rush();	// 충돌완
	// 3. 위에서 내려찍기
	CBT_Composite_Node* Flash_Jump_Attack();	// 충돌완
	// 4. 한번 베기 
	CBT_Composite_Node* Flash_Cut();	// 충돌완
	// 5. 점멸 후 중앙으로 내려치기
	CBT_Composite_Node* Flash_Middle_Ground();	// 이펙트 넣어야함


	///////////// 응용
	CBT_Composite_Node* Smart_ThreeCombo_Cut();
	CBT_Composite_Node* Create_LeakField_Or_Not();


	//// 게임 시작
	CBT_Composite_Node* Start_Game();

	// 체력 70퍼 이상	// 점멸 없음
	CBT_Composite_Node* More_Than_HP_70();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP70();
	CBT_Composite_Node* FarAttack_More_Than_HP70();

	// 체력 70퍼 미만	// 점멸 추가
	CBT_Composite_Node* More_Than_HP_40();
	CBT_Composite_Node* NearAttack_Dist5_More_Than_HP40();
	CBT_Composite_Node* FarAttack_More_Than_HP40();

	// 체력 40퍼 미만	// 점멸 자주 씀
	CBT_Composite_Node* HP_Final();
	CBT_Composite_Node* NearAttack_Dist5_Final();
	CBT_Composite_Node* FarAttack_Fianl();

	//////// 시연회용
	CBT_Composite_Node* Start_Show();
	CBT_Composite_Node* Show_ChaseAndNearAttack();
	CBT_Composite_Node* Show_NearAttack();	// 7개
	CBT_Composite_Node* Show_FarAttack();	// 5개, 일반돌진 포함


private:
	void	Down();		// 체력 70퍼 이하면 다운

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	CAIController*		m_pAIControllerCom = nullptr;
	CNavMesh*			m_pNavMesh = nullptr;
	CCollider*			m_pCollider = nullptr;

	CWeapon*			m_pSword = nullptr;
	CWeapon*			m_pShield = nullptr;

	// 피격가능 현재 시간
	_double				m_dHitTime = 0;

	//렌더에서 타임델타 쓰기위해서 저장해놓음
	_double				m_dTimeDelta = 0;

	_bool				m_bFindPlayer = false;	// 플레이어 발견 못한 상태
	_bool				m_bFight = false;

private:	// UI(지원)
	CBossHP*			m_pBossUI = nullptr;

private:	// 다운 상태를 위한 변수
	_bool				m_bDown_Start = false;

	_bool				m_bDown_StartAni = false;
	_bool				m_bDown_LoopAni = false;
	_bool				m_bDown_EndAni = false;
	_double				m_dDownTime = 0;

	_byte				m_iDownCount = 0;

private:
	// 충돌체를 위한 뼈다귀
	_mat*				m_matBones[Bone_End];
	_bool				m_bAIController = true;

	// 블랙보드에서 뼈의 Pos 저장소
	_v3					m_vWing = _v3(0.f, 0.f, 0.f);

private:	// 최초상태 세팅
	_float				m_fFov = 0.f;
	_float				m_fMaxLength = 0.f;
	_float				m_fMinLength = 0.f;

private:
	_bool				m_bPlayerFriendly = false;		// 플레이어 껀지

private:
	HRESULT Update_Bone_Of_BlackBoard();
	HRESULT Update_Value_Of_BB();

	HRESULT Update_NF();
	HRESULT Update_Collider();

private:
	void Check_PhyCollider();
	void Push_Collider();

private:
	void OnCollisionEnter();
	void OnCollisionEvent(list<CGameObject*> plistGameObject);

private:
	HRESULT Add_Component();
	HRESULT SetUp_ConstantTable();

	HRESULT Ready_Weapon();
	HRESULT Ready_BoneMatrix();
	HRESULT Ready_Collider();
	HRESULT Ready_NF(void* pArg);

public:
	static CQueensKnight* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END