#ifndef PathExtract_h__
#define PathExtract_h__

class CPathExtract
{
public:
	CPathExtract();
	virtual ~CPathExtract();

public:
	void MakePathFile();
	void ExportFile();

	void Release();

private:
	void OnBnClickedSave(bool _bForClient = false);
	void OnBnClickedLoad();

public:
	list<ENGINE::PATH_INFO*>	m_PathInfoLst_Multi;
	list<ENGINE::PATH_INFO*>	m_PathInfoLst_Single;
};

#endif // !PathExtract_h__
