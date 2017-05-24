#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		

class CCreatebgApp : public CWinAppEx
{
public:
	CCreatebgApp();

	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	DECLARE_MESSAGE_MAP()

private:
	ULONG_PTR m_gdiplusToken;


	private:
	void    LoadImage(CString strPath);
	void    SavePng(Bitmap* OriginBmp, CString strPath, int nType);
	void    SavePng(Bitmap* OriginBmp, CString strPath);
	double  colorDist (Gdiplus::Color clColor1, Gdiplus::Color clColor2);
	int     CompareImage(Bitmap* OriginBmp, Bitmap* NewBmp);
	int     GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
	CString GetModulePath();
	CString GetFileName(CString strFilename);
};

extern CCreatebgApp theApp;