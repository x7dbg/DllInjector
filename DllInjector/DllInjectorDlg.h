
// DllInjectorDlg.h: 头文件
//

#pragma once
#include "Injector.h"
#include "DragPic.h"

//#include <map>
// CDllInjectorDlg 对话框
class CDllInjectorDlg : public CDialogEx
{
// 构造
public:
	CDllInjectorDlg(CWnd* pParent = nullptr);	// 标准构造函数
    virtual ~CDllInjectorDlg();
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLLINJECTOR_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedOk();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnDropFiles(HDROP hDropInfo);
    afx_msg void OnBnClickedBtngetdllpath();
    afx_msg void OnBnClickedButtonInjector();
    afx_msg void OnCbnDropdownComboProcessList();
    afx_msg void OnBnClickedButtonRefresh();
    afx_msg void OnBnClickedCheckTopmost();

    void GetAllProcess();
    void RefreshProcess();
private:
    CEdit m_editDllPath;
    CComboBox m_comboxInjecrotMode;
    CInjector* m_pInjector;
    CComboBox m_comboProcessList;
    //std::map<DWORD, CString> m_mapProcessList;
    CDragPic m_picDrag;
};
