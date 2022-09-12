
// Driver LoaderDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DriverLoader.h"
#include "DriverLoaderDlg.h"
#include "afxdialogex.h"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDriverLoaderDlg 对话框



CDriverLoaderDlg::CDriverLoaderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRIVER_LOADER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	// 初始化变量
	m_sc_manager = NULL;
}

void CDriverLoaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ctl_path);
	DDX_Control(pDX, IDC_BUTTON2, m_ctl_browse);
	DDX_Control(pDX, IDC_BUTTON3, m_ctl_install);
	DDX_Control(pDX, IDC_BUTTON4, m_ctl_start);
	DDX_Control(pDX, IDC_BUTTON5, m_ctl_stop);
	DDX_Control(pDX, IDC_BUTTON6, m_ctl_uninstall);
	DDX_Control(pDX, IDC_STATIC1, m_ctl_tip);
}

BEGIN_MESSAGE_MAP(CDriverLoaderDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CDriverLoaderDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON2, &CDriverLoaderDlg::OnBnClickedButton2)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON4, &CDriverLoaderDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDriverLoaderDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CDriverLoaderDlg::OnBnClickedButton6)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CDriverLoaderDlg 消息处理程序

BOOL CDriverLoaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);       // 0x0049 == WM_COPYGLOBALDATA

	// TODO: 在此添加额外的初始化代码
	m_ctl_install.EnableWindow(FALSE);
	m_ctl_start.EnableWindow(FALSE);
	m_ctl_stop.EnableWindow(FALSE);
	m_ctl_uninstall.EnableWindow(FALSE);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CDriverLoaderDlg::OnPaint()
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
HCURSOR CDriverLoaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDriverLoaderDlg::OnBnClickedButton2()
{
	// 选择文件
	CString file_path = _T("");
	CString file_name = _T("");
	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("驱动文件 (*.sys)|*.sys||"), NULL);
	if (dlgFile.DoModal())
	{
		file_path = dlgFile.GetPathName();
		file_name = dlgFile.GetFileName();
		if (file_path.Find(L".sys") != -1) {
			m_ctl_path.SetWindowText(file_path);
			wcscpy_s(m_sys_path, MAX_PATH, file_path);
			wcscpy_s(m_sys_name, MAX_PATH, file_name);
			allowInstall();
		}
	}
	// TODO: 在此添加控件通知处理程序代码
}

void CDriverLoaderDlg::OnBnClickedButton3()
{
	// 安装

	DWORD status;

	// 打开服务管理器数据库
	m_sc_manager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (m_sc_manager == NULL)
	{
		m_ctl_tip.SetWindowText(L"安装驱动失败：无法打开驱动管理器");
		return;
	}

	// 创建服务
	m_sc_service = CreateServiceW(
		m_sc_manager,
		m_sys_name,
		m_sys_name,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE,
		m_sys_path,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (m_sc_service == NULL)
	{
		status = GetLastError();
		if (status != ERROR_SERVICE_EXISTS)
		{
			m_ctl_tip.SetWindowText(L"安装驱动失败：无法创建服务");
			return;
		}
	}

	// 打开服务
	m_sc_service = OpenService(m_sc_manager, m_sys_name, SERVICE_ALL_ACCESS);
	if (m_sc_service == NULL) {
		m_ctl_tip.SetWindowText(L"安装驱动失败：无法打开服务");
		return;
	}

	alreadyInstalled();
}

void CDriverLoaderDlg::OnBnClickedButton4()
{
	DWORD status;

	// 启动

	if (!StartService(m_sc_service, 0, NULL))
	{
		status = GetLastError();
		if (status != ERROR_SERVICE_ALREADY_RUNNING)
		{
			m_ctl_tip.SetWindowText(L"启动驱动失败");
			return;
		}
	}

	alreadyStart();
}


void CDriverLoaderDlg::OnBnClickedButton5()
{
	// 停止
	SERVICE_STATUS service_status;

	if (!::ControlService(m_sc_service, SERVICE_CONTROL_STOP, &service_status))
	{
		m_ctl_tip.SetWindowText(L"停止驱动失败");
		return;
	}

	m_sc_service = NULL;

	alreadyStop();


	// TODO: 在此添加控件通知处理程序代码
}


void CDriverLoaderDlg::OnBnClickedButton6()
{
	// 卸载

	if (m_sc_service != NULL) 
	{
		if (!::DeleteService(m_sc_service))
		{
			m_ctl_tip.SetWindowText(L"卸载驱动失败");
		}
		CloseServiceHandle(m_sc_service);
		m_sc_service = NULL;
	}

	if (m_sc_manager != NULL) 
	{
		CloseServiceHandle(m_sc_manager);
		m_sc_manager = NULL;
	}

	alreadyUninstall();
	// TODO: 在此添加控件通知处理程序代码
}

void CDriverLoaderDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	UINT fileCount = DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (fileCount != 1) {
		AfxMessageBox(L"仅支持拖拽一个文件");
		return;
	}

	wchar_t filepath[MAX_PATH];
	DragQueryFile(hDropInfo, 0, filepath, MAX_PATH);
	wcscpy_s(m_sys_path, MAX_PATH, filepath);

	LPWSTR file_name = filepath;
	PathStripPathW(file_name);
	wcscpy_s(m_sys_name, MAX_PATH, file_name);

	CString path = CString(m_sys_path);
	if (path.Find(L".sys") != -1) {
		m_ctl_path.SetWindowText(path);
		allowInstall();
	}
	else {
		AfxMessageBox(L"仅支持sys文件");
	}

	CDialogEx::OnDropFiles(hDropInfo);

	CDialogEx::OnDropFiles(hDropInfo);
}

void CDriverLoaderDlg::allowInstall()
{
	m_ctl_install.EnableWindow(TRUE);
	m_ctl_tip.SetWindowText(L"等待安装驱动");
}

void CDriverLoaderDlg::alreadyInstalled()
{
	m_ctl_browse.EnableWindow(FALSE);
	m_ctl_install.EnableWindow(FALSE);
	m_ctl_start.EnableWindow(TRUE);
	m_ctl_stop.EnableWindow(FALSE);
	m_ctl_uninstall.EnableWindow(FALSE);
	m_ctl_tip.SetWindowText(L"安装驱动成功");
}

void CDriverLoaderDlg::alreadyStart()
{
	m_ctl_browse.EnableWindow(FALSE);
	m_ctl_install.EnableWindow(FALSE);
	m_ctl_start.EnableWindow(FALSE);
	m_ctl_stop.EnableWindow(TRUE);
	m_ctl_tip.SetWindowText(L"驱动启动成功");
}

void CDriverLoaderDlg::alreadyStop()
{
	m_ctl_browse.EnableWindow(FALSE);
	m_ctl_install.EnableWindow(FALSE);
	m_ctl_start.EnableWindow(FALSE);
	m_ctl_stop.EnableWindow(FALSE);
	m_ctl_uninstall.EnableWindow(TRUE);
	m_ctl_tip.SetWindowText(L"驱动停止成功");
}

void CDriverLoaderDlg::alreadyUninstall()
{
	m_ctl_browse.EnableWindow(TRUE);
	m_ctl_install.EnableWindow(FALSE);
	m_ctl_start.EnableWindow(FALSE);
	m_ctl_stop.EnableWindow(FALSE);
	m_ctl_uninstall.EnableWindow(FALSE);
	m_ctl_tip.SetWindowText(L"驱动卸载成功");
	m_ctl_path.SetWindowText(L"");
}


void CDriverLoaderDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_sc_service != NULL) {
		CloseServiceHandle(m_sc_service);
	}
	if (m_sc_manager != NULL) {
		CloseServiceHandle(m_sc_manager);
	}
	CDialogEx::OnClose();
}
