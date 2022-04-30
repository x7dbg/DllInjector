
// DllInjectorDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DllInjector.h"
#include "DllInjectorDlg.h"
#include "afxdialogex.h"

#include "RemoteThreadInject.h"
#include "ManualMapInject.h"

#include <tlhelp32.h>

#define INJECTORDLL_FORREMOTRTHREAD     0 //远程线程注入
#define INJECTORDLL_FORMEMORYMAP        1 //内存映射注入

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDllInjectorDlg 对话框



CDllInjectorDlg::CDllInjectorDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLLINJECTOR_DIALOG, pParent)
    , m_pInjector(nullptr)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDllInjectorDlg::~CDllInjectorDlg()
{
    if (m_pInjector)
    {
        delete m_pInjector;
        m_pInjector = nullptr;
    }
}

void CDllInjectorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_DLL_PATH, m_editDllPath);
    DDX_Control(pDX, IDC_COMBO_INJECTOR_MODE, m_comboxInjecrotMode);
    DDX_Control(pDX, IDC_COMBO_PROCESS_LIST, m_comboProcessList);
    DDX_Control(pDX, IDC_DRAG, m_picDrag);
}

BEGIN_MESSAGE_MAP(CDllInjectorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CDllInjectorDlg::OnBnClickedOk)
    ON_WM_DROPFILES()
    ON_BN_CLICKED(IDC_BTNGETDLLPATH, &CDllInjectorDlg::OnBnClickedBtngetdllpath)
    ON_BN_CLICKED(IDC_BUTTON_INJECTOR, &CDllInjectorDlg::OnBnClickedButtonInjector)
    ON_CBN_DROPDOWN(IDC_COMBO_PROCESS_LIST, &CDllInjectorDlg::OnCbnDropdownComboProcessList)
END_MESSAGE_MAP()


// CDllInjectorDlg 消息处理程序

BOOL CDllInjectorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

    GetAllProcess();

    m_comboxInjecrotMode.InsertString(INJECTORDLL_FORREMOTRTHREAD, _T("远程线程注入"));
    m_comboxInjecrotMode.InsertString(INJECTORDLL_FORMEMORYMAP,_T("内存映射注入"));

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CDllInjectorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDllInjectorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDllInjectorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDllInjectorDlg::OnBnClickedOk()
{
    
}


BOOL CDllInjectorDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP)
    {
        if (pMsg->wParam == VK_ESCAPE)
        {
            return FALSE;
        }
    }
    if (pMsg->message == WM_USER_SELECTPROCESS)
    {
        CString strTmp;
        strTmp.Format(_T("[%d]"),pMsg->wParam);
        
        CString strTitle;
        for (int i=0;i<m_comboProcessList.GetCount();i++)
        {
            m_comboProcessList.GetLBText(i, strTitle);
            if (strTitle.Find(strTmp) != -1)
            {
                m_comboProcessList.SetCurSel(i);
                break;
            }
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


void CDllInjectorDlg::OnDropFiles(HDROP hDropInfo)
{
    int nDropCount = DragQueryFile(hDropInfo, -1, NULL, 0);//取得被拖动文件的数目
    TCHAR szPath[MAX_PATH] = {0};
    if (nDropCount >1)
    {
        MessageBox(_T("一次只能拖拽一个DLL文件！！"));
        return;
    }

    DragQueryFile(hDropInfo, 0, szPath, MAX_PATH);//获得拖曳的第i个文件的文件名
    DragFinish(hDropInfo);  //拖放结束后,释放内存

    m_editDllPath.SetWindowText(szPath);
    
    CDialogEx::OnDropFiles(hDropInfo);
}


void CDllInjectorDlg::OnBnClickedBtngetdllpath()
{
    TCHAR szFileName[MAX_PATH] = { 0 };

    OPENFILENAME openFileName = { 0 };
    openFileName.lStructSize = sizeof(OPENFILENAME);
    openFileName.lpstrInitialDir = NULL;
    openFileName.nMaxFile = MAX_PATH;
    openFileName.lpstrFilter = L"DLL(*.dll)\0*.dll\0";
    openFileName.lpstrFile = szFileName;
    openFileName.nFilterIndex = 1;
    openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (!GetOpenFileName(&openFileName))
    {
        return;
    }
    m_editDllPath.SetWindowText(szFileName);
}


void CDllInjectorDlg::OnBnClickedButtonInjector()
{
    if (m_pInjector)
    {
        delete m_pInjector;
        m_pInjector = nullptr;
    }
    switch (m_comboxInjecrotMode.GetCurSel())
    {
    case INJECTORDLL_FORMEMORYMAP ://内存注入
        m_pInjector = new CManualMapInject;
        break;
    case INJECTORDLL_FORREMOTRTHREAD://远程线程注入
        m_pInjector = new CRemoteThreadInject;
        break;
    default:
        break;
    }
    if (m_pInjector)
    {
        TCHAR szPath[MAX_PATH] = { 0 };
        ::GetWindowText(m_editDllPath.m_hWnd, szPath, MAX_PATH);
        CString strPid;
        m_comboProcessList.GetWindowText(strPid);

        int nPos = strPid.Find(_T("]"));
        if (nPos == -1)
        {
            return;
        }
        strPid.Replace(_T("["), _T(""));
        strPid = strPid.Left(nPos-1);

        m_pInjector->InjectorDLL(szPath, _ttoi(strPid));
    }
}


void CDllInjectorDlg::OnCbnDropdownComboProcessList()
{
    m_comboProcessList.Clear();

    GetAllProcess();
//     CString strTitle;
// 
//     for each (auto var in m_mapProcessList)
//     {
//         strTitle.Format(_T("[%d] "),var.first);
//         strTitle += var.second;
//         m_comboProcessList.InsertString(-1, strTitle);
//     }
}

void CDllInjectorDlg::GetAllProcess()
{
    HANDLE hProcess;                        //进程句柄
    BOOL bProcess = FALSE;                  //获取进程信息的函数返回值

    PROCESSENTRY32 pe;                    //保存进程信息
    pe.dwSize = sizeof(PROCESSENTRY32);

    hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//获取进程快照
    CString strTitle;
    bProcess = Process32First(hProcess, &pe);              //获取第一个进程信息
    while (bProcess)
    {
        strTitle.Format(_T("[%d] "), pe.th32ProcessID);
        strTitle += pe.szExeFile;
        m_comboProcessList.InsertString(-1, strTitle);
        //m_mapProcessList.insert(std::make_pair(pe.th32ProcessID,pe.szExeFile));
        bProcess = Process32Next(hProcess, &pe);
    }
}