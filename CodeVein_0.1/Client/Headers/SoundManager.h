#pragma once

#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CSoundManager : public CBase
{
	DECLARE_SINGLETON(CSoundManager)

public:
	enum CHANNELID 
	{ 
	Ambient_01, Ambient_02, Ambient_03,
	Background_01, Background_02,
	//===============================================================
	Genji_SFX_01, Genji_SFX_02, Genji_Voice,
	Wolf_SFX_01, Wolf_SFX_02, Wolf_Voice,
	Yacha_SFX_01, Yacha_SFX_02, Yacha_Voice,
	Cocoon_SFX_01, Cocoon_SFX_02, Cocoon_Voice,
	Monkey_SFX_01, Monkey_SFX_02, Monkey_Voice,
	Hunter_SFX_01, Hunter_SFX_02, Hunter_Voice,
	Yeti_SFX_01, Yeti_SFX_02, Yeti_Voice,
	SpikeBall_SFX_01, SpikeBall_SFX_02, SpikeBall_Voice,
	//===============================================================
	Butterfly_SFX_01, Butterfly_SFX_02, ButterFly_Voice,
	DearKing_SFX_01, DearKing_SFX_02, DearKing_Voice,
	FireBoy_SFX_01, FireBoy_SFX_02, FireBoy_Voice,
	IceGirl_SFX_01, IceGirl_SFX_02, IceGirl_Voice,
	QueenKnight_SFX_01, QueenKnight_SFX_02, QueenKnight_Voice,
	//===============================================================
	Player_SFX_01, Player_SFX_02, Player_SFX_03, Player_Voice,
	//===============================================================
	Jack_SFX_01, Jack_SFX_02, Jack_SFX_03, Jack_Voice,
	//===============================================================
	Pet_SFX_01, Pet_SFX_02, Pet_SFX_03, Pet_Voice,
	//===============================================================
	NPC_Voice_01, NPC_Voice_02, NPC_Voice_03,
	//===============================================================
	Effect_SFX_01, Effect_SFX_02, Effect_SFX_03,
	//===============================================================
	Obj_SFX_01, Obj_SFX_02, Obj_SFX_03,
	//===============================================================
	UI_SFX_01, UI_SFX_02, UI_SFX_03,
	//===============================================================
	MAX_CHANNEL
	};

public:
	typedef enum tagSoundOption
	{
		BGM_Sound, Voice_Sound, Effect_Sound, Ambient_Sound,
		End_Sound,
	}SOUND;

	typedef enum tagVolumeCtrl
	{
		BGM_Volume, Voice_Volume, Effect_Volume, Ambien_Volume, Master_Volume,
		End_Volume,
	}VOLUME;

private:
	explicit	CSoundManager();
	virtual		~CSoundManager() = default;

public:
	virtual _float Get_Volume(VOLUME _eChannel = Master_Volume) { return m_fVolumeChannel[_eChannel]; }
	virtual _float Get_Pitch(VOLUME _eChannel = Master_Volume) { return m_fPitchChannel[_eChannel]; }

public:
	virtual void Set_Volume(VOLUME _eChannel = Master_Volume, _float _fVolume = 1.f) { m_fVolumeChannel[_eChannel] = _fVolume; }
	virtual void Set_Pitch(VOLUME _eChannel = Master_Volume, _float _fPitch = 1.f) { m_fPitchChannel[_eChannel] = _fPitch; }

public: 
	virtual void Play_Sound(TCHAR* pSoundKey, CHANNELID eID, SOUND _eSoundGroup);
	virtual void Play_BGM(TCHAR* pSoundKey);
	virtual void Stop_Sound(CHANNELID eID);
	virtual void Stop_All();
	virtual void Update_SoundManager();

public:
	virtual HRESULT Ready_SoundManager();

public:
	virtual _ulong	Get_CurCnt() { return m_dwSoundCnt_Cur; };
	virtual _ulong	Get_MaxCnt() { return m_dwSoundCnt_Max; };
	virtual _bool	Is_Finished() { return m_bIsFinished; }

public:
	virtual void Load_Directory_SouneFile_W(_tchar* _szFolderName);

public:
	virtual void Free();

private:
	// FMOD_SOUND 리소스 정보를 갖는 객체
	map<TCHAR*, FMOD_SOUND*> m_mapSound;

	// 재생하고 있는 사운드를 관리할 객체 
	FMOD_CHANNEL* m_pChannelArr[MAX_CHANNEL];

	FMOD_CHANNELGROUP* m_pChannelGroup[3];

	FMOD_CHANNELGROUP* m_pMasterChannelGroup;

	//사운드, 채널 객체 및 장치를 관리할 객체. 
	FMOD_SYSTEM* m_pSystem = nullptr;

	_float m_fVolumeChannel[End_Sound] = {};		// 음량
	_float m_fPitchChannel[End_Sound] = {};		// 재생 속도

private:
	_ulong	m_dwSoundCnt_Cur = 0;
	_ulong	m_dwSoundCnt_Max = 0;

private:
	_bool	m_bIsFinished = false;
};

END