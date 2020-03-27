
// EffectToolView.h : CEffectToolView 클래스의 인터페이스
//

#pragma once

namespace Engine
{
	class CGameObject;
	class CRenderer;
}

class CTerrain_Guide;
class CMainFrame;
class CParticleTab;
class CEffectToolDoc;
class CEffectToolView : public CView
{
protected: // serialization에서만 만들어집니다.
	CEffectToolView();
	DECLARE_DYNCREATE(CEffectToolView)

// 특성입니다.
public:
	CEffectToolDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnInitialUpdate();

// 구현입니다.
public:
	virtual ~CEffectToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()
//===========================================================
public:
	void Update();
	void Render();


private:
	void Ready_Default();
	void Ready_Camera();
	void Ready_Layer();
	void Ready_Resource();
	void Release();


private:
	LPDIRECT3DDEVICE9			m_pGraphicDev = nullptr;
	Engine::CGameObject*		m_pTerrainGuide = nullptr;
	CRenderer*					m_pRenderer = nullptr;

private:
	CMainFrame*					m_pMainFrm;
	CParticleTab*				m_pParticleTab;

};

#ifndef _DEBUG  // EffectToolView.cpp의 디버그 버전
inline CEffectToolDoc* CEffectToolView::GetDocument() const
   { return reinterpret_cast<CEffectToolDoc*>(m_pDocument); }
#endif

