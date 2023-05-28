#include "json/json.h"

#include "pch.h"
#include "framework.h"
#include "NGammaMonitor.h"
#include "NGammaMonitorDlg.h"
#include "afxdialogex.h"
#include "Order.h"
// ���ļ���Ų����ú���

// �ļ�����
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

	m_LogEdit.LineScroll(m_LogEdit.GetLineCount()); //ÿ��ˢ�º���ʾ��ײ�
}

// ��UDPͨ��
void CNGammaMonitorDlg::OpenUDP()
{
	UpdateData(TRUE); //��ȡ����ؼ�������ֵ
	// ��ȡ���ò��������õ���Ӧ�ؼ���
	Json::Value jsonSetting = ReadSetting();
	//�����ļ������ڣ������������ļ�
	jsonSetting["Port_UDP"] = m_UDPPort;
	WriteSetting(jsonSetting);

	//--------------1.����UDPSocket------------
	if (!m_UDPSocket) delete m_UDPSocket;
	m_UDPSocket = new CClientSocket(this);//��ʼ��,�´���һ���Ի���Socket
	m_UDPSocket->Create(m_UDPPort, SOCK_DGRAM, NULL);

	//--------------2.��ȡSocket�󶨵�ip�Ͷ˿�--------------
	//��ȡ������IP�Ͷ˿ں�
	CString strIp;
	UINT uiPort;

	//��ȡ���صķ���źͶ˿ں�
	m_UDPSocket->GetSockName(strIp, uiPort);

	//��ʾ���صĶ˿ںź�IP��
	SetDlgItemText(IDC_UDPIP, strIp);
	SetDlgItemInt(IDC_UDPPORT, uiPort);
	CString info;
	info.Format(_T("UDP�Ѵ򿪣��˿ں�Ϊ:%d"), uiPort);
	PrintLog(info);
	m_page1->PrintLog(info);
	m_page2->PrintLog(info);
}

void CNGammaMonitorDlg::CloseUDP() {
	if (m_UDPSocket != NULL) delete m_UDPSocket;
	CString info = _T("UDP�����ѹر�");
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
	//���ļ�
	mFile.Open(filename, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite); //׷�ӷ�ʽ���ļ�
	mFile.Write(message, sizeof(message));
	mFile.Flush();
	mFile.Close();
	*/
	fstream datafile(wholePath, ios::out | ios::app | ios::binary);   // ׷��
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

	//���ļ��ж�ȡ����֤��ǰ�ļ���demo.json�ļ�
	ifstream in("Setting.json", ios::binary);
	if (!in.is_open())
	{
		cout << "Error opening file\n";
		return root;
	}

	if (reader.parse(in, root))
	{
		//��ȡ���ڵ���Ϣ
		string IP = root["IP_Detector"].asString();
		int port = root["Port_Detector"].asInt();


		//��ȡ�ӽڵ���Ϣ
		string friend_name = root["friends"]["friend_name"].asString();
		int friend_age = root["friends"]["friend_age"].asInt();
		string friend_sex = root["friends"]["friend_sex"].asString();

		//��ȡ������Ϣ
		for (unsigned int i = 0; i < root["hobby"].size(); i++)
		{
			string ach = root["hobby"][i].asString();
		}
	}
	in.close();
	return root;
	*/
}

// д�������ļ���ʵ�������޸������ļ�
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

//���ƶ˿����뷶Χ0~65535
void CNGammaMonitorDlg::OnEnKillfocusPort1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	if ((sPort < 0) || (sPort > 65535))
	{
		MessageBox(_T("�˿ڵķ�ΧΪ0~65535\n"));
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

//���ƶ˿����뷶Χ0~65535
void CNGammaMonitorDlg::OnEnKillfocusUDPPort()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	if ((sPort < 0) || (sPort > 65535))
	{
		MessageBox(_T("�˿ڵķ�ΧΪ0~65535\n"));
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

// �����ļ��洢·��
void CNGammaMonitorDlg::OnBnClickedSaveas()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//��ȡĿ¼·��
	TCHAR   szPath[MAX_PATH] = { 0 };
	LPITEMIDLIST   pitem;
	BROWSEINFO   info;
	::ZeroMemory(&info, sizeof(info));
	info.hwndOwner = this->m_hWnd;
	info.lpszTitle = _T("��ѡ��·��: ");
	info.pszDisplayName = szPath;
	if (pitem = ::SHBrowseForFolder(&info))
	{
		::SHGetPathFromIDList(pitem, szPath);
		saveAsPath = szPath;
		CString str = saveAsPath += "\\";
		CString info = _T("ʵ�����ݱ���·����") + str;
		PrintLog(info);
		UpdateData(FALSE);
	}
}
