#include "stdafx.h"
#include "LLMApiServer.h"
#include "DownloadQueue.h"
#include "PartFile.h"
#include "emule.h"

CString CLLMApiServer::_ExtractHashFromUrl(const CString &sURL) {
  // Asumimos formato /api/v1/downloads/{HASH}/...
  int nStart = sURL.Find(_T("/downloads/"));
  if (nStart == -1) return _T("");
  
  nStart += 11; // len("/downloads/")
  
  CString sRest = sURL.Mid(nStart);
  int nEnd = sRest.Find(_T("/"));
  if (nEnd == -1) return sRest;
  
  return sRest.Left(nEnd);
}

CPartFile* CLLMApiServer::_FindPartFileByHash(const CString &sHash) {
  if (sHash.GetLength() != 32) return NULL;
  
  // Convertir hex string a byte array
  uchar hash[16];
  for(int i = 0; i < 16; i++) {
       int val;
       if (_stscanf(sHash.Mid(i*2, 2), _T("%x"), &val) != 1) return NULL;
       hash[i] = (uchar)val;
  }
  
  if (theApp.downloadqueue)
      return theApp.downloadqueue->GetFileByID(hash);
      
  return NULL;
}

CString CLLMApiServer::_ParseJsonField(const CString &sBody, const CString &sField) {
  // Parser JSON muy básico y sucio
  // Busca "field": "value" o "field": value
  
  CString sSearch = _T("\"") + sField + _T("\"");
  int nPos = sBody.Find(sSearch);
  if (nPos == -1) return _T("");
  
  nPos += sSearch.GetLength(); 
  
  // Buscar :
  nPos = sBody.Find(_T(":"), nPos);
  if (nPos == -1) return _T("");
  nPos++;
  
  // Saltar espacios
  while (nPos < sBody.GetLength() && (sBody[nPos] == ' ' || sBody[nPos] == '\t' || sBody[nPos] == '\n' || sBody[nPos] == '\r'))
      nPos++;
      
  if (nPos >= sBody.GetLength()) return _T("");
  
  // Si es string
  if (sBody[nPos] == '"') {
      nPos++;
      int nEnd = sBody.Find(_T("\""), nPos);
      if (nEnd == -1) return _T("");
      return sBody.Mid(nPos, nEnd - nPos);
  }
  
  // Si es número/boolean
  int nEnd = nPos;
  while (nEnd < sBody.GetLength() && sBody[nEnd] != ',' && sBody[nEnd] != '}' && sBody[nEnd] != ' ' && sBody[nEnd] != '\n')
      nEnd++;
      
  return sBody.Mid(nPos, nEnd - nPos);
}
