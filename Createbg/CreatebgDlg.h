#pragma once

class CCreatebgDlg : public CDialog
{
public:
	CCreatebgDlg(CWnd* pParent = NULL);	

	enum { IDD = IDD_CREATEBG_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	

protected:	
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};
