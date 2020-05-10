#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CCostume_Outer : public CGameObject
{

public:
	enum OuterType
	{
		OuterNone, Outer_Drape, Outer_LongCoat, Outer_Gauntlet, Outer_Muffler,
		Outer_End
	};

public:
	typedef struct tagInitInfo
	{
		tagInitInfo(OuterType _eOuterType, _mat* pmatParent, _mat* pmatBone, _v4 vColorValue, CGameObject* pTarget)
			:eOuterType(_eOuterType), pmatParent(pmatParent), pmatBone(pmatBone), vColorValue(vColorValue), pTarget(pTarget)
		{}

		OuterType		eOuterType = OuterNone;
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
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatBone = _matAttachBone; }
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }

public:
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT Add_Components();
	virtual HRESULT Setup_Default();
	virtual HRESULT SetUp_ConstantTable();

private:
	virtual void Calc_AttachBoneTransform();
	virtual void Change_OuterMesh(OuterType _eOuterType);

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Dynamic*			m_pDynamicMesh = nullptr;
	CBattleAgent*			m_pBattleAgent = nullptr;

private:
	_mat*					m_pmatParent = nullptr;
	_mat*					m_pmatBone = nullptr;

private:
	OuterType				m_eHairType = OuterNone;
	_v4						m_vColorValue = {};

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