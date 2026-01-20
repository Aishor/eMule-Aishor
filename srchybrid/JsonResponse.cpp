// JsonResponse.cpp - Implementación del helper JSON
// Parte de eMule-Aishor R1.3 - Integración LLM
#include "stdafx.h"
#include "JsonResponse.h"


CJsonResponse::CJsonResponse() : m_nDepth(0), m_bNeedComma(false) {}

CJsonResponse::~CJsonResponse() {}

void CJsonResponse::BeginObject() {
  AddCommaIfNeeded();
  m_sJson += _T("{");
  m_nDepth++;
  m_bNeedComma = false;
  m_aIsArray.Add(false);
}

void CJsonResponse::EndObject() {
  m_sJson += _T("}");
  m_nDepth--;
  if (m_aIsArray.GetSize() > 0)
    m_aIsArray.RemoveAt(m_aIsArray.GetSize() - 1);
  m_bNeedComma = true;
}

void CJsonResponse::BeginArray(LPCTSTR szKey) {
  AddCommaIfNeeded();
  if (szKey != NULL && _tcslen(szKey) > 0) {
    m_sJson += _T("\"");
    m_sJson += szKey;
    m_sJson += _T("\":[");
  } else {
    m_sJson += _T("[");
  }
  m_nDepth++;
  m_bNeedComma = false;
  m_aIsArray.Add(true);
}

void CJsonResponse::EndArray() {
  m_sJson += _T("]");
  m_nDepth--;
  if (m_aIsArray.GetSize() > 0)
    m_aIsArray.RemoveAt(m_aIsArray.GetSize() - 1);
  m_bNeedComma = true;
}

void CJsonResponse::AddString(LPCTSTR szKey, LPCTSTR szValue) {
  AddCommaIfNeeded();
  m_sJson += _T("\"");
  m_sJson += szKey;
  m_sJson += _T("\":\"");
  m_sJson += EscapeJson(szValue);
  m_sJson += _T("\"");
  m_bNeedComma = true;
}

void CJsonResponse::AddString(LPCTSTR szKey, const CString& sValue) {
  AddString(szKey, (LPCTSTR)sValue);
}

void CJsonResponse::AddNumber(LPCTSTR szKey, int nValue) {
  AddCommaIfNeeded();
  CString sTemp;
  sTemp.Format(_T("\"%s\":%d"), szKey, nValue);
  m_sJson += sTemp;
  m_bNeedComma = true;
}

void CJsonResponse::AddNumber(LPCTSTR szKey, uint32 nValue) {
  AddCommaIfNeeded();
  CString sTemp;
  sTemp.Format(_T("\"%s\":%u"), szKey, nValue);
  m_sJson += sTemp;
  m_bNeedComma = true;
}

void CJsonResponse::AddNumber(LPCTSTR szKey, uint64 qwValue) {
  AddCommaIfNeeded();
  CString sTemp;
  sTemp.Format(_T("\"%s\":%I64u"), szKey, qwValue);
  m_sJson += sTemp;
  m_bNeedComma = true;
}

void CJsonResponse::AddNumber(LPCTSTR szKey, double dValue) {
  AddCommaIfNeeded();
  CString sTemp;
  sTemp.Format(_T("\"%s\":%.2f"), szKey, dValue);
  m_sJson += sTemp;
  m_bNeedComma = true;
}

void CJsonResponse::AddBool(LPCTSTR szKey, bool bValue) {
  AddCommaIfNeeded();
  CString sTemp;
  sTemp.Format(_T("\"%s\":%s"), szKey, bValue ? _T("true") : _T("false"));
  m_sJson += sTemp;
  m_bNeedComma = true;
}

void CJsonResponse::AddNull(LPCTSTR szKey) {
  AddCommaIfNeeded();
  CString sTemp;
  sTemp.Format(_T("\"%s\":null"), szKey);
  m_sJson += sTemp;
  m_bNeedComma = true;
}

void CJsonResponse::Clear() {
  m_sJson.Empty();
  m_nDepth = 0;
  m_bNeedComma = false;
  m_aIsArray.RemoveAll();
}

void CJsonResponse::AddCommaIfNeeded() {
  if (m_bNeedComma) {
    m_sJson += _T(",");
    m_bNeedComma = false;
  }
}

CString CJsonResponse::EscapeJson(LPCTSTR szText) {
  if (szText == NULL)
    return _T("");

  CString sResult;
  for (int i = 0; szText[i] != _T('\0'); i++) {
    TCHAR c = szText[i];
    switch (c) {
    case _T('\"'):
      sResult += _T("\\\"");
      break;
    case _T('\\'):
      sResult += _T("\\\\");
      break;
    case _T('\b'):
      sResult += _T("\\b");
      break;
    case _T('\f'):
      sResult += _T("\\f");
      break;
    case _T('\n'):
      sResult += _T("\\n");
      break;
    case _T('\r'):
      sResult += _T("\\r");
      break;
    case _T('\t'):
      sResult += _T("\\t");
      break;
    default:
      if (c < 32) {
        CString sTemp;
        sTemp.Format(_T("\\u%04x"), (int)c);
        sResult += sTemp;
      } else {
        sResult += c;
      }
      break;
    }
  }
  return sResult;
}

// Helpers estáticos
CString CJsonResponse::Success(LPCTSTR szMessage) {
  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));
  if (szMessage != NULL && _tcslen(szMessage) > 0)
    json.AddString(_T("message"), szMessage);
  json.EndObject();
  return json.GetJson();
}

CString CJsonResponse::Error(LPCTSTR szMessage, int nCode) {
  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("error"));
  json.AddNumber(_T("code"), nCode);
  json.AddString(_T("message"), szMessage);
  json.EndObject();
  return json.GetJson();
}

CString CJsonResponse::NotFound(LPCTSTR szResource) {
  CString sMsg;
  sMsg.Format(_T("Resource not found: %s"), szResource);
  return Error(sMsg, 404);
}

CString CJsonResponse::Unauthorized() {
  return Error(_T("Unauthorized. Please provide valid API key."), 401);
}
