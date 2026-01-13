//
// ZIPFile.cpp
//
// Refactored to use zlib/minizip for 64-bit support.
//

#include "StdAfx.h"
#include "ZIPFile.h"
#include "zlib/zlib.h"
// Define _CRT_SECURE_NO_WARNINGS to avoid errors in minizip headers if any
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "zlib/contrib/minizip/iowin32.h"
#include "zlib/contrib/minizip/unzip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// External definitions from iowin32.c (not in header)
extern "C" {
    // Structure used by iowin32.c - MUST MATCH!
    typedef struct
    {
        HANDLE hf;
        int error;
    } WIN32FILE_IOWIN;

    uLong ZCALLBACK win32_read_file_func(voidpf opaque, voidpf stream, void* buf, uLong size);
    uLong ZCALLBACK win32_write_file_func(voidpf opaque, voidpf stream, const void* buf, uLong size);
    ZPOS64_T ZCALLBACK win32_tell64_file_func(voidpf opaque, voidpf stream);
    long ZCALLBACK win32_seek64_file_func(voidpf opaque, voidpf stream, ZPOS64_T offset, int origin);
    int ZCALLBACK win32_close_file_func(voidpf opaque, voidpf stream);
    int ZCALLBACK win32_error_file_func(voidpf opaque, voidpf stream);
}

// Custom Open/Close for Attached handles
static voidpf ZCALLBACK win32_open64_file_func_attached(voidpf opaque, const void* filename, int mode)
{
    // Opaque holds the handle
    HANDLE hFile = (HANDLE)opaque;
    if (hFile == INVALID_HANDLE_VALUE) return NULL;

    WIN32FILE_IOWIN* ret = (WIN32FILE_IOWIN*)malloc(sizeof(WIN32FILE_IOWIN));
    if (ret) {
        ret->hf = hFile;
        ret->error = 0;
    }
    return ret;
}

static int ZCALLBACK win32_close_file_func_attached(voidpf opaque, voidpf stream)
{
    // Only free the structure, DO NOT close the handle
    if (stream) {
        free(stream);
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile construction

CZIPFile::CZIPFile(HANDLE hAttach)
	: m_hZip(NULL)
	, m_pFiles(NULL)
	, m_nCount(0)
	, m_bAttach(false)
    , m_hAttachedFile(INVALID_HANDLE_VALUE)
{
	if (hAttach != INVALID_HANDLE_VALUE)
		Attach(hAttach);
}

CZIPFile::~CZIPFile()
{
	Close();
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile open

bool CZIPFile::Open(LPCTSTR pszFile)
{
	ASSERT(pszFile != NULL);
	Close();

#ifdef _UNICODE
    zlib_filefunc64_def ffunc;
    fill_win32_filefunc64W(&ffunc);
    m_hZip = unzOpen2_64(pszFile, &ffunc);
#else
    m_hZip = unzOpen64(pszFile);
#endif

    if (m_hZip) {
        BuildDirectory();
        return true;
    }
	return false;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile attach

bool CZIPFile::Attach(HANDLE hFile)
{
	ASSERT(hFile != INVALID_HANDLE_VALUE);
	Close();

	m_bAttach = true;
    m_hAttachedFile = hFile;

    zlib_filefunc64_def ffunc;
    // Fill basic functions first (read, write, seek, tell, error) from iowin32
    ffunc.zread_file = win32_read_file_func;
    ffunc.zwrite_file = win32_write_file_func; // Not used for unzip but good practice
    ffunc.ztell64_file = win32_tell64_file_func;
    ffunc.zseek64_file = win32_seek64_file_func;
    ffunc.zerror_file = win32_error_file_func;
    
    // Override Open/Close for attachment logic
    ffunc.zopen64_file = win32_open64_file_func_attached;
    ffunc.zclose_file = win32_close_file_func_attached;
    
    // Pass handle as opaque
    ffunc.opaque = hFile;

    m_hZip = unzOpen2_64(NULL, &ffunc);

	if (m_hZip) {
        BuildDirectory();
		return true;
    }

    // If failed, we don't close hFile (caller responsibility if attach failed?)
    // But we set m_bAttach=true, so Close() won't close it either.
    // Ideally if Attach fails, we should revert state.
    m_bAttach = false;
    m_hAttachedFile = INVALID_HANDLE_VALUE;
	return false;
}

bool CZIPFile::IsOpen() const
{
	return m_hZip != NULL;
}

void CZIPFile::Close()
{
	if (m_hZip) {
        unzClose(m_hZip);
        m_hZip = NULL;
	}

    if (m_pFiles) {
	    delete[] m_pFiles;
	    m_pFiles = NULL;
    }
	m_nCount = 0;

    m_bAttach = false;
    m_hAttachedFile = INVALID_HANDLE_VALUE;
}

void CZIPFile::BuildDirectory()
{
    if (!m_hZip) return;

    unz_global_info64 gi;
    if (unzGetGlobalInfo64(m_hZip, &gi) != UNZ_OK) return;

    m_nCount = (int)gi.number_entry;
    if (m_nCount <= 0) return;

    m_pFiles = new File[m_nCount];

    if (unzGoToFirstFile(m_hZip) != UNZ_OK) {
        m_nCount = 0;
        delete[] m_pFiles;
        m_pFiles = NULL;
        return;
    }

    for (int i = 0; i < m_nCount; i++) {
        char filename[1024];
        unz_file_info64 file_info;
        
        if (unzGetCurrentFileInfo64(m_hZip, &file_info, filename, sizeof(filename), NULL, 0, NULL, 0) != UNZ_OK)
            break;

        File* pFile = &m_pFiles[i];
        pFile->m_pZIP = this;
        
        // Convert to CString (handles MultiByte/Unicode conversion)
        pFile->m_sName = filename;
        pFile->m_sName.Replace('\\', '/');

        pFile->m_nSize = file_info.uncompressed_size;
        pFile->m_nCompressedSize = file_info.compressed_size;
        pFile->m_nCompression = file_info.compression_method;
        pFile->m_nIndex = i;
        
        // Cache position
        if (unzGetFilePos64(m_hZip, &pFile->m_ZipPos) != UNZ_OK) {
            // Should not happen
        }

        if (unzGoToNextFile(m_hZip) != UNZ_OK)
            break;
    }
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile get a particular file

CZIPFile::File* CZIPFile::GetFile(int nFile) const
{
	return (nFile < 0 || nFile >= m_nCount) ? NULL : m_pFiles + nFile;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile lookup a file by name

CZIPFile::File* CZIPFile::GetFile(LPCTSTR pszFile, BOOL bPartial) const
{
	File *pFile = m_pFiles;
	for (int nFile = m_nCount; --nFile >= 0;) {
		LPCTSTR pszName = bPartial ? _tcsrchr(pFile->m_sName, '/') : NULL;
		pszName = pszName ? pszName + 1 : (LPCTSTR)pFile->m_sName;
		if (_tcsicoll(pszName, pszFile) == 0)
			return pFile;
		++pFile;
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CZIPFile::File extract

bool CZIPFile::File::Extract(LPCTSTR pszFile)
{
    if (!m_pZIP || !m_pZIP->m_hZip) return false;

    // Restore position
    if (unzGoToFilePos64(m_pZIP->m_hZip, &m_ZipPos) != UNZ_OK) return false;

    if (unzOpenCurrentFile(m_pZIP->m_hZip) != UNZ_OK) return false;

    // Create output file
    HANDLE hOut = ::CreateFile(pszFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hOut == INVALID_HANDLE_VALUE) {
        unzCloseCurrentFile(m_pZIP->m_hZip);
        return false;
    }

    const DWORD BUFFER_SIZE = 65536; // 64K buffer
    BYTE* pBuffer = new BYTE[BUFFER_SIZE];
    int nRead = 0;
    bool bOk = true;

    do {
        nRead = unzReadCurrentFile(m_pZIP->m_hZip, pBuffer, BUFFER_SIZE);
        if (nRead < 0) {
            bOk = false;
            break;
        }
        if (nRead > 0) {
            DWORD nWritten;
            if (!::WriteFile(hOut, pBuffer, (DWORD)nRead, &nWritten, NULL) || nWritten != (DWORD)nRead) {
                bOk = false;
                break;
            }
        }
    } while (nRead > 0);

    delete[] pBuffer;
    ::CloseHandle(hOut);
    unzCloseCurrentFile(m_pZIP->m_hZip);

    if (!bOk) {
        ::DeleteFile(pszFile);
        return false;
    }

    return true;
}