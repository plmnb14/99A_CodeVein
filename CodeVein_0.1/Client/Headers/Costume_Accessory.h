#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"

BEGIN(Client)

class CCostume_Accessory : public CGameObject
{

public:
	enum AccType
	{
		AccEnd
	};

public:
	typedef struct tagInitInfo
	{
		tagInitInfo(AccType _eAccType, _mat* pmatParent, _mat* pmatBone, _v4 vColorValue)
			:eAccType(_eAccType), pmatParent(pmatParent), pmatBone(pmatBone), vColorValue(vColorValue)
		{}

		AccType		eAccType = AccEnd;
		_mat*		pmatParent = nullptr;
		_mat*		pmatBone = nullptr;
		_v4			vColorValue = _v4(0.f, 0.f, 0.f, 0.f);
	}_INFO;

private:
	explicit CCostume_Accessory(_Device pGraphicDev);
	explicit CCostume_Accessory(const CCostume_Accessory& rhs);
	virtual ~CCostume_Accessory() = default;

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
	virtual void Change_AccMesh(AccType _eAccType);

private:
	CTransform*				m_pTransform = nullptr;
	CRenderer*				m_pRenderer = nullptr;
	CShader*				m_pShader = nullptr;
	CMesh_Static*			m_pStaticMesh = nullptr;
	CBattleAgent*			m_pBattleAgent = nullptr;

private:
	_mat*					m_pmatParent = nullptr;
	_mat*					m_pmatBone = nullptr;

private:
	AccType					m_eHairType = AccEnd;
	_v4						m_vColorValue = {};

public:
	static CCostume_Accessory* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void free();
};

END