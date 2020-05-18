#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CCostume_Body: public CGameObject
{
public:
	typedef enum tagBodyType
	{
		Chara_Body_01, Chara_Body_02, Chara_Body_03, Chara_Body_04,
		Chara_Body_End
	}CHAR_BODY;

public:
	typedef struct tagInitInfo
	{
		tagInitInfo(_mat* pmatParent, _mat* pmatBone, _v4 vColorValue, CGameObject* pTarget, CHAR_BODY _eCharaBody)
			: pmatParent(pmatParent), pmatBone(pmatBone), vColorValue(vColorValue), pTarget(pTarget), eBodyTag(_eCharaBody)
		{}

		CHAR_BODY		eBodyTag = Chara_Body_End;
		_mat*			pmatParent = nullptr;
		_mat*			pmatBone = nullptr;
		_v4				vColorValue = _v4(0.f, 0.f, 0.f, 0.f);
		CGameObject*	pTarget = nullptr;
	}_INFO;

private:
	explicit CCostume_Body(_Device pGraphicDev);
	explicit CCostume_Body(const CCostume_Body& rhs);
	virtual ~CCostume_Body() = default;

public:
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatBone = _matAttachBone; }
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }

	void					SetUp_Animation(_uint iIndex, _bool _bOffLerp);

public:
	virtual _int	Update_GameObject(_double TimeDelta);
	_int	Update_GameObject(_double TimeDelta, _bool bSkill);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
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
	CMesh_Dynamic*					m_pDynamicMesh = nullptr;
	CBattleAgent*					m_pBattleAgent = nullptr;

private:
	_mat*							m_pmatParent = nullptr;
	_mat*							m_pmatBone = nullptr;

private:
	CHAR_BODY						m_eBodyType = Chara_Body_01;
	_v4								m_vColorValue = {};

private:
	_float					m_fAnimMultiply = 0.f;
	_bool					m_bLerpOff = false;
	_uint					m_eAnimUpper = 0;
	_uint					m_eAnimLower = 0;
	_uint					m_eAnimRight = 0;
	_uint					m_eAnimLeft = 0;

private:
	_float					m_fAnimMutiply = 0.f;

public:
	static CCostume_Body* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void free();
};

END