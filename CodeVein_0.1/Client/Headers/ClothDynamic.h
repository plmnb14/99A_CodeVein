#pragma once

#include "..\Headers\Cloth.h"

BEGIN(Client)

class CClothDynamic final : public CCloth
{
public:
	enum ClothDynamic_Name {
		Drape, Gauntlet, Muffler, LongCoat
	};

	typedef struct tagInitInfo
	{
		tagInitInfo(ClothDynamic_Name _eName)
			: eName(_eName)
		{}

		ClothDynamic_Name	eName;

	}INFO;

protected:
	explicit CClothDynamic(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CClothDynamic(const CClothDynamic& rhs);
	virtual ~CClothDynamic() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype();
	virtual HRESULT Ready_GameObject(void* pArg);
	virtual _int Update_GameObject(_double TimeDelta);
	virtual _int Late_Update_GameObject(_double TimeDelta);
	virtual HRESULT Render_GameObject();
	virtual HRESULT Render_GameObject_SetPass(CShader* pShader, _int iPass, _bool _bIsForMotionBlur = false);

private:
	void Init_Darpe();
	void Init_Gauntlet();
	void Init_Muffler();
	void Init_LongCoat();

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CMesh_Dynamic*		m_pMeshCom = nullptr;
	COptimization*		m_pOptimizationCom = nullptr;

private:
	HRESULT Add_Component(void* pArg);
	HRESULT SetUp_ConstantTable();

public:
	static CClothDynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone_GameObject(void* pArg);
	virtual void Free();
};

END