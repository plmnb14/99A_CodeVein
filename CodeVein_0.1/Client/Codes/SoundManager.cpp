#include "stdafx.h"
#include <CString>
#include "SoundManager.h"

IMPLEMENT_SINGLETON(CSoundManager)

CSoundManager::CSoundManager()
{
	m_pSystem = nullptr;
}

void CSoundManager::Add_Volume(VOLUME _eChannel, _float _fVolume)
{
	if (m_fVolumeChannel[_eChannel] <= m_fMaxVolume)
	{
		m_fVolumeChannel[_eChannel] += _fVolume;

		if (m_fVolumeChannel[_eChannel] > m_fMaxVolume)
		{
			m_fVolumeChannel[_eChannel] = m_fMaxVolume;
		}

		else if (m_fVolumeChannel[_eChannel] < 0.f)
		{
			m_fVolumeChannel[_eChannel] = 0.f;
		}
	}
}

void CSoundManager::Play_Sound(TCHAR * pSoundKey, CHANNELID eID, SOUND _eSoundGroup)
{
	if (m_mapSound.empty())
		return;

	if (_eSoundGroup == BGM_Sound)
		return;

	if (_eSoundGroup == Ambient_Sound)
		return;

	auto& iter = find_if(m_mapSound.begin(), m_mapSound.end(), CMyStrCmp(pSoundKey));

	if (m_mapSound.end() == iter)
		return;

	FMOD_BOOL IsPlay;
	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &IsPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, FALSE,&m_pChannelArr[eID]);
		FMOD_Channel_SetChannelGroup(m_pChannelArr[eID], m_pChannelGroup[_eSoundGroup]);
	}
	FMOD_System_Update(m_pSystem);
}

void CSoundManager::Play_BGM(TCHAR * pSoundKey,  _bool _bIsAmbient)
{
	return;

	if (m_mapSound.empty())
		return;

	auto& iter = find_if(m_mapSound.begin(), m_mapSound.end(), CMyStrCmp(pSoundKey));

	if (m_mapSound.end() == iter)
		return;

	if (false == _bIsAmbient)
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, false, &m_pChannelArr[Background_Loop]);
		FMOD_Channel_SetChannelGroup(m_pChannelArr[Background_Loop], m_pChannelGroup[BGM_Sound]);

		FMOD_Channel_SetMode(m_pChannelArr[Background_Loop], FMOD_LOOP_NORMAL);
	}

	else
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, false, &m_pChannelArr[Ambient_Loop]);
		FMOD_Channel_SetChannelGroup(m_pChannelArr[Ambient_Loop], m_pChannelGroup[Ambient_Sound]);

		FMOD_Channel_SetMode(m_pChannelArr[Ambient_Loop], FMOD_LOOP_NORMAL);
	}
}

void CSoundManager::Play_BGM_Type(TCHAR * pSoundKey, _bool _bIsAmbient)
{
	return;

	if (m_mapSound.empty())
		return;

	auto& iter = find_if(m_mapSound.begin(), m_mapSound.end(), CMyStrCmp(pSoundKey));

	if (m_mapSound.end() == iter)
		return;

	if (false == _bIsAmbient)
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, false, &m_pChannelArr[Background_Loop]);
		FMOD_Channel_SetChannelGroup(m_pChannelArr[Background_Loop], m_pChannelGroup[BGM_Sound]);

		FMOD_Channel_SetMode(m_pChannelArr[Background_Loop], FMOD_LOOP_NORMAL);
	}

	else
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, FALSE, false, &m_pChannelArr[Ambient_Loop]);
		FMOD_Channel_SetChannelGroup(m_pChannelArr[Ambient_Loop], m_pChannelGroup[Ambient_Sound]);

		FMOD_Channel_SetMode(m_pChannelArr[Ambient_Loop], FMOD_LOOP_NORMAL);
	}
}

void CSoundManager::Stop_Sound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);

}

void CSoundManager::Stop_All()
{
	for (int i = 0; i < MAX_CHANNEL; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundManager::Update_SoundManager()
{
	FMOD_ChannelGroup_SetPitch(m_pChannelGroup[0], m_fPitchChannel[Master_Volume] * m_fPitchChannel[BGM_Sound]);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[0], m_fVolumeChannel[Master_Volume] * m_fVolumeChannel[BGM_Sound]);

	FMOD_ChannelGroup_SetPitch(m_pChannelGroup[1], m_fPitchChannel[Master_Volume] * m_fPitchChannel[Voice_Sound]);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[1], m_fVolumeChannel[Master_Volume] * m_fVolumeChannel[Voice_Sound]);

	FMOD_ChannelGroup_SetPitch(m_pChannelGroup[2], m_fPitchChannel[Master_Volume] * m_fPitchChannel[Effect_Sound]);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[2], m_fVolumeChannel[Master_Volume] * m_fVolumeChannel[Effect_Sound]);

	FMOD_ChannelGroup_SetPitch(m_pChannelGroup[3], m_fPitchChannel[Master_Volume] * m_fPitchChannel[Ambient_Sound]);
	FMOD_ChannelGroup_SetVolume(m_pChannelGroup[3], m_fVolumeChannel[Master_Volume] * m_fVolumeChannel[Ambient_Sound]);
}

HRESULT CSoundManager::Ready_SoundManager()
{
	for (_uint i = 0; i < End_Volume; ++i)
	{
		m_fVolumeChannel[i] = 1.f;
		m_fPitchChannel[i] = 1.f;
 	}

	FMOD_System_Create(&m_pSystem);
	FMOD_System_Init(m_pSystem, MAX_CHANNEL, FMOD_INIT_NORMAL, NULL);

	FMOD_System_CreateChannelGroup(m_pSystem, "Group A", &m_pChannelGroup[0]);
	FMOD_System_CreateChannelGroup(m_pSystem, "Group B", &m_pChannelGroup[1]);
	FMOD_System_CreateChannelGroup(m_pSystem, "Group C", &m_pChannelGroup[2]);
	FMOD_System_CreateChannelGroup(m_pSystem, "Group D", &m_pChannelGroup[3]);

	FMOD_System_GetMasterChannelGroup(m_pSystem, &m_pMasterChannelGroup);

	FMOD_ChannelGroup_AddGroup(m_pMasterChannelGroup, m_pChannelGroup[0], false , nullptr);
	FMOD_ChannelGroup_AddGroup(m_pMasterChannelGroup, m_pChannelGroup[1], false, nullptr);
	FMOD_ChannelGroup_AddGroup(m_pMasterChannelGroup, m_pChannelGroup[2], false, nullptr);
	FMOD_ChannelGroup_AddGroup(m_pMasterChannelGroup, m_pChannelGroup[3], false, nullptr);

	return S_OK;
}

void CSoundManager::Load_Directory_SouneFile_W(_tchar * _szFolderName)
{
	// 찾았는지 못찾았는지
	_bool bResult = false;

	// 찾아야하는 데이터
	WIN32_FIND_DATA FindData;

	// 기본 경로명
	_tchar szPathDefault[STR_128] = L"../Sounds/";
	_tchar szExtension[STR_128] = L"/*.*";

	_tchar szFolderFolderName[STR_128] = L"";
	_tchar szFolderFolder[STR_128] = L"";

	// 폴더 + 임의 폴더
	lstrcpy(szFolderFolder, szPathDefault);
	lstrcat(szFolderFolder, _szFolderName);
	lstrcat(szFolderFolder, L"/");

	// 폴더 + 임의 폴더 + 파일
	lstrcpy(szFolderFolderName, szFolderFolder);
	lstrcat(szFolderFolderName, szExtension);

	// 파일 찾아보고
	HANDLE hFind = FindFirstFile(szFolderFolderName, &FindData);

	if (hFind == INVALID_HANDLE_VALUE)
	{
		//cout << "( " << szFolderFolderName << " ) 경로에 파일이 없습니다." << endl;
		return;
	}

	bResult = true;
	
	while (bResult)
	{
		_tchar szSoundName[STR_128] = L"";
	
		lstrcpy(szSoundName, szFolderFolder);
		lstrcat(szSoundName, FindData.cFileName);

		char szSoundName_Muti[STR_128] = "";
		WideCharToMultiByte(CP_UTF8, 0, szSoundName, -1, szSoundName_Muti, sizeof(char) * STR_128, NULL, NULL);
	
		FMOD_SOUND* pSound = nullptr;
	
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szSoundName_Muti, FMOD_DEFAULT, 0, &pSound);
	
		if (eRes == FMOD_OK)
		{
			_tchar* pSoundKey = new _tchar[STR_128];
			lstrcpy(pSoundKey, FindData.cFileName);
	
			m_mapSound.emplace(pSoundKey, pSound);
			//++m_dwSoundCnt_Cur;
		}
	
		bResult = FindNextFile(hFind, &FindData);
	}
	
	FMOD_System_Update(m_pSystem);
}

void CSoundManager::Free()
{
	for (auto& MyPair : m_mapSound)
	{
		delete[] MyPair.first;
		FMOD_Sound_Release(MyPair.second);
	}
	m_mapSound.clear();
	FMOD_System_Release(m_pSystem);
	FMOD_System_Close(m_pSystem);
}
