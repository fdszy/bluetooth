
// bluetoothDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "bluetooth.h"
#include "bluetoothDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <initguid.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct RemoteDev
{//远程蓝牙设备详细信息
	RemoteDev()
	{
		Address = 0;
	}
	CString sName;      //名称
	__int64 Address;    //地址
};

CArray<RemoteDev, RemoteDev&>m_arrRemote;    //远程蓝牙设备数组

CString CbluetoothDlg::AddrToStr(const void* pAddr)
{
	CString sAddress;
	if (pAddr)
	{
		BLUETOOTH_ADDRESS Address;
		int         nBytes = sizeof(Address.rgBytes);
		TCHAR       szByte[8];
		const BYTE* pByte = (const BYTE*)pAddr + (nBytes - 1);

		for (int i = 0; i < nBytes; i++)
		{
			_stprintf_s(szByte, _T(":%02x"), *pByte--);
			sAddress += i ? szByte : (szByte + 1);
		}
	}
	return sAddress;
}

void CbluetoothDlg::fresh_host_list() {
	host_list.DeleteAllItems();
	client_list.DeleteAllItems(); 
	HANDLE  hRadio = NULL;
	HBLUETOOTH_RADIO_FIND hbf = NULL;
	BLUETOOTH_RADIO_INFO bri = { sizeof(BLUETOOTH_RADIO_INFO) };
	BLUETOOTH_FIND_RADIO_PARAMS btfrp = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
	BLUETOOTH_DEVICE_SEARCH_PARAMS btsp = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) };
	HBLUETOOTH_DEVICE_FIND hbdf = NULL;
	BLUETOOTH_DEVICE_INFO btdi = { sizeof(BLUETOOTH_DEVICE_INFO) };
	int nRow = 0;
	CString str;
	hbf = BluetoothFindFirstRadio(&btfrp, &hRadio);
	m_hLocalRadio = hRadio;
	bool brfind = hbf != NULL;
	while (brfind) {
		if (ERROR_SUCCESS == BluetoothGetRadioInfo(hRadio, &bri))
		{
			//名称
			str.Format(_T("%ls"), bri.szName);
			host_list.InsertItem(nRow, str);
			//地址
			host_list.SetItemText(nRow, 1, CbluetoothDlg::AddrToStr(&bri.address));
			
			
			++nRow;
			BluetoothEnableIncomingConnections(hRadio, TRUE);
			BluetoothEnableDiscovery(hRadio, TRUE);
			BluetoothFindDeviceClose(hbdf);
		}
		CloseHandle(hRadio);
		brfind = BluetoothFindNextRadio(hbf, &hRadio);
	}

	BluetoothFindRadioClose(hbf);
}

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
public:
	afx_msg void OnBnClickedButton1();
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CAboutDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CbluetoothDlg 对话框



CbluetoothDlg::CbluetoothDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BLUETOOTH_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CbluetoothDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, host_list);
	DDX_Control(pDX, IDC_LIST3, client_list);
	DDX_Control(pDX, IDC_LIST7, status);
	DDX_Control(pDX, IDC_EDIT1, edit);
	DDX_Control(pDX, IDC_LIST5, me);
}

BEGIN_MESSAGE_MAP(CbluetoothDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST2, &CbluetoothDlg::OnLvnItemchangedList2)
	ON_BN_CLICKED(IDC_BUTTON1, &CbluetoothDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CbluetoothDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CbluetoothDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDOK, &CbluetoothDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON6, &CbluetoothDlg::OnBnClickedButton6)
END_MESSAGE_MAP()


// CbluetoothDlg 消息处理程序

BOOL CbluetoothDlg::OnInitDialog()
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

	// TODO: 在此添加额外的初始化代码
	WSADATA WSAData = { 0 };
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	host_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	host_list.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 280);
	host_list.InsertColumn(1, _T("地址"), LVCFMT_LEFT, 280);

	client_list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	client_list.InsertColumn(0, _T("名称"), LVCFMT_LEFT, 280);
	client_list.InsertColumn(1, _T("地址"), LVCFMT_LEFT, 280);

	std::thread t1(&CbluetoothDlg::fresh_host_list, this);
	t1.detach();
	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CbluetoothDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CbluetoothDlg::OnPaint()
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
HCURSOR CbluetoothDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CbluetoothDlg::OnLvnItemchangedList2(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CAboutDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CbluetoothDlg::OnBnClickedButton1()
{
	SOCKADDR_BTH sa;
	int sa_len = sizeof(SOCKADDR_BTH);
	s_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (s_socket == SOCKET_ERROR) {
		AfxMessageBox(_T("创建失败"));
	}
	// TODO: 在此添加控件通知处理程序代码
	memset(&sa, 0, sizeof(SOCKADDR_BTH));
	sa.addressFamily = AF_BTH;
	//WSAStringToAddress(_T("48:a4:72:63:7b:52"), AF_BTH, NULL, (LPSOCKADDR)&sa, &sa_len);
	sa.port = 20 & 0xff;
	int j = WSAGetLastError();
	if (SOCKET_ERROR == bind(s_socket, (SOCKADDR*)&sa, sizeof(sa))) {
		int i = WSAGetLastError();
		AfxMessageBox(_T("绑定失败"));
		i = WSAGetLastError();
		closesocket(s_socket);
		s_socket = NULL;
		return;
	}
	listen(s_socket, 1);
	CString str("服务器启动了");
	status.AddString(str);
	std::thread t2(&CbluetoothDlg::acceept, this);
	t2.detach();
	Sleep(500);
	std::thread t3(&CbluetoothDlg::recvmessage, this);
	t3.detach();

}
void CbluetoothDlg::acceept() {
	SOCKADDR_BTH sa;
	SOCKET temp;
	int sa_len = sizeof(sa);
	memset(&sa, 0, sa_len);
	temp = accept(s_socket, (LPSOCKADDR)&sa, &sa_len);
	if (SOCKET_ERROR != temp) {
		CString str("accept成功");
		status.AddString(str);
	}
}

void CbluetoothDlg::recvmessage() {
	char buf[1024];
	int received;
	char cmd;
	int len;
	while (1) {
		memset(buf, 0, 1024);
		received = recv(s_socket, buf, 5, 0);
		if (received > 0)
		{
			strcat(buf, "\r\n");
			CString str(buf);
			me.AddString(str);
		}
	}
}


bool CbluetoothDlg::GetSomeInfo(int* nLocal, int* nRemote, BLUETOOTH_DEVICE_INFO* bdiRemote)
{
	if (nLocal)
	{//获取选中的本地设备
		*nLocal = host_list.GetNextItem(-1, LVNI_SELECTED);
		if (*nLocal < 0)
		{
			AfxMessageBox(_T("请选中一个本地蓝牙设备"));
			return false;
		}
	}
	if (nRemote || bdiRemote)
	{//获取选中的远程设备
		int nSel = client_list.GetNextItem(-1, LVNI_SELECTED);
		if (nSel < 0)
		{
			AfxMessageBox(_T("请选中一个远程蓝牙设备"));
			return false;
		}
		if (nRemote)
		{
			*nRemote = nSel;
		}
		if (bdiRemote)
		{
			memset(bdiRemote, 0, sizeof(*bdiRemote));
			bdiRemote->dwSize = sizeof(*bdiRemote);
			bdiRemote->Address.ullLong = m_arrRemote[nSel].Address;
			if (ERROR_SUCCESS != BluetoothGetDeviceInfo(m_hLocalRadio, bdiRemote))
			{
				AfxMessageBox(_T("获取远程蓝牙设备信息失败"));
				return false;
			}
		}
	}
	return true;
}

void CbluetoothDlg::OnBnClickedButton2()
{
	int nSelLocal = 0;
	int nSelRemote = 0;
	SOCKADDR_BTH sa;
	int sa_len = sizeof(SOCKADDR_BTH);
	BLUETOOTH_DEVICE_INFO bdi;
	if (!GetSomeInfo(&nSelLocal, &nSelRemote, &bdi))
	{
		return;
	}
	CString m_sLocal =  host_list.GetItemText(nSelLocal, 1);
	CString m_sRemote = client_list.GetItemText(nSelRemote, 1);
	
	// TODO: 在此添加控件通知处理程序代码
	char *ttt;
	LPCTSTR p = m_sLocal.GetBuffer(0);
	m_sLocal.ReleaseBuffer();
	ttt = new char[m_sLocal.GetLength() + 1];
	strcpy_s(ttt, m_sLocal.GetLength() + 1, CT2CA(p));
	char* tttt;
	LPCTSTR pp = m_sRemote.GetBuffer(0);
	m_sLocal.ReleaseBuffer();
	tttt = new char[m_sRemote.GetLength() + 1];
	strcpy_s(tttt, m_sRemote.GetLength() + 1, CT2CA(pp));
	printf("1");
	memset(&sa, 0, sa_len);
	if (SOCKET_ERROR == WSAStringToAddress(m_sRemote.GetBuffer(m_sRemote.GetLength()), AF_BTH, NULL, (LPSOCKADDR)&sa, &sa_len)) {
		int i = WSAGetLastError();
		AfxMessageBox(_T("地址转化错误"));
		return;
	}
	sa.addressFamily = AF_BTH;
	sa.port = 20;
	s_socket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
	if (SOCKET_ERROR == s_socket) {
		AfxMessageBox(_T("失败"));
	}
	if (SOCKET_ERROR == connect(s_socket, (LPSOCKADDR)&sa, sa_len)) {
		int i = WSAGetLastError();
		AfxMessageBox(_T("连接失败"));
		closesocket(s_socket);
		s_socket = NULL;
		return;
	}
	status.AddString(_T("蓝牙socket连接成功"));
}


void CbluetoothDlg::OnBnClickedButton3()
{
	int nSel = 0;
	BLUETOOTH_DEVICE_INFO           bdi = { sizeof(BLUETOOTH_DEVICE_INFO) };
	BLUETOOTH_DEVICE_SEARCH_PARAMS  bdsp = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) };
	bdsp.hRadio = m_hLocalRadio;
	bdsp.fReturnAuthenticated = TRUE;
	bdsp.fReturnRemembered = TRUE;
	bdsp.fReturnUnknown = TRUE;
	bdsp.fReturnConnected = TRUE;
	bdsp.fIssueInquiry = TRUE;
	bdsp.cTimeoutMultiplier = 30;
	HBLUETOOTH_DEVICE_FIND  hbf = BluetoothFindFirstDevice(&bdsp, &bdi);
	if (hbf == NULL)
	{
		return ;
	}
	do
	{
		TRACE(_T("%s ( %s )\n"), bdi.szName, AddrToStr(bdi.Address.rgBytes));
		RemoteDev dev;
		dev.Address = bdi.Address.ullLong;
		dev.sName = bdi.szName;
		m_arrRemote.Add(dev);
	} while (BluetoothFindNextDevice(hbf, &bdi));
	BluetoothFindDeviceClose(hbf);
	
	CString str;
	int     num = m_arrRemote.GetSize();
	

	for (int i = 0; i < num; i++)
	{
		const RemoteDev& dev =m_arrRemote[i];
		//名称
		client_list.InsertItem(i, dev.sName);
		//地址
		client_list.SetItemText(i, 1, CbluetoothDlg::AddrToStr(&dev.Address));
		bdi.Address.ullLong = dev.Address;
	}
	// TODO: 在此添加控件通知处理程序代码
}

int g_iCounter = 0;
void CbluetoothDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	char ch[20];
	CString str;
	int len;
	edit.GetWindowText(str);
	sprintf(ch, " %04d", g_iCounter++);
	str += ch;
	len = str.GetLength();
	ch[0] = 0;
	sprintf(ch + 1, "%04d", len);
	send(s_socket, ch, 5, 0);
	send(s_socket, (char *)str.GetBuffer(), len, 0);
	str += "\r\n";
	edit.SetSel(edit.GetWindowTextLength(), edit.GetWindowTextLength());
	edit.ReplaceSel(str.GetBuffer());
	CDialogEx::OnOK();
}


void CbluetoothDlg::OnBnClickedButton6()
{
	CString str;
	edit.GetWindowTextW(str);//从输入框提取信息打印到屏幕上（加到末尾行下面），显示为自己发送（CString类型）
	USES_CONVERSION; // 用于类型转换的宏，为了可以使用W2A
	char* content = W2A(str.GetBuffer(0));	//GetBuffer(0)可以读出CString中内容（LPTSTR类型），WSA将其转换为char*型
	content += '\0';
	send(s_socket, content, strlen(content), 0);	//发送消息+强制转化
	edit.SetWindowTextW(_T(""));
	// TODO: 在此添加控件通知处理程序代码
}
