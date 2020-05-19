#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "ClothManager.h"

BEGIN(Client)

class CCostume_Outer : public CGameObject
{
public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_mat* pmatParent, _mat* pmatBone, _v4 vColorValue, CGameObject* pTarget)
			: pmatParent(pmatParent), pmatBone(pmatBone), vColorValue(vColorValue), pTarget(pTarget)
		{}

		_mat*			pmatParent = nullptr;
		_mat*			pmatBone = nullptr;
		_v4				vColorValue = _v4(0.f, 0.f, 0.f, 0.f);
		CGameObject*	pTarget = nullptr;
	}_INFO;

private:
	explicit CCostume_Outer(_Device pGraphicDev);
	explicit CCostume_Outer(const CCostume_Outer& rhs);
	virtual ~CCostume_Outer() = default;

public:
	CClothManager::Cloth_Dynamic Get_OuterType() { return m_eOuterType; }

public:
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatBone = _matAttachBone; }
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }

	void					SetUp_Animation(_uint iIndex, _bool _bOffLerp);

public:
	virtual _int	Update_GameObject(_double TimeDelta);
	_int	Update_GameObject(_double TimeDelta, _bool bClearCol);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

public:
	virtual void Set_LowerAnimation(_ulong _dwAnimIdx, _bool _bOffLerp);
	virtual void Set_UpperAnimation(_ulong _dwAnimIdx, _bool _bOffLerp);
	virtual void Set_LeftArmAnimation(_ulong _dwAnimIdx, _bool _bOffLerp);
	virtual void Set_RightArmAnimation(_ulong _dwAnimIdx, _bool _bOffLerp);

	virtual void Set_AnimMultiply(_float _float) { m_fAnimMultiply = _float; };

	virtual void Reset_OldAniIdx(_ulong _dwNumber);

private:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT Add_Components();
	virtual HRESULT Setup_Default();
	virtual HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	virtual void Calc_AttachBoneTransform();

public:
	virtual void Change_OuterMesh(CClothManager::Cloth_Dynamic _eOuterTag);

private:
	void Change_Vertex();

private:
	CTransform*						m_pTransform = nullptr;
	CRenderer*						m_pRenderer = nullptr;
	CShader*						m_pShader = nullptr;
	CMesh_Dynamic*					m_pDynamicMesh = nullptr;
	CBattleAgent*					m_pBattleAgent = nullptr;

private:
	_mat*							m_pmatParent = nullptr;
	_mat*							m_pmatBone = nullptr;

private:
	CClothManager::Cloth_Dynamic	m_eOuterType = CClothManager::None;
	_v4								m_vColorValue = {};

private:
	_float					m_fAnimMultiply = 0.f;
	_bool					m_bLerpOff = false;
	_uint					m_eAnimUpper = 0;
	_uint					m_eAnimLower = 0;
	_uint					m_eAnimRight = 0;
	_uint					m_eAnimLeft = 0;

public:
	static CCostume_Outer* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void free();
};

END