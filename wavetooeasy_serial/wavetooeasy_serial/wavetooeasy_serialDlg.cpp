
// wavetooeasy_serialDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "wavetooeasy_serial.h"
#include "wavetooeasy_serialDlg.h"
#include "afxdialogex.h"
#include "..\..\WaveTooEasy_protocol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CwavetooeasyserialDlg dialog



CwavetooeasyserialDlg::CwavetooeasyserialDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WAVETOOEASY_SERIAL_DIALOG, pParent), m_bConnected(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CwavetooeasyserialDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERIAL_PORTS, m_SerialPorts);
	DDX_Control(pDX, IDC_LBL_ANSWER, m_lbl_answer);
	DDX_Control(pDX, IDC_LBL_CHANNELS, m_lbl_channels);
	DDX_Control(pDX, IDC_SPEAKER_VOL, m_SpeakerVolumeBox);
	DDX_Control(pDX, IDC_HEADPHONE_VOL, m_HeadphoneVolumeBox);
	DDX_Control(pDX, IDC_FILE_PATH, m_PlayFileBox);
	DDX_Control(pDX, IDC_CHANNEL_VOLUME, m_ChannelVolumeBox);
}

BEGIN_MESSAGE_MAP(CwavetooeasyserialDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CwavetooeasyserialDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CwavetooeasyserialDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CONNECT, &CwavetooeasyserialDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_HELLO, &CwavetooeasyserialDlg::OnBnClickedHello)
	ON_BN_CLICKED(IDC_VERSION, &CwavetooeasyserialDlg::OnBnClickedVersion)
	ON_BN_CLICKED(IDC_STOP_ALL, &CwavetooeasyserialDlg::OnBnClickedStopAll)
	ON_BN_CLICKED(IDC_PAUSE_ALL, &CwavetooeasyserialDlg::OnBnClickedPauseAll)
	ON_BN_CLICKED(IDC_RESUME_ALL, &CwavetooeasyserialDlg::OnBnClickedResumeAll)
	ON_BN_CLICKED(IDC_SET_SPEAKER_VOL, &CwavetooeasyserialDlg::OnBnClickedSetSpeakerVol)
	ON_BN_CLICKED(IDC_SET_HEADPHONE_VOL, &CwavetooeasyserialDlg::OnBnClickedSetHeadphoneVol)
	ON_BN_CLICKED(IDC_CHANNEL_PLAY, &CwavetooeasyserialDlg::OnBnClickedChannelPlay)
	ON_BN_CLICKED(IDC_CHANNEL_PAUSE, &CwavetooeasyserialDlg::OnBnClickedChannelPause)
	ON_BN_CLICKED(IDC_CHANNEL_STOP2, &CwavetooeasyserialDlg::OnBnClickedChannelStop2)
	ON_BN_CLICKED(IDC_CHANNEL_RESUME, &CwavetooeasyserialDlg::OnBnClickedChannelResume)
	ON_BN_CLICKED(IDC_CHANNEL_STATUS, &CwavetooeasyserialDlg::OnBnClickedChannelStatus)
	ON_BN_CLICKED(IDC_PLAY_FILE, &CwavetooeasyserialDlg::OnBnClickedPlayFile)
	ON_BN_CLICKED(IDC_CHANNEL_GET_VOLUME, &CwavetooeasyserialDlg::OnBnClickedChannelGetVolume)
	ON_BN_CLICKED(IDC_CHANNEL_SET_VOLUME, &CwavetooeasyserialDlg::OnBnClickedChannelSetVolume)
	ON_BN_CLICKED(IDC_PLAY_ALL, &CwavetooeasyserialDlg::OnBnClickedPlayAll)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHANNEL_1, &CwavetooeasyserialDlg::OnBnClickedChannel1)
	ON_BN_CLICKED(IDC_CHANNEL_2, &CwavetooeasyserialDlg::OnBnClickedChannel2)
	ON_BN_CLICKED(IDC_CHANNEL_3, &CwavetooeasyserialDlg::OnBnClickedChannel3)
	ON_BN_CLICKED(IDC_CHANNEL_4, &CwavetooeasyserialDlg::OnBnClickedChannel4)
	ON_BN_CLICKED(IDC_CHANNEL_5, &CwavetooeasyserialDlg::OnBnClickedChannel5)
	ON_BN_CLICKED(IDC_CHANNEL_6, &CwavetooeasyserialDlg::OnBnClickedChannel6)
	ON_BN_CLICKED(IDC_CHANNEL_7, &CwavetooeasyserialDlg::OnBnClickedChannel7)
	ON_BN_CLICKED(IDC_CHANNEL_8, &CwavetooeasyserialDlg::OnBnClickedChannel8)
	ON_BN_CLICKED(IDC_CHANNEL_9, &CwavetooeasyserialDlg::OnBnClickedChannel9)
	ON_BN_CLICKED(IDC_CHANNEL_10, &CwavetooeasyserialDlg::OnBnClickedChannel10)
	ON_BN_CLICKED(IDC_GET_SPEAKER_VOL, &CwavetooeasyserialDlg::OnBnClickedGetSpeakerVol)
	ON_BN_CLICKED(IDC_GET_HEADPHONE_VOL, &CwavetooeasyserialDlg::OnBnClickedGetHeadphoneVol)
END_MESSAGE_MAP()

#define	SelectCBox(x,y)			::SendMessage(::GetDlgItem(this->m_hWnd,x),CB_SETCURSEL,y,0L)
#define	CheckCButton(x,y)		::SendMessage(::GetDlgItem(this->m_hWnd,x),BM_SETCHECK,y,0L)
#define	SetCText(x,y)			::SetWindowText(::GetDlgItem(this->m_hWnd,x),y)
#define	GetCText(x,y)			::GetWindowText(::GetDlgItem(this->m_hWnd,x),y,sizeof(y) * 2)
#define	GetCButton(x)			::SendMessage(::GetDlgItem(this->m_hWnd,x),BM_GETCHECK,0,0L)

// CwavetooeasyserialDlg message handlers

BOOL CwavetooeasyserialDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CEnumerateSerial::CNamesArray names;
	m_enumserial.UsingRegistry(names);

	for (const auto& name : names)
		m_SerialPorts.AddString(name.c_str());

	m_SerialPorts.SetCurSel(0);
	CheckCButton(IDC_CHANNEL_1, TRUE);
	CheckCButton(IDC_PLAY_NORMAL, TRUE);
	SetCText(IDC_SPEAKER_VOL, L"0.00");
	SetCText(IDC_HEADPHONE_VOL, L"0.00");
	SetCText(IDC_FILE_PATH, L"test/song.wav");
	SetCText(IDC_CHANNEL_VOLUME, L"1.00");
	SetCText(IDC_BAUDRATE, L"460800");
	
	return TRUE;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CwavetooeasyserialDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CwavetooeasyserialDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CwavetooeasyserialDlg::SerialTxCallback(uint8_t* data, size_t size)
{
	m_uart.Write(data, (uint32_t) size);
}

uint32_t CwavetooeasyserialDlg::SerialRxCallback(uint8_t* data, uint32_t size, uint32_t timeout)
{
	return m_uart.Read(data, 1, timeout);
}

void CwavetooeasyserialDlg::ShowReceivedAnswer(uint32_t cmd, uint32_t answer, TCHAR* extra)
{
	CString szCmd;
	CString szAnswer;

	switch (cmd)
	{
		case CMD_HELLO: szCmd = L"CMD_HELLO"; break;
		case CMD_VERSION: szCmd = L"CMD_VERSION"; break;
		case CMD_PLAY_FILE: szCmd = L"CMD_PLAY_FILE"; break;
		case CMD_PLAY_CHANNEL: szCmd = L"CMD_PLAY_CHANNEL"; break;
		case CMD_STOP_ALL: szCmd = L"CMD_STOP_ALL"; break;
		case CMD_STOP: szCmd = L"CMD_STOP"; break;
		case CMD_PAUSE: szCmd = L"CMD_PAUSE"; break;
		case CMD_PAUSE_ALL: szCmd = L"CMD_PAUSE_ALL"; break;
		case CMD_RESUME: szCmd = L"CMD_RESUME"; break;
		case CMD_RESUME_ALL: szCmd = L"CMD_RESUME_ALL"; break;
		case CMD_CHANNELS_STATUS: szCmd = L"CMD_CHANNELS_STATUS"; break;
		case CMD_CHANNEL_STATUS: szCmd = L"CMD_CHANNEL_STATUS"; break;
		case CMD_GET_CHANNEL_VOL: szCmd = L"CMD_GET_CHANNEL_VOL"; break;
		case CMD_SET_CHANNEL_VOL: szCmd = L"CMD_SET_CHANNEL_VOL"; break;
		case CMD_SET_SPEAKERS_VOL: szCmd = L"CMD_SET_SPEAKERS_VOL"; break;
		case CMD_SET_HEADPHONE_VOL: szCmd = L"CMD_SET_HEADPHONE_VOL"; break;
		case CMD_GET_SPEAKERS_VOL: szCmd = L"CMD_GET_SPEAKERS_VOL"; break;
		case CMD_GET_HEADPHONE_VOL: szCmd = L"CMD_GET_HEADPHONE_VOL"; break;
		default: szCmd = L"Unknown command"; break;
	}

	switch (answer)
	{
		case ERROR_NONE: szAnswer = "ERROR_NONE"; break;
		case ERROR_INVALID_LENGTH: szAnswer = "ERROR_INVALID_LENGTH"; break;
		case ERROR_INVALID_FILE_LENGTH: szAnswer = "ERROR_INVALID_FILE_LENGTH"; break;
		case ERROR_INVALID_CHANNEL: szAnswer = "ERROR_INVALID_CHANNEL"; break;
		case ERROR_INVALID_MODE: szAnswer = "ERROR_INVALID_MODE"; break;
		case ERROR_INTERNAL: szAnswer = "ERROR_INTERNAL"; break;
		case ERROR_PLAYING: szAnswer = "ERROR_PLAYING"; break;
		case ERROR_NOT_PAUSED: szAnswer = "ERROR_NOT_PAUSED"; break;
		case ERROR_NOT_PLAYING: szAnswer = "ERROR_NOT_PLAYING"; break;
		case ERROR_ON_RX: szAnswer = "ERROR_ON_RX"; break;
		case ERROR_RX_TIMEOUT: szAnswer = "ERROR_RX_TIMEOUT"; break;
		case ERROR_PARAM: szAnswer = "ERROR_PARAM"; break;
		default: szAnswer = L"Unknown answer"; break;
	}

	CString str;

	str.Format(L"%s => %s", szCmd.GetBuffer(), szAnswer.GetBuffer());
	if (extra)
	{
		str.Append(L" (");
		str.Append(extra);
		str.Append(L")");
	}
	m_lbl_answer.SetWindowText(str);
}

uint8_t CwavetooeasyserialDlg::GetSelectedMode()
{
	if (GetCButton(IDC_PLAY_NORMAL)) return 0;
	if (GetCButton(IDC_PLAY_LOOP)) return 1;
	return 0xFF;
}

uint8_t CwavetooeasyserialDlg::GetSelectedChannel()
{
	if (GetCButton(IDC_CHANNEL_1)) return 1;
	if (GetCButton(IDC_CHANNEL_2)) return 2;
	if (GetCButton(IDC_CHANNEL_3)) return 3;
	if (GetCButton(IDC_CHANNEL_4)) return 4;
	if (GetCButton(IDC_CHANNEL_5)) return 5;
	if (GetCButton(IDC_CHANNEL_6)) return 6;
	if (GetCButton(IDC_CHANNEL_7)) return 7;
	if (GetCButton(IDC_CHANNEL_8)) return 8;
	if (GetCButton(IDC_CHANNEL_9)) return 9;
	if (GetCButton(IDC_CHANNEL_10)) return 10;
	return 0;
}

void CwavetooeasyserialDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}


void CwavetooeasyserialDlg::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnCancel();
}

void CwavetooeasyserialDlg::OnBnClickedConnect()
{
	TCHAR port[32];
	TCHAR baudrate[32];

	if (m_bConnected)
	{
		m_uart.Reset();
		return;
	}

	GetCText(IDC_SERIAL_PORTS, port);
	GetCText(IDC_BAUDRATE, baudrate);

	if (!wcslen(port) || !wcslen(baudrate))
		return;

	if (!m_uart.Open(port, _wtoi(baudrate)))
	{
		AfxMessageBox(L"Cannot open COM port");
		return;
	}

	wteInit(SerialMillis, SerialRxCallbackStub, SerialTxCallbackStub, this);

	uint32_t res = wteHello();

	if (res == ERROR_NONE)
	{
		m_bConnected = true;
		SetTimer(1, 500, NULL);
	} else {
		m_uart.Close();
		ShowReceivedAnswer(CMD_HELLO, res);
		return;
	}

	ShowReceivedAnswer(CMD_HELLO, res);

	OnBnClickedChannelGetVolume();
	OnBnClickedGetSpeakerVol();
	OnBnClickedGetHeadphoneVol();
}

void CwavetooeasyserialDlg::OnBnClickedHello()
{
	uint32_t res = wteHello();
	ShowReceivedAnswer(CMD_HELLO, res);
}

void CwavetooeasyserialDlg::OnBnClickedVersion()
{
	CString str;
	uint8_t major, minor, fix;
	uint32_t res = wteGetVersion(&major, &minor, &fix);
	if (res == ERROR_NONE)
	{
		str.Format(L"%i.%i.%i", major, minor, fix);
		ShowReceivedAnswer(CMD_VERSION, res, str.GetBuffer());
	} else {
		ShowReceivedAnswer(CMD_VERSION, res);
	}
}


void CwavetooeasyserialDlg::OnBnClickedStopAll()
{
	uint32_t res = wteStopAll();
	ShowReceivedAnswer(CMD_STOP_ALL, res);
}


void CwavetooeasyserialDlg::OnBnClickedPauseAll()
{
	uint32_t res = wtePauseAll();
	ShowReceivedAnswer(CMD_PAUSE_ALL, res);
}


void CwavetooeasyserialDlg::OnBnClickedResumeAll()
{
	uint32_t res = wteResumeAll();
	ShowReceivedAnswer(CMD_RESUME_ALL, res);
}


void CwavetooeasyserialDlg::OnBnClickedSetSpeakerVol()
{
	CString str;
	float volume;
	m_SpeakerVolumeBox.GetWindowTextW(str);
	if (str.IsEmpty())
		return;

	volume = (float) _wtof(str);
	
	uint32_t res = wteSetSpeakersVolume(volume);
	ShowReceivedAnswer(CMD_SET_SPEAKERS_VOL, res);
}


void CwavetooeasyserialDlg::OnBnClickedSetHeadphoneVol()
{
	CString str;
	float volume;
	m_HeadphoneVolumeBox.GetWindowTextW(str);
	if (str.IsEmpty())
		return;

	volume = (float) _wtof(str);

	uint32_t res = wteSetHeadphoneVolume(volume);
	ShowReceivedAnswer(CMD_SET_HEADPHONE_VOL, res);
}


void CwavetooeasyserialDlg::OnBnClickedChannelPlay()
{
	uint8_t channel = GetSelectedChannel();
	if (!channel)
		return;

	uint8_t mode = GetSelectedMode();
	if (mode == 0xFF)
		return;

	uint8_t res = wtePlayChannel(channel, mode);
	ShowReceivedAnswer(CMD_PLAY_CHANNEL, res);
}


void CwavetooeasyserialDlg::OnBnClickedChannelPause()
{
	uint8_t channel = GetSelectedChannel();
	if (!channel)
		return;

	uint8_t res = wtePauseChannel(channel);
	ShowReceivedAnswer(CMD_PAUSE, res);
}


void CwavetooeasyserialDlg::OnBnClickedChannelStop2()
{
	uint8_t channel = GetSelectedChannel();
	if (!channel)
		return;

	uint8_t res = wteStopChannel(channel);
	ShowReceivedAnswer(CMD_STOP, res);
}


void CwavetooeasyserialDlg::OnBnClickedChannelResume()
{
	uint8_t channel = GetSelectedChannel();
	if (!channel)
		return;

	uint8_t res = wteResumeChannel(channel);
	ShowReceivedAnswer(CMD_RESUME, res);
}

void CwavetooeasyserialDlg::OnBnClickedChannelStatus()
{
	uint8_t channel = GetSelectedChannel();
	uint8_t status;

	if (!channel)
		return;

	uint8_t res = wteGetChannelStatus(channel, &status);
	
	if (res == ERROR_NONE)
	{
		CString extra;
		switch (status)
		{
			case 0: extra = "Stopped"; break;
			case 1: extra = "Playing"; break;
			case 2: extra = "Paused"; break;
			default: extra = "Unknown"; break;
		}

		ShowReceivedAnswer(CMD_CHANNEL_STATUS, res, extra.GetBuffer());
	} else {
		ShowReceivedAnswer(CMD_CHANNEL_STATUS, res);
	}
}


void CwavetooeasyserialDlg::OnBnClickedPlayFile()
{
	CString str;
	m_PlayFileBox.GetWindowTextW(str);
	if (str.IsEmpty())
		return;

	uint8_t channel = GetSelectedChannel();
	if (!channel)
		return;

	uint8_t mode = GetSelectedMode();
	if (mode == 0xFF)
		return;

	char file[WTE_MAX_PACKET_DATA_SIZE];

	if (str.GetLength() > WTE_MAX_PACKET_DATA_SIZE)
		return;

	wcstombs_s(NULL, file, WTE_MAX_PACKET_DATA_SIZE, str, WTE_MAX_PACKET_DATA_SIZE);

	uint8_t res = wtePlayFile(file, channel, mode);
	ShowReceivedAnswer(CMD_PLAY_FILE, res);
}


void CwavetooeasyserialDlg::OnBnClickedChannelGetVolume()
{
	uint8_t channel = GetSelectedChannel();
	if (!channel)
		return;

	float volume;
	uint8_t res = wteGetChannelVolume(channel, &volume);
	ShowReceivedAnswer(CMD_GET_CHANNEL_VOL, res);
	if (res != ERROR_NONE)
		return;

	CString str;
	str.Format(L"%2.2f", volume);
	m_ChannelVolumeBox.SetWindowTextW(str);
}


void CwavetooeasyserialDlg::OnBnClickedChannelSetVolume()
{
	uint8_t channel = GetSelectedChannel();
	if (!channel)
		return;

	CString str;
	m_ChannelVolumeBox.GetWindowTextW(str);
	if (str.IsEmpty())
		return;

	float volume = (float)_wtof(str);
	uint8_t res = wteSetChannelVolume(channel, volume);
	
	ShowReceivedAnswer(CMD_SET_CHANNEL_VOL, res);	
}


void CwavetooeasyserialDlg::OnBnClickedPlayAll()
{	
	uint8_t mode = GetSelectedMode();
	if (mode == 0xFF)
		return;

	for (uint8_t i = 1; i <= 10; i++)
	{
		uint8_t res = wtePlayChannel(i, mode);
		ShowReceivedAnswer(CMD_PLAY_CHANNEL, res);
		Sleep(10);
	}
}


void CwavetooeasyserialDlg::OnTimer(UINT_PTR nIDEvent)
{
	WTE_CHANNELS_STATUS status;
	uint8_t res = wteGetAllChannelsStatus(&status);
	if (res == ERROR_NONE)
	{
		CString str;
		str.Format(L"%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s", status.channel1 == 0 ? L"[]" : (status.channel1 == 1 ? L">" : L"||"),
			status.channel2 == 0 ? L"[]" : (status.channel2 == 1 ? L">" : L"||"),
			status.channel3 == 0 ? L"[]" : (status.channel3 == 1 ? L">" : L"||"),
			status.channel4 == 0 ? L"[]" : (status.channel4 == 1 ? L">" : L"||"),
			status.channel5 == 0 ? L"[]" : (status.channel5 == 1 ? L">" : L"||"),
			status.channel6 == 0 ? L"[]" : (status.channel6 == 1 ? L">" : L"||"),
			status.channel7 == 0 ? L"[]" : (status.channel7 == 1 ? L">" : L"||"),
			status.channel8 == 0 ? L"[]" : (status.channel8 == 1 ? L">" : L"||"),
			status.channel9 == 0 ? L"[]" : (status.channel9 == 1 ? L">" : L"||"),
			status.channel10 == 0 ? L"[]" : (status.channel10 == 1 ? L">" : L"||")
			);

		m_lbl_channels.SetWindowText(str);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CwavetooeasyserialDlg::OnBnClickedChannel1()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel2()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel3()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel4()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel5()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel6()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel7()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel8()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel9()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedChannel10()
{
	OnBnClickedChannelGetVolume();
}


void CwavetooeasyserialDlg::OnBnClickedGetSpeakerVol()
{
	float volume;
	uint8_t res = wteGetSpeakersVolume(&volume);
	ShowReceivedAnswer(CMD_GET_SPEAKERS_VOL, res);
	if (res != ERROR_NONE)
		return;

	CString str;
	str.Format(L"%2.2f", volume);
	m_SpeakerVolumeBox.SetWindowTextW(str);
}


void CwavetooeasyserialDlg::OnBnClickedGetHeadphoneVol()
{
	float volume;
	uint8_t res = wteGetHeadphoneVolume(&volume);
	ShowReceivedAnswer(CMD_GET_HEADPHONE_VOL, res);
	if (res != ERROR_NONE)
		return;

	CString str;
	str.Format(L"%2.2f", volume);
	m_HeadphoneVolumeBox.SetWindowTextW(str);
}
