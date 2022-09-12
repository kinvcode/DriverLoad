
// Driver LoaderDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "DriverLoader.h"
#include "DriverLoaderDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDriverLoaderDlg 对话框



CDriverLoaderDlg::CDriverLoaderDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DRIVER_LOADER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
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
END_MESSAGE_MAP()


// CDriverLoaderDlg 消息处理程序

BOOL CDriverLoaderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

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
	CString strFile = _T("");
	CFileDialog    dlgFile(TRUE, NULL, NULL, OFN_HIDEREADONLY, _T("驱动文件 (*.sys)|*.sys||"), NULL);
	if (dlgFile.DoModal())
	{
		strFile = dlgFile.GetPathName();
		if (strFile.Find(L".sys") != -1) {
			m_ctl_path.SetWindowText(strFile);
			allowInstall();
		}
	}
	// TODO: 在此添加控件通知处理程序代码
}

void CDriverLoaderDlg::OnBnClickedButton3()
{
	// 安装
	// TODO: 在此添加控件通知处理程序代码

	alreadyInstalled();
}

void CDriverLoaderDlg::OnBnClickedButton4()
{
	// 启动

	alreadyStart();
	// TODO: 在此添加控件通知处理程序代码
}


void CDriverLoaderDlg::OnBnClickedButton5()
{
	// 停止
	alreadyStop();


	// TODO: 在此添加控件通知处理程序代码
}


void CDriverLoaderDlg::OnBnClickedButton6()
{
	// 卸载
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

	CString path = CString(filepath);
	if (path.Find(L".sys") != -1) {
		m_ctl_path.SetWindowText(path);
		allowInstall();
	}
	else {
		AfxMessageBox(L"仅支持sys文件");
	}

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
	m_ctl_uninstall.EnableWindow(TRUE);
	m_ctl_tip.SetWindowText(L"安装驱动成功");
}

void CDriverLoaderDlg::alreadyStart() 
{
	m_ctl_start.EnableWindow(FALSE);
	m_ctl_stop.EnableWindow(TRUE);
	m_ctl_tip.SetWindowText(L"驱动启动成功");
}

void CDriverLoaderDlg::alreadyStop()
{
	m_ctl_stop.EnableWindow(FALSE);
	m_ctl_uninstall.EnableWindow(TRUE);
	m_ctl_tip.SetWindowText(L"驱动停止成功");
}

void CDriverLoaderDlg::alreadyUninstall()
{
	m_ctl_uninstall.EnableWindow(FALSE);
	m_ctl_browse.EnableWindow(TRUE);
	m_ctl_tip.SetWindowText(L"驱动卸载成功");
	m_ctl_path.SetWindowText(L"");
}
