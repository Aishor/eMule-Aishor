// LLMApiServer.h - Servidor API REST/JSON para control por LLM
// Parte de eMule-Aishor R1.3 - Integración LLM
#pragma once

#include "JsonResponse.h"
#include "QualityDetector.h"
#include "WebSocket.h"


// Puerto por defecto para la API LLM (diferente del WebServer en 4661)
#define LLM_API_DEFAULT_PORT 4711

// Estructura para datos de thread de API
typedef struct {
  CString sURL;
  CString sMethod; // GET, POST, PUT, DELETE
  CString sBody;   // Body del request (JSON)
  void *pThis;
  CWebSocket *pSocket;
  in_addr inadr;
} ApiThreadData;

// Clase principal del servidor API para LLM
class CLLMApiServer {
  friend class CWebSocket;

public:
  CLLMApiServer();
  ~CLLMApiServer();

  // Control del servidor
  void StartServer();
  void StopServer();
  void RestartServer();
  bool IsRunning() const { return m_bServerRunning; }

  // Configuración
  void SetPort(uint16 nPort) { m_nPort = nPort; }
  uint16 GetPort() const { return m_nPort; }
  void SetApiKey(const CString &sKey) { m_sApiKey = sKey; }

protected:
  // Procesamiento de requests
  static void _ProcessApiRequest(const ApiThreadData &Data);

private:
  // === Endpoints de la API REST ===

  // GET /api/v1/status - Estado general de eMule
  static CString _GetStatus(const ApiThreadData &Data);

  // GET /api/v1/downloads - Lista de descargas
  // GET /api/v1/downloads/active - Solo descargas activas
  static CString _GetDownloads(const ApiThreadData &Data);

  // POST /api/v1/downloads - Añadir nueva descarga
  // Body: { "hash": "...", "ed2k": "...", "category": "..." }
  static CString _AddDownload(const ApiThreadData &Data);

  // PUT /api/v1/downloads/{hash}/pause - Pausar descarga
  static CString _PauseDownload(const ApiThreadData &Data);

  // PUT /api/v1/downloads/{hash}/resume - Reanudar descarga
  static CString _ResumeDownload(const ApiThreadData &Data);

  // DELETE /api/v1/downloads/{hash} - Eliminar descarga
  static CString _DeleteDownload(const ApiThreadData &Data);

  // GET /api/v1/search?q=query&type=video|audio&min_sources=N
  static CString _Search(const ApiThreadData &Data);

  // GET /api/v1/library - Archivos compartidos (biblioteca)
  // GET /api/v1/library?category=Movies&min_quality=720p
  static CString _GetLibrary(const ApiThreadData &Data);

  // GET /api/v1/servers - Lista de servidores
  static CString _GetServers(const ApiThreadData &Data);

  // POST /api/v1/servers/connect - Conectar a servidor
  // Body: { "ip": "...", "port": 4661 }
  static CString _ConnectServer(const ApiThreadData &Data);

  // POST /api/v1/servers/disconnect - Desconectar
  static CString _DisconnectServer(const ApiThreadData &Data);

  // GET /api/v1/stats - Estadísticas detalladas
  static CString _GetStats(const ApiThreadData &Data);

  // GET /api/v1/preferences - Preferencias actuales
  static CString _GetPreferences(const ApiThreadData &Data);

  // PUT /api/v1/preferences - Actualizar preferencias
  // Body: { "max_download": 0, "max_upload": 50, ... }
  static CString _UpdatePreferences(const ApiThreadData &Data);

  // === Helpers ===

  // Autenticación
  static bool _IsAuthorized(const ApiThreadData &Data);
  static CString _GetAuthToken(const ApiThreadData &Data);

  // Parsing de URL y parámetros
  static CString _ParseUrlPath(const CString &sURL);
  static CString _ParseUrlParam(const CString &sURL, LPCTSTR szParam);
  static bool _ParseJsonBody(const CString &sBody, CString &sError);

  // Routing
  static CString _RouteRequest(const ApiThreadData &Data);
  static bool _MatchRoute(const CString &sPath, LPCTSTR szPattern,
                          CStringArray &params);

  // Conversión de datos internos a JSON
  static CString _DownloadFileToJson(const class CPartFile *pFile);
  static CString _SharedFileToJson(const class CKnownFile *pFile);
  static CString _ServerToJson(const class CServer *pServer);
  static CString _SearchResultToJson(const class CSearchFile *pFile);

  // Helpers de calidad
  static CString _QualityInfoToJson(const QualityInfo &info);
  static CString _FilterByQuality(const CString &sFileName,
                                  LPCTSTR szMinQuality);

  // Miembros
  uint16 m_nPort;
  CString m_sApiKey;
  bool m_bServerRunning;
  HANDLE m_hServerThread;
};
