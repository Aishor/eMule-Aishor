//
// ZIPFile.h
//
// Copyright (c) Shareaza Development Team, 2002-2004.
// This file is part of SHAREAZA (www.shareaza.com)
//
// Shareaza is free software; you can redistribute it
// and/or modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2 of
// the License, or (at your option) any later version.
//
// Shareaza is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Shareaza; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once

#include "unzip.h"

class CBuffer;

class CZIPFile
{
// Construction
public:
	explicit CZIPFile(HANDLE hAttach = INVALID_HANDLE_VALUE);
	~CZIPFile();

// File Class
public:
	class File
	{
		friend class CZIPFile;

		inline File() = default;
		CZIPFile *m_pZIP;
	public:
		//CBuffer*	Decompress();
		bool	Extract(LPCTSTR pszFile);
		CString	m_sName;
		uint64	m_nSize;
	protected:
		//bool	PrepareToDecompress(LPVOID pStream); // No needed for minizip
		uint64	m_nLocalOffset; // Used as index/pos in minizip maybe? Or just keep for compat if needed, but minizip uses internal state.
		// Actually minizip iterates by current file. To jump to a file we need its name or index.
		// We can store the index here.
		uLong   m_nIndex; 
		uint64	m_nCompressedSize;
		int		m_nCompression;
		unz64_file_pos m_ZipPos;
	};

// Attributes
protected:
	unzFile		m_hZip;
	File		*m_pFiles;
	int			m_nCount;
	bool		m_bAttach;
	HANDLE      m_hAttachedFile; // Keep track if we need to close it manually or if it was attached.

// Operations
public:
	bool	Open(LPCTSTR pszFile);
	bool	Attach(HANDLE hFile);
	bool	IsOpen() const;
	void	Close();
public:
	int		GetCount() const						{ return m_nCount; } //get the file count
	File*	GetFile(int nFile) const;
	File*	GetFile(LPCTSTR pszFile, BOOL bPartial = FALSE) const;
protected:
	// Helpers
	void    BuildDirectory();
};
