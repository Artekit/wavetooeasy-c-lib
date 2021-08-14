#include "pch.h"
#include "Uart.h"

CUart::CUart()
{
	m_bOpened = FALSE;

	memset(&m_rdo, 0, sizeof(OVERLAPPED));
	m_rdo.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	memset(&m_wro, 0, sizeof(OVERLAPPED));
	m_wro.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	memset(m_ucTxBuffer, 0, sizeof(m_ucTxBuffer));
	memset(m_ucRxBuffer, 0, sizeof(m_ucRxBuffer));
	m_dwRxRd = m_dwRxWr = m_dwRxCount = m_dwTxRd = m_dwTxWr = m_dwTxCount = 0;
	InitializeCriticalSection(&m_csTx);
	InitializeCriticalSection(&m_csRx);
	m_hTxThread = m_hRxThread = NULL;

	m_hTxEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_hRxEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
}

CUart::~CUart()
{
	Close();
	if (m_rdo.hEvent)
		CloseHandle(m_rdo.hEvent);
	if (m_wro.hEvent)
		CloseHandle(m_wro.hEvent);
	if (m_hTxEvent)
		CloseHandle(m_hTxEvent);
	if (m_hRxEvent)
		CloseHandle(m_hRxEvent);
}

bool CUart::Open(const TCHAR* szCom, DWORD dwBaudrate)
{
	DCB		dcb;
	CString str;
	CString szSerialPort;

	if (m_bOpened) return FALSE;

	szSerialPort.Format(L"\\\\.\\%s", szCom);

	memset(&dcb, 0, sizeof(dcb));
	m_hCom = CreateFile(szSerialPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	if (m_hCom == INVALID_HANDLE_VALUE)
		return false;

	str.Format(L"baud=%lu parity=N data=8 stop=1", dwBaudrate);

	dcb.DCBlength = sizeof(dcb);
	if (!BuildCommDCB(str, &dcb)) {
		CloseHandle(m_hCom);
		m_hCom = NULL;
		return(FALSE);
	}
	dcb.fBinary = 1;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	if (!SetCommState(m_hCom, &dcb)) {
		CloseHandle(m_hCom);
		m_hCom = NULL;
		return(FALSE);
	}

	SetupComm(m_hCom, 4096, 4096);
	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);

	SetCommMask(m_hCom, EV_RXCHAR);

	m_bOpened = TRUE;
	m_hRxThread = CreateThread(NULL, 0, _RxThread, this, NULL, NULL);
	m_hTxThread = CreateThread(NULL, 0, _TxThread, this, NULL, NULL);
	m_dwRxRd = m_dwRxWr = m_dwRxCount = m_dwTxRd = m_dwTxWr = m_dwTxCount = 0;
	return true;
}

DWORD CUart::RxThread()
{
	SetCommMask(m_hCom, EV_RXCHAR);
	DWORD dwEvent = 0;

	while (m_bOpened)
	{
		ResetEvent(m_rdo.hEvent);
		WaitCommEvent(m_hCom, &dwEvent, NULL);
		if (dwEvent & EV_RXCHAR)
		{
			DWORD dwError;
			COMSTAT comstat;
			DWORD dwAvail;
			BOOL bResult = ClearCommError(m_hCom, &dwError, &comstat);
			if (!bResult || comstat.cbInQue == 0)
				continue;

			dwAvail = comstat.cbInQue;

			DWORD dwRead = 0;
			DWORD dwTotal = 0;

			EnterCriticalSection(&m_csRx);
			DWORD dwBufferSpace = UART_RX_BUFFER_SIZE - m_dwRxCount;
			LeaveCriticalSection(&m_csRx);

			if (!dwBufferSpace)
				continue;

			DWORD dwToRead;

			if (dwAvail > dwBufferSpace)
				dwAvail = dwBufferSpace;

			while (dwAvail)
			{
				if (m_dwRxWr + dwAvail > UART_RX_BUFFER_SIZE)
					dwToRead = UART_RX_BUFFER_SIZE - m_dwRxWr;
				else
					dwToRead = dwAvail;

				if (!ReadFile(m_hCom, m_ucRxBuffer + m_dwRxWr, dwToRead, &dwRead, &m_rdo))
				{
					DWORD dwError = GetLastError();
					if (dwError == ERROR_IO_PENDING)
					{
						BOOL bWaiting = TRUE;
						while (bWaiting && m_bOpened)
						{
							DWORD dwRes = WaitForSingleObject(m_rdo.hEvent, 500);
							switch (dwRes)
							{
								case WAIT_OBJECT_0:
									GetOverlappedResult(m_hCom, &m_rdo, &dwRead, FALSE);
									bWaiting = FALSE;
									break;

								case WAIT_TIMEOUT:
								default:
									break;
							}
						}
					}

					if (!m_bOpened)
						break;
				}

				dwAvail -= dwRead;
				m_dwRxWr += dwRead;
				if (m_dwRxWr == UART_RX_BUFFER_SIZE)
					m_dwRxWr = 0;

				dwTotal += dwRead;
			}

			EnterCriticalSection(&m_csRx);
			m_dwRxCount += dwTotal;
			LeaveCriticalSection(&m_csRx);

			SetEvent(m_hRxEvent);
		}
	}

	return 0;
}

DWORD CUart::TxThread()
{
	while (m_bOpened)
	{
		WaitForSingleObject(m_hTxEvent, INFINITE);
		if (!m_bOpened)
			break;

		ResetEvent(m_hTxEvent);

		DWORD dwCount, dWTotal;

		EnterCriticalSection(&m_csTx);
		dWTotal = dwCount = m_dwTxCount;
		LeaveCriticalSection(&m_csTx);

		if (!dwCount)
			continue;

		DWORD dwToSend = dwCount;

		while (dwToSend && m_bOpened)
		{
			if (m_dwTxRd + dwCount > UART_TX_BUFFER_SIZE)
				dwToSend = UART_TX_BUFFER_SIZE - m_dwTxRd;
			else
				dwToSend = dwCount;

			DWORD dwErr;
			DWORD dwWritten = 0;

			while (dwWritten < dwToSend)
			{
				if (!WriteFile(m_hCom, m_ucTxBuffer + m_dwTxRd + dwWritten, dwToSend - dwWritten, &dwErr, &m_wro))
				{
					if (GetLastError() == ERROR_IO_PENDING)
					{
						if (WaitForSingleObject(m_wro.hEvent, INFINITE) == WAIT_OBJECT_0)
						{
							GetOverlappedResult(m_hCom, &m_wro, &dwWritten, FALSE);
						}
					}
				}
			}

			m_dwTxRd += dwToSend;
			if (m_dwTxRd == UART_TX_BUFFER_SIZE)
				m_dwTxRd = 0;

			dwCount -= dwToSend;

			FlushFileBuffers(m_hCom);
		}

		EnterCriticalSection(&m_csTx);
		m_dwTxCount -= dWTotal;
		LeaveCriticalSection(&m_csTx);
	}

	return 0;
}

void CUart::Close() {

	if (m_bOpened) {
		m_bOpened = FALSE;

		if (m_hCom != INVALID_HANDLE_VALUE)
			CloseHandle(m_hCom);

		SetEvent(m_hTxEvent);
		WaitForSingleObject(m_hTxThread, INFINITE);
		WaitForSingleObject(m_hRxThread, INFINITE);
		
		m_hCom = NULL;
	}
}

DWORD CUart::Write(BYTE* pBuf, DWORD dwBufLen)
{
	if (!pBuf || !dwBufLen)
		return 0;

	DWORD dwWritten = 0;

	EnterCriticalSection(&m_csTx);
	DWORD dwCount = UART_TX_BUFFER_SIZE - m_dwTxCount;
	LeaveCriticalSection(&m_csTx);

	if (dwBufLen > dwCount)
		dwBufLen = dwCount;

	while (dwBufLen)
	{
		DWORD dwToWrite;

		if (dwBufLen + m_dwTxWr > UART_TX_BUFFER_SIZE)
			dwToWrite = UART_TX_BUFFER_SIZE - m_dwTxWr;
		else
			dwToWrite = dwBufLen;

		memcpy(m_ucTxBuffer + m_dwTxWr, pBuf, dwToWrite);

		m_dwTxWr += dwToWrite;
		if (m_dwTxWr == UART_TX_BUFFER_SIZE)
			m_dwTxWr = 0;

		dwWritten += dwToWrite;
		dwBufLen -= dwToWrite;
	}

	EnterCriticalSection(&m_csTx);
	m_dwTxCount += dwWritten;
	LeaveCriticalSection(&m_csTx);

	SetEvent(m_hTxEvent);

	return dwWritten;
}

uint32_t CUart::Available()
{
	EnterCriticalSection(&m_csRx);
	DWORD dwAvail = m_dwRxCount;
	LeaveCriticalSection(&m_csRx);

	return dwAvail;
}

bool CUart::WaitForData(DWORD timeout)
{
	return (WaitForSingleObject(m_hRxEvent, timeout) == WAIT_OBJECT_0);
}

DWORD CUart::Read(BYTE* pBuf, DWORD dwLen, DWORD dwTimeout)
{
	if (!pBuf || !dwLen)
		return 0;

	DWORD dwRead = 0;
	DWORD dwCount;
	DWORD dwRemainingTime = dwTimeout;

	while (dwLen)
	{
		DWORD dwToRead;

		dwCount = Available();
		if (dwCount == 0)
		{
			if (!dwTimeout)
				return 0;
			DWORD dwStart = GetTickCount();
			if (!WaitForData(dwRemainingTime))
				return dwRead;

			DWORD dwPassed = GetTickCount() - dwStart;
			if (dwPassed > dwRemainingTime)
				dwRemainingTime = 0;
			else
				dwRemainingTime -= dwPassed;

			dwCount = Available();
			if (dwCount == 0)
				return dwRead;
		}
		
		if (dwCount < dwLen)
		{
			dwToRead = dwCount;
		}
		else {
			dwToRead = dwLen;
		}

		if (dwToRead + m_dwRxRd > UART_RX_BUFFER_SIZE)
			dwToRead = UART_RX_BUFFER_SIZE - m_dwRxRd;
		else
			dwToRead = dwLen;

		memcpy(pBuf + dwRead, m_ucRxBuffer + m_dwRxRd, dwToRead);
		dwRead += dwToRead;
		dwLen -= dwToRead;

		m_dwRxRd += dwToRead;
		if (m_dwRxRd == UART_RX_BUFFER_SIZE)
			m_dwRxRd = 0;

		EnterCriticalSection(&m_csRx);
		m_dwRxCount -= dwToRead;
		LeaveCriticalSection(&m_csRx);
	}		

	return dwRead;
}

void CUart::Reset()
{
	EnterCriticalSection(&m_csRx);
	m_dwRxCount = 0;
	LeaveCriticalSection(&m_csRx);

	EnterCriticalSection(&m_csTx);
	m_dwTxCount = 0;
	LeaveCriticalSection(&m_csTx);

	PurgeComm(m_hCom, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
}
