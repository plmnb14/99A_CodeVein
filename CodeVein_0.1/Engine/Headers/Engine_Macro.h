#ifndef Engine_Macro_h__
#define Engine_Macro_h__

#define			USING(NAMESPACE)	using namespace NAMESPACE;

#define			MSG_BOX(_message)			MessageBox(NULL, TEXT(_message), L"System Message", MB_OK)
#define			TAGMSG_BOX(_tag,_message)	MessageBoxW(NULL, _message, _tag, MB_OK)

#define			BEGIN(NAMESPACE)		namespace NAMESPACE {
#define			END						}

#ifdef ENGINE_EXPORTS
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif // ENGINE_EXPORTS

#define NO_COPY(CLASSNAME)									\
	private:												\
	CLASSNAME(const CLASSNAME&);							\
	CLASSNAME& operator = (const CLASSNAME&);				

#define DECLARE_SINGLETON(CLASSNAME)						\
	NO_COPY(CLASSNAME)										\
	private:												\
	static CLASSNAME*	m_pInstance;						\
	public:													\
	static CLASSNAME*	Get_Instance( void );				\
	static unsigned long Destroy_Instance( void );			

#define IMPLEMENT_SINGLETON(CLASSNAME)						\
	CLASSNAME*	CLASSNAME::m_pInstance = NULL;				\
	CLASSNAME*	CLASSNAME::Get_Instance( void )	{			\
		if(nullptr == m_pInstance) {						\
			m_pInstance = new CLASSNAME;					\
		}													\
		return m_pInstance;									\
	}														\
	unsigned long CLASSNAME::Destroy_Instance( void ) {		\
		unsigned long dwRefCnt = 0;							\
		if(nullptr != m_pInstance)	{						\
			dwRefCnt = m_pInstance->Release();				\
			if(0 == dwRefCnt)								\
			m_pInstance = nullptr;							\
		}													\
		return dwRefCnt;									\
	}


#endif // Engine_Macro_h__
