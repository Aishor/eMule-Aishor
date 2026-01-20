// JsonResponse.h - Helper para generar respuestas JSON
// Parte de eMule-Aishor R1.3 - Integración LLM
#pragma once

#include <AfxTempl.h>

// Helper class para construir respuestas JSON de forma segura
class CJsonResponse {
public:
  CJsonResponse();
  ~CJsonResponse();

  // Construcción de objetos JSON
  void BeginObject();
  void EndObject();
  void BeginArray(LPCTSTR szKey);
  void EndArray();

  // Añadir campos
  void AddString(LPCTSTR szKey, LPCTSTR szValue);
  void AddString(LPCTSTR szKey, const CString &sValue);
  void AddNumber(LPCTSTR szKey, int nValue);
  void AddNumber(LPCTSTR szKey, uint32 nValue);
  void AddNumber(LPCTSTR szKey, uint64 qwValue);
  void AddNumber(LPCTSTR szKey, double dValue);
  void AddBool(LPCTSTR szKey, bool bValue);
  void AddNull(LPCTSTR szKey);

  // Obtener resultado
  CString GetJson() const { return m_sJson; }
  void Clear();

  // Helpers estáticos para respuestas comunes
  static CString Success(LPCTSTR szMessage = NULL);
  static CString Error(LPCTSTR szMessage, int nCode = 400);
  static CString NotFound(LPCTSTR szResource);
  static CString Unauthorized();

private:
  CString m_sJson;
  int m_nDepth;
  bool m_bNeedComma;
  CArray<bool> m_aIsArray;

  void AddCommaIfNeeded();
  CString EscapeJson(LPCTSTR szText);
};
