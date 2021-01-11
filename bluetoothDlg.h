
// bluetoothDlg.h: 头文件
//

#pragma once
#include <bthsdpdef.h>
#include <BluetoothAPIs.h>
#include "winsock2.h"
#include <Ws2bth.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Bthprops.lib")  


// CbluetoothDlg 对话框
class CbluetoothDlg : public CDialogEx
{
// 构造
public:
	CbluetoothDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BLUETOOTH_DIALOG };
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
	afx_msg void OnLvnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult);
	CListCtrl host_list;
	CListCtrl client_list;
	void fresh_host_list();
	void acceept();
	void recvmessage();
	bool GetSomeInfo(int* nLocal, int* nRemote, BLUETOOTH_DEVICE_INFO* bdiRemote);
	static CString AddrToStr(const void* pAddr);
	SOCKET s_socket;
	afx_msg void OnBnClickedButton1();
	CListBox status;
	afx_msg void OnBnClickedButton2();
	HANDLE  m_hLocalRadio;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedOk();
	CEdit edit;
	afx_msg void OnBnClickedButton6();
	CListBox me;
};
