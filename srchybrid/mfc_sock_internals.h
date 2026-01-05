#pragma once
#include <afxwin.h>

// Internals of MFC for socket state management
// Definitions extracted from sockimpl.h and sockcore.cpp (VS 2026 / v145)

class _AFX_SOCK_STATE : public CNoTrackObject
{
public:
	DWORD m_dwReserved1;
	HINSTANCE m_hInstSOCK;
	void (AFXAPI *m_pfnSockTerm)(void);
	virtual ~_AFX_SOCK_STATE() {
		if (m_pfnSockTerm != NULL)
			m_pfnSockTerm();
	}
};

#define _afxSockThreadState AfxGetModuleThreadState()
#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE

// Note: _afxSockState is an EXTERN_PROCESS_LOCAL
#ifndef _AFXDLL
extern PROCESS_LOCAL(_AFX_SOCK_STATE, _afxSockState)
#else
extern AFX_DATA PROCESS_LOCAL(_AFX_SOCK_STATE, _afxSockState)
#endif
