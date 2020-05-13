#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "Management.h"
#include "ClothManager.h"

BEGIN(Client)

class CCostume_Hair : public CGameObject
{

public:
	enum HairType
	{
		Hair_01, Hair_02, Hair_03, Hair_04, Hair_05, Hair_06, Hair_07,
		Hair_End
	};

public:
	typedef struct tagInitInfo
	{
		tagInitInfo(HairType _eHairType , _mat* pmatParent, _mat* pmatBone, _v4 vColorValue)
			:eHairType(_eHairType) , pmatParent(pmatParent) , pmatBone(pmatBone) , vColorValue(vColorValue)
		{}

		HairType	eHairType = Hair_01;
		_mat*		pmatParent = nullptr;
		_mat*		pmatBone = nullptr;
		_v4			vColorValue = _v4(0.f, 0.f, 0.f, 0.f);
	}_INFO;

private:
	explicit CCostume_Hair(_Device pGraphicDev);
	explicit CCostume_Hair(const CCostume_Hair& rhs);
	virtual ~CCostume_Hair() = default;

public:
	virtual void			Set_AttachBoneMartix(_mat* _matAttachBone) { m_pmatBone = _matAttachBone; }
	virtual void			Set_ParentMatrix(_mat* _matParent) { m_pmatParent = _matParent; }

public:
	virtual _int	Update_GameObject(_double TimeDelta);
	virtual _int	Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_Instancing_SetPass(CShader* pShader);
	virtual HRESULT Render_GameObject_SetPass(CShader * pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual HRESULT Add_Components();
	virtual HRESULT Setup_Default();
	virtual HRESULT SetUp_ConstantTable(CShader* pShader);

private:
	virtual void Calc_AttachBoneTransform();
	virtual void Change_HairMesh(HairType _eHairType);

private:
	void Change_Vertex();

private:
	CTransform*					m_pTransform = nullptr;
	CRenderer*					m_pRenderer = nullptr;
	CShader*					m_pShader = nullptr;
	CMesh_Static*				m_pStaticMesh = nullptr;
	CBattleAgent*				m_pBattleAgent = nullptr;

private:
	_mat*						m_pmatParent = nullptr;
	_mat*						m_pmatBone = nullptr;

private:
	HairType					m_eHairType = Hair_01;
	_v4							m_vColorValue = {};

private:
	CClothManager::Cloth_Static	m_eHairTag = CClothManager::Hair01;

public:
	static CCostume_Hair* Create(_Device pGraphicDev);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void free();
};

END