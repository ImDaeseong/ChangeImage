#include "stdafx.h"
#include "Createbg.h"
#include "CreatebgDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCreatebgDlg::CCreatebgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCreatebgDlg::IDD, pParent)
{
	
}

void CCreatebgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CCreatebgDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CCreatebgDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
		
	return TRUE;  
}

void CCreatebgDlg::OnPaint()
{
	CPaintDC dc(this);
}
