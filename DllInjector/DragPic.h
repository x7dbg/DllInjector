#pragma once

#define WM_USER_SELECTPROCESS WM_USER+0x999

class CDragPic : public CStatic
{
	DECLARE_DYNAMIC(CDragPic)

public:
	CDragPic();
	virtual ~CDragPic();
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
private:
	HWND m_hWnd;
	RECT rc;
};
