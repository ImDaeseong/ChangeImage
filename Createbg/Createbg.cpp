#include "stdafx.h"
#include "Createbg.h"
#include "CreatebgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CCreatebgApp, CWinAppEx)
END_MESSAGE_MAP()

CCreatebgApp::CCreatebgApp()
{
	m_gdiplusToken = NULL;
}

CCreatebgApp theApp;

BOOL CCreatebgApp::InitInstance()
{	
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	CString strParams = cmdInfo.m_strFileName;
	if(strParams == "")
		return FALSE;
   
	CoInitialize(NULL);

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, NULL);
	
	LoadImage(strParams);

	return FALSE;
}

int CCreatebgApp::ExitInstance()
{
	if (m_gdiplusToken != NULL)
	{
		GdiplusShutdown(m_gdiplusToken);
		m_gdiplusToken = NULL;
	}

	CoUninitialize();

	return CWinApp::ExitInstance();
}


CStringW CSA2CSW(const CStringA &strA)
{
	if (strA.IsEmpty())
		return L"";

	const char *p = (LPCSTR)strA;
	int outlen = MultiByteToWideChar(CP_ACP, NULL, p, -1, NULL, 0);

	wchar_t *buf = new wchar_t[outlen];
	wmemset(buf, NULL, outlen);

	MultiByteToWideChar(CP_ACP, NULL, p, strA.GetLength(), buf, outlen);
	CStringW strW = buf;
	delete [] buf;

	return strW;
}

inline double round(double x) 
{
	return floor(x + 0.5);
}

Bitmap* RotateLeft(Bitmap* OriginBmp)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetHeight(), OriginBmp->GetWidth());

	for(int i = 0;  i < OriginBmp->GetHeight(); i++) 
	{  
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);
			NewBmp->SetPixel(i, OriginBmp->GetWidth() - j - 1, clColor);
		}
	}
	delete OriginBmp;
	return NewBmp;
}

Bitmap* RotateRight(Bitmap* OriginBmp)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetHeight(), OriginBmp->GetWidth());

	for(int i = 0;  i < OriginBmp->GetHeight(); i++) 
	{  
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);
			NewBmp->SetPixel(OriginBmp->GetHeight() - i - 1, j, clColor);
		}
	}

	delete OriginBmp;
	return NewBmp;
}

Bitmap* InvertXAxis(Bitmap* OriginBmp)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetWidth(), OriginBmp->GetHeight());

	for(int i = 0;  i < OriginBmp->GetHeight(); i++) 
	{  
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);
			NewBmp->SetPixel(j, OriginBmp->GetHeight() - i - 1, clColor);
		}
	}

	delete OriginBmp;
	return NewBmp;
}

Bitmap* ConvertToGrayscale(Bitmap* OriginBmp)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetWidth(), OriginBmp->GetHeight());

	for(int i = 0;  i < OriginBmp->GetHeight(); i++) 
	{  
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{	
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);

			int nValue = clColor.GetR() * 0.299 + clColor.GetG() * 0.587 + clColor.GetB() * 0.114;
			Color color = Gdiplus::Color(clColor.GetA(), nValue, nValue, nValue);
			NewBmp->SetPixel(j, i, color);
		}
	}

	delete OriginBmp;
	return NewBmp;
}

Bitmap* ConvertToNegative(Bitmap* OriginBmp)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetWidth(), OriginBmp->GetHeight());

	for(int i = 0;  i < OriginBmp->GetHeight(); i++) 
	{  
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);

			int nR = 255 - clColor.GetR();
			int nG = 255 - clColor.GetG();
			int nB = 255 - clColor.GetB();

			Color color = Gdiplus::Color(clColor.GetA(), nR, nG, nB);
			NewBmp->SetPixel(j, i, color);
		}
	}

	delete OriginBmp;
	return NewBmp;
}

Bitmap* Quantize(Bitmap* OriginBmp, int numShades)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetWidth(), OriginBmp->GetHeight());

	int size = 256/numShades;
	int mean = (size - 1)/2;

	for(int i = 0;  i < OriginBmp->GetHeight(); i++) 
	{  
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);

			int nR = (clColor.GetR()/size)*size+mean;
			int nG = (clColor.GetG()/size)*size+mean;
			int nB = (clColor.GetB()/size)*size+mean;

			nR = (nR > 255) ? 255 : nR;
			nG = (nG > 255) ? 255 : nG;
			nB = (nB > 255) ? 255 : nB;

			Color color = Gdiplus::Color(clColor.GetA(), nR, nG, nB);
			NewBmp->SetPixel(j, i, color);
		}
	}

	delete OriginBmp;
	return NewBmp;
}

Bitmap* AdjustColors(Bitmap* OriginBmp, float contrastValue, int brightnessValue)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetWidth(), OriginBmp->GetHeight());

	for(int i = 0; i < OriginBmp->GetHeight(); i++)
	{
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);


			int nR = (int)clColor.GetR()*contrastValue + brightnessValue;
			nR = (nR < 0) ? 0 : nR;
			nR = (nR > 255) ? 255 : nR;

			int nG = (int)clColor.GetG()*contrastValue + brightnessValue;
			nG = (nG < 0) ? 0 : nG;
			nG = (nG > 255) ? 255 : nG;

			int nB = (int)clColor.GetB()*contrastValue + brightnessValue;
			nB = (nB < 0) ? 0 : nB;
			nB = (nB > 255) ? 255 : nB;

			Color color = Gdiplus::Color(clColor.GetA(), nR, nG, nB);
			NewBmp->SetPixel(j, i, color);
		}
	}

	delete OriginBmp;
	return NewBmp;
}

Bitmap* ZoomIn(Bitmap* OriginBmp)
{
	Bitmap* NewBmp = new Bitmap(OriginBmp->GetWidth()*2, OriginBmp->GetHeight()*2);

	for(int i = 0; i < OriginBmp->GetHeight(); i++)
	{
		for(int j = 0; j < OriginBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor;
			OriginBmp->GetPixel(j,i, &clColor);

			NewBmp->SetPixel(2*j, 2*i, clColor);
		}
	}

	for(int i = 0; i < NewBmp->GetHeight(); i += 2)
	{
		for(int j = 1; j < NewBmp->GetWidth() - 1; j += 2)
		{
			Gdiplus::Color clColor1;
			Gdiplus::Color clColor2;
			NewBmp->GetPixel(j-1,i, &clColor1);
			NewBmp->GetPixel(j+1,i, &clColor2);

			float nR = 0.5*(clColor1.GetR() + clColor2.GetR());
			float nG = 0.5*(clColor1.GetG() + clColor2.GetG());
			float nB = 0.5*(clColor1.GetB() + clColor2.GetB());

			Color color = Gdiplus::Color(nR, nG, nB);
			NewBmp->SetPixel(j,i,color);
		}
	}

	for(int i = 1; i < NewBmp->GetHeight() - 1; i += 2)
	{
		for(int j = 0; j < NewBmp->GetWidth(); j++)
		{
			Gdiplus::Color clColor1;
			Gdiplus::Color clColor2;
			NewBmp->GetPixel(j,i-1, &clColor1);
			NewBmp->GetPixel(j,i+1, &clColor2);

			float nR = 0.5*(clColor1.GetR() + clColor2.GetR());
			float nG = 0.5*(clColor1.GetG() + clColor2.GetG());
			float nB = 0.5*(clColor1.GetB() + clColor2.GetB());
			
			Color color = Gdiplus::Color(nR, nG, nB);
			NewBmp->SetPixel(j,i,color);
		}
	}

	delete OriginBmp;
	return NewBmp;
}

Bitmap* ZoomOut(Bitmap* OriginBmp, int divHeight, int divWidth)
{
	Bitmap* NewBmp = new Bitmap( ceil( (double)(OriginBmp->GetWidth()/divWidth) ), ceil( (double)(OriginBmp->GetHeight()/divHeight)) );

	for(int i = 0; i < OriginBmp->GetHeight(); i += divHeight)
    {
		for(int j = 0; j < OriginBmp->GetWidth(); j += divWidth)
        {
			if ((j/divWidth < NewBmp->GetWidth()) && (i/divHeight < NewBmp->GetHeight()))
			{
				float rAvarage = 0;
				float gAvarage = 0;
				float bAvarage = 0;

				for(int k = i; k < i + divHeight; k++)
				{
					for(int l = j; l < j + divWidth; l++)
					{
						if (!((l > OriginBmp->GetWidth()) || (l >OriginBmp->GetHeight())))
						{
							Gdiplus::Color clColor;
							OriginBmp->GetPixel(j,i, &clColor);

							rAvarage = rAvarage + clColor.GetR();
							gAvarage = gAvarage + clColor.GetG();
							bAvarage = bAvarage + clColor.GetB();
						}
					}
				}
            
				rAvarage = rAvarage/(divHeight*divWidth);
				gAvarage = gAvarage/(divHeight*divWidth);
				bAvarage = bAvarage/(divHeight*divWidth);

				Color color = Gdiplus::Color(rAvarage, gAvarage, bAvarage);
				NewBmp->SetPixel(j/divWidth,i/divHeight, color);
			}
        }
    }

	delete OriginBmp;
	return NewBmp;
}

void CCreatebgApp::LoadImage(CString strPath)
{
	Bitmap *OriginBmp;
	Bitmap *NewBmp;

#ifdef _UNICODE
	OriginBmp = Bitmap::FromFile(strPath, TRUE);
#else
	OriginBmp = Bitmap::FromFile(CSA2CSW(strPath), TRUE);
#endif

	NewBmp = OriginBmp;

	if (CompareImage(OriginBmp, NewBmp) > 0)
	{
		//::MessageBox(NULL, _T("error"), _T("CompareImage"), MB_OK);
		return;
	}

	SavePng(OriginBmp, strPath, 1);
	//SavePng(OriginBmp, strPath);
}

void CCreatebgApp::SavePng(Bitmap* OriginBmp, CString strPath, int nType)
{
	Gdiplus::Bitmap *NewBmp = NULL;

	if(nType == 1)
		NewBmp = RotateLeft(OriginBmp);
	else if(nType == 2)
		NewBmp = RotateRight(OriginBmp);
	else if(nType == 3)
		NewBmp = InvertXAxis(OriginBmp);
	else if(nType == 4)
		NewBmp = ConvertToGrayscale(OriginBmp);
	else if(nType == 5)
		NewBmp = ConvertToNegative(OriginBmp);
	else if(nType == 6)
		NewBmp = Quantize(OriginBmp, 1);//Quantize(OriginBmp, 2);//Quantize(OriginBmp, 3);
	else if(nType == 7)
		NewBmp = AdjustColors(OriginBmp, 50, 50);
	else if(nType == 8)
		NewBmp = ZoomIn(OriginBmp);
	else if(nType == 9)
		NewBmp = ZoomOut(OriginBmp, 2, 2);//ZoomOut(OriginBmp, 3, 3);
	
	CString strSavedPath;
	CString strFileName = GetFileName(strPath);
	CString strOnlyName = strFileName.Left(strFileName.ReverseFind('.'));
	strSavedPath.Format("%s\\%s.png", GetModulePath(), strOnlyName); 
   
	int nSizeCount = MultiByteToWideChar(CP_ACP, 0, strSavedPath, -1, NULL, 0);
	WCHAR * pWString = new WCHAR[nSizeCount];
	MultiByteToWideChar(CP_ACP, 0, strSavedPath, -1, pWString, nSizeCount);

	CLSID encoderClsid;
	GetEncoderClsid(L"image/png", &encoderClsid);
	NewBmp->Save(pWString, &encoderClsid, NULL);
}

void CCreatebgApp::SavePng(Bitmap* OriginBmp, CString strPath)
{		
    /*
	Gdiplus::Bitmap *NewBmp = new Gdiplus::Bitmap(OriginBmp->GetWidth(), OriginBmp->GetHeight(), PixelFormat32bppARGB);

	Gdiplus::Color backgroundColor;
	backgroundColor = OriginBmp->GetPixel(0, 0, &backgroundColor);
    const double whitenessThreshold = 0.3;

	for (int y = 0; y < OriginBmp->GetHeight(); y++)
	{
		for (int x = 0; x < OriginBmp->GetWidth(); x++)
        {                    
			Gdiplus::Color color;
			OriginBmp->GetPixel(x, y, &color);
            double whiteness = colorDist(color, backgroundColor);

			NewBmp->SetPixel(x, y, Gdiplus::Color( 255, color.GetR(), color.GetG(), color.GetB()) );
			
            if (whiteness < whitenessThreshold)
            {
                double alpha = (int)round(whiteness / whitenessThreshold);
                if (alpha > 0.0)
                {
                    int fr = (int)round((color.GetR() - (1.0 - alpha) * backgroundColor.GetR()) / alpha);
                    int fg = (int)round((color.GetR() - (1.0 - alpha) * backgroundColor.GetR()) / alpha);
                    int fb = (int)round((color.GetR() - (1.0 - alpha) * backgroundColor.GetR()) / alpha);
                    NewBmp->SetPixel(x, y, Gdiplus::Color(0, fr, fg, fb));
                }
                else
                {
                    NewBmp->SetPixel(x, y, Gdiplus::Color(0, backgroundColor.GetR(), backgroundColor.GetG(), backgroundColor.GetB()));
                }
            }
        }
    }
	
	CString strSavedPath;
	CString strFileName = GetFileName(strPath);
	CString strOnlyName = strFileName.Left(strFileName.ReverseFind('.'));
	strSavedPath.Format("%s\\%s.png", GetModulePath(), strOnlyName); 
   
	int nSizeCount = MultiByteToWideChar(CP_ACP, 0, strSavedPath, -1, NULL, 0);
	WCHAR * pWString = new WCHAR[nSizeCount];
	MultiByteToWideChar(CP_ACP, 0, strSavedPath, -1, pWString, nSizeCount);

	CLSID encoderClsid;
	GetEncoderClsid(L"image/png", &encoderClsid);
	NewBmp->Save(pWString, &encoderClsid, NULL);
	*/
}

double CCreatebgApp::colorDist (Gdiplus::Color clColor1, Gdiplus::Color clColor2)
{
	int diffR = clColor1.GetR() - clColor2.GetR();
    int diffG = clColor1.GetG() - clColor2.GetG();
    int diffB = clColor1.GetB() - clColor2.GetB();
	return (abs(diffR) + abs(diffG) + abs(diffB)) / 255.0;
}

int CCreatebgApp::CompareImage(Bitmap* OriginBmp, Bitmap* NewBmp) {
	
	int nPixCounts = 0;
	Gdiplus::Color clColor1;
	Gdiplus::Color clColor2;

	for (int i = 0; i < OriginBmp->GetHeight(); i++) {
		for (int j = 0; j < OriginBmp->GetWidth(); j++) {
			
			OriginBmp->GetPixel(j, i, &clColor1);
			NewBmp->GetPixel(j, i, &clColor2);
			
			if (clColor1.GetValue() != clColor2.GetValue()) {
				++nPixCounts;
			}
		}
	}
	return nPixCounts;
}

int CCreatebgApp::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT num = 0;
	UINT size = 0;
	ImageCodecInfo * pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo *)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}

CString CCreatebgApp::GetModulePath()
{
	char cTemp[MAX_PATH];
	char *spzRealDirEnd;
	CString strModulePath;

	GetModuleFileName(NULL, cTemp, MAX_PATH);  
	spzRealDirEnd = strrchr(cTemp, '\\');
	*spzRealDirEnd = '\0';
	strModulePath = (CString)cTemp;
	return strModulePath;
}

CString CCreatebgApp::GetFileName(CString strFilename)
{	
	int nFindExt = 0;
	int i = 0;
	for (i = 0; i < strFilename.GetLength(); )
	{
		nFindExt = strFilename.Find('\\', nFindExt);
		if (nFindExt == -1) break;
		nFindExt++;
		i = nFindExt;
	}

	if (i > 0)
		return strFilename.Mid(i, strFilename.GetLength());
	else
		return strFilename;
}
