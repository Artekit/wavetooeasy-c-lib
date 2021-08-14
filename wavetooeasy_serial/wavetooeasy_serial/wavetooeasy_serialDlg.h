
// wavetooeasy_serialDlg.h : header file
//

#pragma once
#include "enumser.h"
#include "Uart.h"

// CwavetooeasyserialDlg dialog
class CwavetooeasyserialDlg : public CDialogEx
{
// Construction
public:
	CwavetooeasyserialDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_WAVETOOEASY_SERIAL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	CEnumerateSerial m_enumserial;
	CUart m_uart;
	BOOL m_bConnected;

	static uint32_t SerialMillis()
	{
		return GetTickCount();
	}

	static void SerialTxCallbackStub(uint8_t* data, size_t size, void* param)
	{
		CwavetooeasyserialDlg* ptr = (CwavetooeasyserialDlg*) param;
		if (!ptr)
			return;

		ptr->SerialTxCallback(data, size);
	}

	static uint8_t SerialRxCallbackStub(uint8_t* data, void* param)
	{
		CwavetooeasyserialDlg* ptr = (CwavetooeasyserialDlg*)param;
		if (!ptr)
			return 0;

		return (uint8_t) ptr->SerialRxCallback(data, 1, 0);
	}

	void SerialTxCallback(uint8_t* data, size_t size);
	uint32_t SerialRxCallback(uint8_t* data, uint32_t size, uint32_t timeout);
	void ShowReceivedAnswer(uint32_t cmd, uint32_t answer, TCHAR* extra = NULL);
	uint8_t GetSelectedChannel();
	uint8_t GetSelectedMode();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedHello();
	CComboBox m_SerialPorts;
	CStatic m_lbl_answer;
	CStatic m_lbl_channels;
	afx_msg void OnBnClickedVersion();
	afx_msg void OnBnClickedStopAll();
	afx_msg void OnBnClickedPauseAll();
	afx_msg void OnBnClickedResumeAll();
	afx_msg void OnBnClickedSetSpeakerVol();
	afx_msg void OnBnClickedSetHeadphoneVol();
	afx_msg void OnBnClickedChannelPlay();
	afx_msg void OnBnClickedChannelPause();
	afx_msg void OnBnClickedChannelStop2();
	afx_msg void OnBnClickedChannelResume();
	afx_msg void OnBnClickedChannelStatus();
	afx_msg void OnBnClickedPlayFile();
	afx_msg void OnBnClickedChannelGetVolume();
	afx_msg void OnBnClickedChannelSetVolume();
	afx_msg void OnBnClickedPlayAll();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CEdit m_SpeakerVolumeBox;
	CEdit m_HeadphoneVolumeBox;
	CEdit m_PlayFileBox;
	CEdit m_ChannelVolumeBox;
	afx_msg void OnBnClickedChannel1();
	afx_msg void OnBnClickedChannel2();
	afx_msg void OnBnClickedChannel3();
	afx_msg void OnBnClickedChannel4();
	afx_msg void OnBnClickedChannel5();
	afx_msg void OnBnClickedChannel6();
	afx_msg void OnBnClickedChannel7();
	afx_msg void OnBnClickedChannel8();
	afx_msg void OnBnClickedChannel9();
	afx_msg void OnBnClickedChannel10();
	afx_msg void OnBnClickedGetSpeakerVol();
	afx_msg void OnBnClickedGetHeadphoneVol();
};
