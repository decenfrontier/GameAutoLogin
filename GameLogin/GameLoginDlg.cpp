﻿
// GameLoginDlg.cpp: 实现文件
//

#include "framework.h"
#include "GameLogin.h"
#include "GameLoginDlg.h"
#include "afxdialogex.h"
#include "MyFunc.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

char szPath_Client[MAX_PATH] = { 0 };	//"client"文件夹的全路径
char szPath_Launcher[MAX_PATH] = { 0 };	//launcher.exe的全路径
char szPath_Game[MAX_PATH] = { 0 };		//"热血江湖兵临城下"文件夹的全路径

HWND hWnd = NULL;
HDC hdcClient;


// CGameLoginDlg 对话框
CGameLoginDlg::CGameLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAMELOGIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGameLoginDlg::Init_AllCtrlData()
{
	SetTimer(ID_TIMER1, 1000, NULL);
	strcpy_s(szPath_Launcher, "D:\\热血江湖兵临城下\\launcher.exe");
	strcpy_s(szPath_Game, "D:\\热血江湖兵临城下\\");
	strcpy_s(szPath_Client, "D:\\热血江湖兵临城下\\Client\\");
	m_edt_LauncherPath = szPath_Launcher;

	CComboBox* pCmbBox_DaQu = (CComboBox*)GetDlgItem(IDC_COMBO1);
	pCmbBox_DaQu->SetCurSel(网通二区);

	CComboBox* pCmbBox_Server = (CComboBox*)GetDlgItem(IDC_COMBO2);
	pCmbBox_Server->ResetContent();
	pCmbBox_Server->AddString("1 雄霸");
	pCmbBox_Server->SetCurSel(0);

	CComboBox* pCmbBox_XianLu = (CComboBox*)GetDlgItem(IDC_COMBO3);
	pCmbBox_XianLu->SetCurSel(七线);

	CComboBox* pCmbBox_RoleIndex = (CComboBox*)GetDlgItem(IDC_COMBO4);
	pCmbBox_RoleIndex->SetCurSel(第1个);

	Init_ListCtrl();

	UpdateData(FALSE);	//变量 -> 控件
}

void CGameLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_edt_LauncherPath);
}

BEGIN_MESSAGE_MAP(CGameLoginDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON5, &CGameLoginDlg::OnBnClickedBtn_AutoLogin)
	ON_BN_CLICKED(IDC_BUTTON1, &CGameLoginDlg::OnBnClickedBtn_LauncherDir)
	ON_WM_TIMER()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CGameLoginDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// CGameLoginDlg 消息处理程序

BOOL CGameLoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	Init_AllCtrlData();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGameLoginDlg::OnPaint()
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
HCURSOR CGameLoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGameLoginDlg::Init_ListCtrl()
{
	CListCtrl *pListC = (CListCtrl*)GetDlgItem(IDC_LIST1);
	CRect RListC;
	DWORD dwStyle = ::GetWindowLongA(pListC->m_hWnd, GWL_STYLE);
	dwStyle |= LVS_REPORT;	//设置为报表格式
	::SetWindowLongA(pListC->m_hWnd, GWL_STYLE, dwStyle);

	DWORD dwExStyle = pListC->GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT;	//选中某行使整行高亮
	dwExStyle |= LVS_EX_GRIDLINES;		//网格线
	pListC->SetExtendedStyle(dwExStyle);

	pListC->InsertColumn(0, "账号", LVCFMT_CENTER, 77, 0);
	pListC->InsertColumn(1, "密码", LVCFMT_CENTER, 77, 0);
	pListC->InsertColumn(2, "大区", LVCFMT_CENTER, 66, 0);
	pListC->InsertColumn(3, "服务器", LVCFMT_CENTER, 88, 0);
	pListC->InsertColumn(4, "线路", LVCFMT_CENTER, 66, 0);
	pListC->InsertColumn(5, "人物角色", LVCFMT_CENTER, 100, 0);
}

BYTE GameOpenNum = 0;	//游戏窗口打开数
BOOL CALLBACK EnumWindowsProc(HWND hwnd,LPARAM lParam)
{
	TCHAR buf_ClassName[200] = { 0 };
	GetClassName(hwnd, buf_ClassName, sizeof(buf_ClassName));
	if (strcmp(buf_ClassName, "D3D Window") == 0)	// 增加窗口打开数
		GameOpenNum++;
	return TRUE;
}

void CGameLoginDlg::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case ID_TIMER1:
	{
		GameOpenNum = 0;
		EnumWindows(EnumWindowsProc, NULL);

		CString str;
		str.Format(_T("%d"), GameOpenNum);
		GetDlgItem(IDC_TEXT)->SetWindowText(str);
	}
	break;
	}
	CDialogEx::OnTimer(nIDEvent);
}

// 选择launcher.exe的目录
void CGameLoginDlg::OnBnClickedBtn_LauncherDir()
{
	BROWSEINFOA bi = { 0 };

	char szDisplayName[MAX_PATH] = { 0 };
	bi.pszDisplayName = szDisplayName;	//接收 文件名称 的缓冲区(不包含路径)
	bi.hwndOwner = m_hWnd;	//父窗口
	bi.lpszTitle = "请选择包含launcher.exe的路径:";	//标题
	bi.ulFlags = BIF_BROWSEINCLUDEFILES | BIF_RETURNONLYFSDIRS;	//样式

	ITEMIDLIST* idList = SHBrowseForFolder(&bi);
	SHGetPathFromIDList(idList, szPath_Launcher);
	TRACE("%s\n", szDisplayName);
	if (strcmp(szDisplayName, "launcher.exe") != 0)
	{
		::MessageBoxA(0, "选择launcher.exe路径错误", "Error", MB_OK);
		szPath_Launcher[0] = 0;
		return;
	}
	char* sztmp = StrStrA(szPath_Launcher, "launcher.exe");
	if (sztmp != NULL)
	{
		sztmp[0] = 0;
		//获取 游戏目录 全路径
		strcpy_s(szPath_Game, szPath_Launcher);
		//重新设置 launcher.exe 的全路径
		strcat_s(szPath_Launcher, "launcher.exe");
		//获取 client目录 全路径
		strcpy_s(szPath_Client, szPath_Game);
		strcat_s(szPath_Client, "Client\\");
	}

	m_edt_LauncherPath = szPath_Launcher;
	UpdateData(FALSE);	//变量 -> 控件
}

//自动登录
void CGameLoginDlg::OnBnClickedBtn_AutoLogin()
{
	CLoginData LoginData;
	LoginData.strAccount = "ws164803";
	LoginData.strPwd = "kptg6594571";
	LoginData.niServer = 网通二区;
	LoginData.niGameLine = 七线;
	LoginData.niRoleIndex = 0;

	AutoLogin(&LoginData);
}



#define MultiOpenAddr 0x00407BDA
/* [2020/03/19 10:25]-[Remark: None] */
/* [启动登录器]-[成功返回真，否则返回假] */
void RunLaucher()
{
	if (szPath_Launcher[0] == 0)
	{
		::MessageBoxA(0, "请先选择launcher.exe的正确路径", "提示", MB_OK);
		return;
	}
	//创建一个挂起的进程
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi;
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;	//指定wShowWindow成员有效
	si.wShowWindow = TRUE;	//显示新建进程的主窗口
	BOOL bRet = CreateProcessA(NULL,szPath_Launcher,NULL,NULL,FALSE,
		CREATE_SUSPENDED,NULL,szPath_Game,&si,&pi);

	//Inline Hook
	BYTE byWriteBuf = 0xEB;
	HANDLE hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, pi.dwProcessId);
	WriteProcessMemory(hProcess, (DWORD*)MultiOpenAddr, &byWriteBuf, 1, NULL);

	//恢复主线程
	ResumeThread(pi.hThread);
}

BOOL IsAbleToStartGame()
{
	hWnd = FindWindowA(NULL, "Yulgang_File_Update");
	if (hWnd == NULL)	return FALSE;
	HDC hdcClient = GetDC(hWnd);
	DWORD dwBGR = GetPixel(hdcClient, 204, 467);	//取窗口客户区一个点的颜色BGR: 0a0a0a
	DbgOutput("%X\n", dwBGR);
	if (dwBGR == 0x0a0a0a)	return TRUE;
	return FALSE;
}

/* [2020/03/19 10:11]-[Remark: None] */
/* [选择区服并开始游戏]-[成功返回真，否则返回假] */
BOOL SelDaQuAndStart(DWORD dwIndex)
{
	int x = 210;	//"网通一区"按钮正中央
	int y = 110 + dwIndex * 28;
	hWnd = FindWindowA(NULL, "Yulgang_File_Update");
	if (hWnd == NULL)	return FALSE;

	// 点击 区服
	MoveTo(x, y,hWnd);
	LeftClick();
	//等待 开始游戏按钮 显示
	while (!IsAbleToStartGame())	Sleep(500);
	Sleep(500);

	// 点击"开始游戏"
	MoveTo(66, 428, hWnd);
	LeftClick();
	return TRUE;
}

/* [2020/03/20 09:03]-[Remark: None] */
/* [输入字符串,用于输入账号和密码]-[成功返回真，否则返回假] */
void InputString(string strKey)
{
	for(DWORD i = 0; i < strKey.size(); ++i)
	{
		if (strKey[i] >= 'a' && strKey[i] <= 'z')
		{
			if ((GetKeyState(VK_CAPITAL) & 01) == 1)	//低位为1,则CapsLock开启, 要关闭它
			{
				KeyPress(VK_CAPITAL);
			}
			KeyPress(strKey[i] + 'A' - 'a');
		}
		else if (strKey[i] >= 'A' && strKey[i] <= 'Z')
		{
			if ((GetKeyState(VK_CAPITAL) & 01) == 0)	//低位为0,则CapsLock关闭, 要打开它
			{
				KeyPress(VK_CAPITAL);
			}
			KeyPress(strKey[i]);
		}
		else if (strKey[i] >= '0' && strKey[i] <= '9')
		{
			KeyPress(strKey[i]);
		}
	}
}

BOOL IsAbleToInputIdAndPwd()
{
	hWnd = FindWindowA(NULL,"YB_OnlineClient");
	if (hWnd == NULL)	return FALSE;
	hdcClient = GetDC(hWnd);
	DWORD dwBGR = GetPixel(hdcClient, 382, 154);	//取窗口客户区一个点的颜色BGR: 78D0FE
	DbgOutput("%X\n", dwBGR);
	if (dwBGR == 0x78D0FE)	return TRUE;
	return FALSE;
}

BOOL InputIdAndPwd(CLoginData* pLoginData)
{
	DbgOutput("开始输入账号和密码\n");
	Sleep(100);
	KeyPress(VK_SHIFT);	//切换为英文输入法

	DbgOutput("输入账号\n");
	InputString(pLoginData->strAccount);	//输入账号
	KeyPress(VK_TAB);	//TAB切换到密码编辑框

	DbgOutput("输入密码\n");
	InputString(pLoginData->strPwd);		//输入密码
	KeyPress(VK_RETURN);	//Enter登录游戏
	Sleep(3000);

	DWORD dwBGR = GetPixel(hdcClient, 498, 432);	//取窗口客户区一个点的颜色BGR: E9EBEA
	DbgOutput("颜色BGR:%X\n", dwBGR);
	if (dwBGR == 0xE9EBEA)
	{
		DbgOutput("账号密码输入有误,自动登录失败,将退出游戏\n");
		MoveTo(507, 462, hWnd);
		LeftClick();
		Sleep(500);
		MoveTo(546, 652, hWnd);
		LeftClick();
		return FALSE;
	}
	while (dwBGR != 0x0000b4)
	{
		Sleep(500);
		dwBGR = GetPixel(hdcClient, 309, 385);
		DbgOutput("还未进入选线窗口,颜色BGR:%X\n", dwBGR);
	}
	return TRUE;
}

BOOL AutoLogin(CLoginData* pLoginData)
{
	// 启动登录器,并等待登录器初始化完成
	RunLaucher();
	while (hWnd == NULL)
	{
		Sleep(500);
		DbgOutput("登录器还未打开\n");
		hWnd = FindWindowA(NULL, "Yulgang_File_Update");
	}
	Sleep(1000);

	// 把登录器窗口前置
	SwitchToThisWindow(hWnd, TRUE);
	SelDaQuAndStart(pLoginData->niServer);	//选择区服

	// 一直等待直到可以输入账号密码
	while (!IsAbleToInputIdAndPwd())	Sleep(500);
	// 把游戏窗口前置
	hWnd = FindWindowA(NULL, "YB_OnlineClient");
	if (hWnd == NULL)	return FALSE;
	SwitchToThisWindow(hWnd, TRUE);
	// 输入账号和密码
	BOOL bRet = InputIdAndPwd(pLoginData);
	if (bRet == FALSE)	return FALSE;
	// 选线
	DbgOutput("y=%d\n", 437 + (pLoginData->niGameLine) * 21);
	MoveTo(613 + rnd(-20,20), 437 + (pLoginData->niGameLine) * 21, hWnd);
	LeftDoubleClick();
	// 等待直到可以选择游戏角色
	DWORD dwBGR = NULL;
	while (dwBGR != 0xa4bbbb)
	{
		Sleep(500);
		DbgOutput("颜色BGR:%X\n", dwBGR);
		dwBGR = GetPixel(hdcClient, 303, 336);
	}
	Sleep(500);
	
	MoveTo(180, 229 + (pLoginData->niRoleIndex) * 42, hWnd);
	LeftClick();
	Sleep(1000);
	// 进入游戏
	MoveTo(498, 722, hWnd);
	LeftClick();
	// 读取人物属性列表基址,判断是否为空,来检测是否进入游戏
	DWORD dwPid = 0;
	GetWindowThreadProcessId(hWnd, &dwPid);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid); 
	char buf[256] = {0};
	for(int i = 0; i < 60; ++i)
	{
		Sleep(1000);
		ReadProcessMemory(hProcess, (LPCVOID)Base_RoleProperty, buf, 20, NULL);
		DbgOutput("第 %d 秒, dwName = %s\n", i, buf);
		if (buf[0] == 0 && i > 50)
		{
			DbgOutput("超过50秒未正常进入游戏,自动登录失败\n");
			TerminateProcess(hProcess, -1);
			return FALSE;
		}
		else if (buf[0] != 0)
		{
			SetWindowTextA(hWnd, (LPCTSTR)buf);	//设置窗口名为人物角色名
			Sleep(2000);
			break;
		}
	}
	// 关闭句柄
	ReleaseDC(hWnd, hdcClient);
	return TRUE;
}



void CGameLoginDlg::OnCbnSelchangeCombo1()
{
	CComboBox* pCmbBox_DaQu = (CComboBox*)GetDlgItem(IDC_COMBO1);
	CComboBox* pCmbBox_Server = (CComboBox*)GetDlgItem(IDC_COMBO2);
	DWORD dwIndex = pCmbBox_DaQu->GetCurSel();
	switch (dwIndex)
	{
	case 0:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 魔神");
		pCmbBox_Server->AddString("2 龙争虎斗");
		pCmbBox_Server->AddString("3 冰宫");
		pCmbBox_Server->AddString("4 雪原");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 1:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 雄霸");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 2:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 登峰造极");
		pCmbBox_Server->AddString("2 剑魔之血");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 3:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 碧海");
		pCmbBox_Server->AddString("2 啸天");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 4:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 狂龙降天");
		pCmbBox_Server->AddString("2 幻影密路");
		pCmbBox_Server->AddString("3 长空");
		pCmbBox_Server->AddString("4 傲雪");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 5:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 锋芒毕露");
		pCmbBox_Server->AddString("2 东岳");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 6:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 玄湖");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 7:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 魅影");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 8:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 紫月");
		pCmbBox_Server->SetCurSel(0);
		break;
	case 9:
		pCmbBox_Server->ResetContent();
		pCmbBox_Server->AddString("1 霸业");
		pCmbBox_Server->AddString("2 苍月");
		pCmbBox_Server->AddString("3 星霜");
		pCmbBox_Server->SetCurSel(0);
		break;
	}
}
