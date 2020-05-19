#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CCostume_Head : public CGameObject
{
public:
	typedef enum tagHeadType
	{
		Chara_Head_01, Chara_Head_02, Chara_Head_03, Chara_Head_04, Chara_Head_05,
		Chara_Head_06, Chara_Head_07, Chara_Head_08, Chara_Head_09, Chara_Head_10,
		Chara_Head_End
	}CHAR_HEAD;

public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_mat* pmatParent, _mat* pmatBone, _v4 vColorValue, CGameObject* pTarget, CHAR_HEAD _eHairTag)
			: pmatParent(pmatParent), pmatBone(pmatBone), vColorValue(vColorValue), pTarget(pTarget), eHeadTag(_eHairTag)
		{}

		CHAR_HEAD		eHeadTag = Chara_Head_End;
		_mat*			pmatParent = nullptr;
		_mat*			pmatBone = nullptr;
		_v4				vColorValue = _v4(0.f, 0.f, 0.f, 0.f);
		CGameObject*	pTarget = nullptr;
	}_INFO;

private:
	explicit CCostume_Head(_Device pGraphicDev);
	explicit CCostume_Head(const CCostume_Head& rhs);
	virtual ~CCostume_Head() = default;

public:
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatBone = _matAttachBone; }
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }

	void					SetUp_Animation(_uint iIndex, _bool _bOffLerp);
	void					Change_HeadMesh(CHAR_HEAD _eHeadType);

public:
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);


private:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT Add_Components(const _tchar * pMeshName);
	virtual HRESULT Setup_Default();
	virtual HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	virtual void Calc_AttachBoneTransform();


private:
	CTransform*						m_pTransform = nullptr;
	CRenderer*						m_pRenderer = nullptr;
	CShader*						m_pShader = nullptr;
	CMesh_Static*					m_pStaticMesh = nullptr;
	CBattleAgent*					m_pBattleAgent = nullptr;

private:
	_mat*							m_pmatParent = nullptr;
	_mat*							m_pmatBone = nullptr;

private:
	CHAR_HEAD						m_eHeadType = Chara_Head_01;
	_v4								m_vColorValue = {};

private:
	_float					m_fAnimMultiply = 0.f;
	_bool					m_bLerpOff = false;
	_uint					m_eAnimUpper = 0;
	_uint					m_eAnimLower = 0;
	_uint					m_eAnimRight = 0;
	_uint					m_eAnimLeft = 0;

private:
	_ulong					m_dwDebugValue = 0;

public:
	static CCostume_Head* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void free();
};

END