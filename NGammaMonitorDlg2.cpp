#include "json/json.h"

#include "pch.h"
#include "framework.h"
#include "NGammaMonitor.h"
#include "NGammaMonitorDlg.h"
#include "afxdialogex.h"
#include "Order.h"
// 该文件存放不常用函数

// 文件操作
#include <iostream>
#include <fstream>
using namespace std;

void CNGammaMonitorDlg::PrintLog(CString info)
{
	CString strLog;
	GetDlgItemText(IDC_LOG, strLog);
	strLog = strLog + info + _T("\r\n");
	m_LogEditStr = strLog;
	UpdateData(FALSE);

	m_LogEdit.LineScroll(m_LogEdit.GetLineCount()); //每次刷新后都显示最底部
}

// 打开UDP通信
void CNGammaMonitorDlg::OpenUDP()
{
	UpdateData(TRUE); //读取界面控件的输入值
	// 读取配置参数并设置到相应控件上
	Json::Value jsonSetting = ReadSetting();
	//配置文件不存在，则生成配置文件
	jsonSetting["Port_UDP"] = m_UDPPort;
	WriteSetting(jsonSetting);

	//--------------1.创建UDPSocket------------
	if (!m_UDPSocket) delete m_UDPSocket;
	m_UDPSocket = new CClientSocket(this);//初始化,新创建一个对话框Socket
	m_UDPSocket->Create(m_UDPPort, SOCK_DGRAM, NULL);

	//--------------2.获取Socket绑定的ip和端口--------------
	//获取本机的IP和端口号
	CString strIp;
	UINT uiPort;

	//获取本地的服务号和端口号
	m_UDPSocket->GetSockName(strIp, uiPort);

	//显示本地的端口号和IP号
	SetDlgItemText(IDC_UDPIP, strIp);
	SetDlgItemInt(IDC_UDPPORT, uiPort);
	CString info;
	info.Format(_T("UDP已打开，端口号为:%d"), uiPort);
	PrintLog(info);
	m_page1->PrintLog(info);
	m_page2->PrintLog(info);
}

void CNGammaMonitorDlg::CloseUDP() {
	if (m_UDPSocket != NULL) delete m_UDPSocket;
	CString info = _T("UDP网络已关闭");
	PrintLog(info);
	m_page1->PrintLog(info);
	m_page2->PrintLog(info);
}

void CNGammaMonitorDlg::SaveFile(CString myID, const char* mk, int length) {
	/*
	CString tempTarget;
	tempTarget.Format(_T("%d"), myID);  //Use Unicode Character Set
	CString filename = tempTarget + _T(".txt");
	*/
	CString filename = myID + _T(".dat");

	CString wholePath = saveAsPath + filename;
	/*
	CStdioFile mFile;
	//打开文件
	mFile.Open(filename, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite); //追加方式打开文件
	mFile.Write(message, sizeof(message));
	mFile.Flush();
	mFile.Close();
	*/
	fstream datafile(wholePath, ios::out | ios::app | ios::binary);   // 追加
	for (int i = 0; i < length; i++) {
		datafile << mk[i];
	}
	datafile.close();
}

char* CNGammaMonitorDlg::CstringToWideCharArry(CString CstrText)
{
	int lth = WideCharToMultiByte(CP_ACP, 0, CstrText, CstrText.GetLength(), NULL, 0, NULL, NULL);
	char* pStr = (char*)malloc((lth + 1) * sizeof(char));
	ASSERT(pStr != NULL);
	memset(pStr, 0, (lth + 1) * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, CstrText.GetBuffer(), CstrText.GetLength(), (LPSTR)pStr, lth, NULL, NULL);
	*(pStr + lth + 1) = '\0';
	/*printf("lth=%d", lth);
	printf("lth+1=%d", (lth + 1) * sizeof(char));
	printf("LthStr=%d", sizeof(CString));
	printf("CharLen=%d", sizeof(char*));
	*/
	return pStr;
}

Json::Value CNGammaMonitorDlg::ReadSetting()
{
	Json::Value root;
	std::ifstream ifs;
	ifs.open("Setting.json");
	
	Json::CharReaderBuilder builder;
	builder["collectComments"] = true;
	JSONCPP_STRING errs;
	if (!parseFromStream(builder, ifs, &root, &errs)) {
		return root;
	}
	return root;
	/*
	Json::Reader reader;
	Json::Value root;

	//从文件中读取，保证当前文件有demo.json文件
	ifstream in("Setting.json", ios::binary);
	if (!in.is_open())
	{
		cout << "Error opening file\n";
		return root;
	}

	if (reader.parse(in, root))
	{
		//读取根节点信息
		string IP = root["IP_Detector"].asString();
		int port = root["Port_Detector"].asInt();


		//读取子节点信息
		string friend_name = root["friends"]["friend_name"].asString();
		int friend_age = root["friends"]["friend_age"].asInt();
		string friend_sex = root["friends"]["friend_sex"].asString();

		//读取数组信息
		for (unsigned int i = 0; i < root["hobby"].size(); i++)
		{
			string ach = root["hobby"][i].asString();
		}
	}
	in.close();
	return root;
	*/
}

// 写入配置文件，实际上是修改配置文件
void CNGammaMonitorDlg::WriteSetting(Json::Value myJson)
{
	ofstream os;
	os.open("Setting.json", std::ios::out);
	Json::StreamWriterBuilder sw;
	const std::unique_ptr<Json::StreamWriter> writer(sw.newStreamWriter());
	if (os.is_open())
	{
		writer->write(myJson, &os);
	}
	os.close();
}

//限制端口输入范围0~65535
void CNGammaMonitorDlg::OnEnKillfocusPort1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if ((sPort < 0) || (sPort > 65535))
	{
		MessageBox(_T("端口的范围为0~65535\n"));
		if (sPort > 65535)
		{
			sPort = 65535;
		}
		else
		{
			sPort = 1;
		}
		UpdateData(false);
	}
}

//限制端口输入范围0~65535
void CNGammaMonitorDlg::OnEnKillfocusUDPPort()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if ((sPort < 0) || (sPort > 65535))
	{
		MessageBox(_T("端口的范围为0~65535\n"));
		if (sPort > 65535)
		{
			sPort = 65535;
		}
		else
		{
			sPort = 1;
		}
		UpdateData(false);
	}
}

// 设置文件存储路径
void CNGammaMonitorDlg::OnBnClickedSaveas()
{
	// TODO: 在此添加控件通知处理程序代码
	//获取目录路径
	TCHAR   szPath[MAX_PATH] = { 0 };
	LPITEMIDLIST   pitem;
	BROWSEINFO   info;
	::ZeroMemory(&info, sizeof(info));
	info.hwndOwner = this->m_hWnd;
	info.lpszTitle = _T("请选择路径: ");
	info.pszDisplayName = szPath;
	if (pitem = ::SHBrowseForFolder(&info))
	{
		::SHGetPathFromIDList(pitem, szPath);
		saveAsPath = szPath;
		CString str = saveAsPath += "\\";
		CString info = _T("实验数据保存路径：") + str;
		PrintLog(info);
		UpdateData(FALSE);
	}
}
