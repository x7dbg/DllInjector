// MyPic.cpp : 实现文件
//

#include "pch.h"
#include "DllInjector.h"
#include "dragpic.h"
#define  TIMERID 1000


IMPLEMENT_DYNAMIC(CDragPic, CStatic)

CDragPic::CDragPic()
{
   m_hWnd=NULL;
}

CDragPic::~CDragPic()
{
}


BEGIN_MESSAGE_MAP(CDragPic, CStatic)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CDragPic::OnLButtonDown(UINT nFlags, CPoint point)
{
    SetCapture(); //设定鼠标消息的发送窗口为当前窗口
	HCURSOR hc = LoadCursor(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDC_CURSOR));
	::SetCursor(hc);

	HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_DRAG));
	this->SetIcon(hIcon);
	SetTimer(TIMERID,677, NULL);
	CStatic::OnLButtonDown(nFlags, point);
}

void CDragPic::OnLButtonUp(UINT nFlags, CPoint point)
{
    ReleaseCapture();
	HICON hIcon = LoadIcon(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_NODRAG));
	this->SetIcon(hIcon);
	KillTimer(TIMERID);
	CStatic::OnLButtonUp(nFlags, point);
}

HWND SmallestWindowFromPoint(POINT point)
{
	RECT rect, rcTemp;
	HWND hParent, hWnd, hTemp;

	hWnd = ::WindowFromPoint(point);
	if( hWnd != NULL ) 
	{
		::GetWindowRect(hWnd, &rect);
		hParent = ::GetParent(hWnd);

		if(hParent != NULL) 
		{
			hTemp = hWnd;
			do 
			{
				hTemp = ::GetWindow(hTemp, GW_HWNDNEXT);

				::GetWindowRect(hTemp, &rcTemp);
				if(::PtInRect(&rcTemp, point) && ::GetParent(hTemp) == hParent && ::IsWindowVisible(hTemp))
				{
					if(((rcTemp.right - rcTemp.left) * (rcTemp.bottom - rcTemp.top)) < ((rect.right - rect.left) * (rect.bottom - rect.top))) 
					{
						hWnd = hTemp;
						::GetWindowRect(hWnd, &rect);
					}
				}
			} while(hTemp != NULL);
		}
	}

	return hWnd;
}


void CDragPic::OnTimer(UINT_PTR nIDEvent)
{
	HWND deskWnd = ::GetDesktopWindow();
	HDC deskDC = ::GetWindowDC(deskWnd);
	int oldRop2 = SetROP2(deskDC, R2_NOTXORPEN);

	CPoint pt;
	GetCursorPos(&pt);


	HWND hTargetWnd = SmallestWindowFromPoint(pt);
	m_hWnd=hTargetWnd;
	if (GetWindowThreadProcessId(GetSafeHwnd(), NULL) == GetWindowThreadProcessId(hTargetWnd, NULL))
	{
		//GetParent()->SetDlgItemText(IDC_EDIT_WINDOWTITLE, _T(""));
		return;
	}

	DWORD pid = 0;
	GetWindowThreadProcessId(hTargetWnd, &pid);
	GetParent()->PostMessage(WM_USER_SELECTPROCESS, pid);

	//窗口标题、编辑框文本的获取
// 	LRESULT nLen = ::SendMessage(hTargetWnd, WM_GETTEXTLENGTH, 0, 0);
// 	if (nLen > 0)
// 	{
// 		nLen += 1;
// 		TCHAR *pszBuf = new TCHAR[nLen];
// 		ZeroMemory(pszBuf, nLen);
// 		::SendMessage(hTargetWnd, WM_GETTEXT, nLen, (LPARAM)pszBuf);
// 		//GetParent()->SetDlgItemText(IDC_EDIT_WINDOWTITLE, pszBuf);
// 		delete [] pszBuf;
// 	}

    ::GetWindowRect(m_hWnd,&rc);
	CRect mRect;
	::GetWindowRect(hTargetWnd, &mRect);

	if( mRect.left < 0 ) mRect.left = 0;
	if (mRect.top < 0 ) mRect.top = 0;

	HPEN newPen = ::CreatePen(0, 5, RGB(127, 60, 110));
	HGDIOBJ oldPen = ::SelectObject(deskDC, newPen);

	::Rectangle(deskDC, mRect.left, mRect.top, mRect.right, mRect.bottom);
	Sleep(200);
	::Rectangle(deskDC, mRect.left, mRect.top, mRect.right, mRect.bottom);

	::SetROP2(deskDC, oldRop2);
	::SelectObject( deskDC, oldPen);

	::DeleteObject(newPen);
	::ReleaseDC(deskWnd, deskDC);
	deskDC = NULL;

	CStatic::OnTimer(nIDEvent);
}
