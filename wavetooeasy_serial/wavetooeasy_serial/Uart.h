#pragma once
#include <Windows.h>
#include <stdint.h>

#define UART_TX_BUFFER_SIZE	512
#define UART_RX_BUFFER_SIZE	512

class CUart
{
public:
	CUart();
	~CUart();

	bool Open(const TCHAR* szCom, DWORD dwBaudrate);
	DWORD Write(BYTE* pBuf, DWORD dwBufLen);
	DWORD Read(BYTE* pBuf, DWORD dwLen, DWORD dwTimeout);
	void Close();
	uint32_t Available();
	bool WaitForData(DWORD timeout);
	void Reset();

private:
	volatile bool m_bOpened;
	OVERLAPPED	m_wro;
	OVERLAPPED	m_rdo;
	HANDLE		m_hCom;
	BYTE		m_ucTxBuffer[UART_TX_BUFFER_SIZE];
	BYTE		m_ucRxBuffer[UART_RX_BUFFER_SIZE];
	DWORD		m_dwRxRd;
	DWORD		m_dwRxWr;
	DWORD		m_dwRxCount;
	DWORD		m_dwTxRd;
	DWORD		m_dwTxWr;
	DWORD		m_dwTxCount;
	CRITICAL_SECTION m_csTx;
	CRITICAL_SECTION m_csRx;
	HANDLE		m_hTxThread;
	HANDLE		m_hRxThread;
	HANDLE		m_hTxEvent;
	HANDLE		m_hRxEvent;
	

	DWORD		PullFromRx(BYTE* pBuf, DWORD dwSize, DWORD dwTimeout);
	DWORD		RxThread();
	DWORD		TxThread();

	static DWORD WINAPI _RxThread(LPVOID pParam)
	{
		CUart* ptr = (CUart*)pParam;
		return ptr->RxThread();
	}

	static DWORD WINAPI _TxThread(LPVOID pParam)
	{
		CUart* ptr = (CUart*)pParam;
		return ptr->TxThread();
	}
};
