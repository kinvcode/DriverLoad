
// Driver LoaderDlg.h: 头文件
//

#pragma once
#include <winsvc.h>


// CDriverLoaderDlg 对话框
class CDriverLoaderDlg : public CDialogEx
{
// 构造
public:
	CDriverLoaderDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DRIVER_LOADER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton3();
	CEdit m_ctl_path;
	CButton m_ctl_browse;
	CButton m_ctl_install;
	CButton m_ctl_start;
	CButton m_ctl_stop;
	CButton m_ctl_uninstall;
	CStatic m_ctl_tip;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton4();
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
public:
	SC_HANDLE m_sc_manager;
	SC_HANDLE m_sc_service;
	wchar_t m_sys_path[MAX_PATH];
	wchar_t m_sys_name[MAX_PATH];
	void allowInstall();
	void alreadyInstalled();
	void alreadyStart();
	void alreadyStop();
	void alreadyUninstall();
	afx_msg void OnClose();
};
