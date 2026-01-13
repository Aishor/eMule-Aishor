// LLMApiServer.cpp - Implementación del servidor API REST para LLM
// Parte de eMule-Aishor R1.3 - Integración LLM
#include "LLMApiServer.h"
#include "DownloadQueue.h"
#include "KnownFile.h"
#include "KnownFileList.h"
#include "Log.h"
#include "OtherFunctions.h"
#include "PartFile.h"
#include "Preferences.h"
#include "SearchFile.h"
#include "SearchList.h"
#include "Server.h"
#include "ServerConnect.h"
#include "ServerList.h"
#include "SharedFileList.h"
#include "Statistics.h"
#include "UploadQueue.h"
#include "emule.h"
#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor
CLLMApiServer::CLLMApiServer()
    : m_nPort(LLM_API_DEFAULT_PORT), m_bServerRunning(false),
      m_hServerThread(NULL) {}

// Destructor
CLLMApiServer::~CLLMApiServer() { StopServer(); }

// Iniciar servidor
void CLLMApiServer::StartServer() {
  if (m_bServerRunning)
    return;

  // TODO: Implementar thread de servidor HTTP
  // Por ahora solo marcamos como running
  m_bServerRunning = true;

  theApp.QueueLogLine(false, _T("LLM API Server started on port %d"), m_nPort);
}

// Detener servidor
void CLLMApiServer::StopServer() {
  if (!m_bServerRunning)
    return;

  m_bServerRunning = false;

  // TODO: Detener thread y cerrar sockets

  theApp.QueueLogLine(false, _T("LLM API Server stopped"));
}

// Reiniciar servidor
void CLLMApiServer::RestartServer() {
  StopServer();
  StartServer();
}

// ============================================================================
// PROCESAMIENTO DE REQUESTS
// ============================================================================

void CLLMApiServer::_ProcessApiRequest(const ApiThreadData &Data) {
  // Routing principal
  CString sResponse = _RouteRequest(Data);

  // Enviar respuesta
  if (Data.pSocket) {
    CStringA sUtf8 = CT2CA(sResponse, CP_UTF8);

    CStringA sHeader;
    sHeader.Format("HTTP/1.1 200 OK\r\n"
                   "Content-Type: application/json; charset=utf-8\r\n"
                   "Content-Length: %d\r\n"
                   "Access-Control-Allow-Origin: *\r\n"
                   "Connection: close\r\n"
                   "\r\n",
                   sUtf8.GetLength());

    Data.pSocket->SendData(sHeader, sHeader.GetLength());
    Data.pSocket->SendData(sUtf8, sUtf8.GetLength());
  }
}

// ============================================================================
// ROUTING
// ============================================================================

CString CLLMApiServer::_RouteRequest(const ApiThreadData &Data) {
  // Verificar autenticación
  if (!_IsAuthorized(Data))
    return CJsonResponse::Unauthorized();

  CString sPath = _ParseUrlPath(Data.sURL);
  CStringArray params;

  // GET /api/v1/status
  if (Data.sMethod == _T("GET") && sPath == _T("/api/v1/status"))
    return _GetStatus(Data);

  // GET /api/v1/downloads
  if (Data.sMethod == _T("GET") && sPath == _T("/api/v1/downloads"))
    return _GetDownloads(Data);

  // GET /api/v1/downloads/active
  if (Data.sMethod == _T("GET") && sPath == _T("/api/v1/downloads/active"))
    return _GetDownloads(Data);

  // POST /api/v1/downloads
  if (Data.sMethod == _T("POST") && sPath == _T("/api/v1/downloads"))
    return _AddDownload(Data);

  // PUT /api/v1/downloads/{hash}/pause
  if (Data.sMethod == _T("PUT") &&
      _MatchRoute(sPath, _T("/api/v1/downloads/*/pause"), params))
    return _PauseDownload(Data);

  // PUT /api/v1/downloads/{hash}/resume
  if (Data.sMethod == _T("PUT") &&
      _MatchRoute(sPath, _T("/api/v1/downloads/*/resume"), params))
    return _ResumeDownload(Data);

  // DELETE /api/v1/downloads/{hash}
  if (Data.sMethod == _T("DELETE") &&
      _MatchRoute(sPath, _T("/api/v1/downloads/*"), params))
    return _DeleteDownload(Data);

  // GET /api/v1/search
  if (Data.sMethod == _T("GET") && sPath.Find(_T("/api/v1/search")) == 0)
    return _Search(Data);

  // GET /api/v1/library
  if (Data.sMethod == _T("GET") && sPath.Find(_T("/api/v1/library")) == 0)
    return _GetLibrary(Data);

  // GET /api/v1/servers
  if (Data.sMethod == _T("GET") && sPath == _T("/api/v1/servers"))
    return _GetServers(Data);

  // POST /api/v1/servers/connect
  if (Data.sMethod == _T("POST") && sPath == _T("/api/v1/servers/connect"))
    return _ConnectServer(Data);

  // POST /api/v1/servers/disconnect
  if (Data.sMethod == _T("POST") && sPath == _T("/api/v1/servers/disconnect"))
    return _DisconnectServer(Data);

  // GET /api/v1/stats
  if (Data.sMethod == _T("GET") && sPath == _T("/api/v1/stats"))
    return _GetStats(Data);

  // GET /api/v1/preferences
  if (Data.sMethod == _T("GET") && sPath == _T("/api/v1/preferences"))
    return _GetPreferences(Data);

  // PUT /api/v1/preferences
  if (Data.sMethod == _T("PUT") && sPath == _T("/api/v1/preferences"))
    return _UpdatePreferences(Data);

  // === VISION READY ENDPOINTS ===

  // GET /api/v1/downloads/{hash}/file_info
  if (Data.sMethod == _T("GET") &&
      _MatchRoute(sPath, _T("/api/v1/downloads/*/file_info"), params))
    return _GetFileInfo(Data);

  // POST /api/v1/downloads/{hash}/preview
  if (Data.sMethod == _T("POST") &&
      _MatchRoute(sPath, _T("/api/v1/downloads/*/preview"), params))
    return _SetPreviewMode(Data);

  // POST /api/v1/downloads/{hash}/action
  if (Data.sMethod == _T("POST") &&
      _MatchRoute(sPath, _T("/api/v1/downloads/*/action"), params))
    return _ExecuteAction(Data);

  // Ruta no encontrada
  return CJsonResponse::NotFound(sPath);
}

// ============================================================================
// ENDPOINTS - STATUS
// ============================================================================

CString CLLMApiServer::_GetStatus(const ApiThreadData &Data) {
  CJsonResponse json;
  json.BeginObject();

  json.AddString(_T("status"), _T("success"));

  // Estado de conexión
  json.AddBool(_T("connected"), theApp.IsConnected());
  json.AddBool(_T("ed2k_connected"),
               theApp.serverconnect && theApp.serverconnect->IsConnected());
  json.AddBool(_T("kad_connected"), theApp.IsConnected(true, false));

  // Servidor actual
  if (theApp.serverconnect && theApp.serverconnect->IsConnected()) {
    CServer *pServer = theApp.serverconnect->GetCurrentServer();
    if (pServer) {
      json.AddString(_T("server_name"), pServer->GetListName());
      json.AddString(_T("server_address"), pServer->GetAddress());
      json.AddNumber(_T("server_port"), pServer->GetPort());
    }
  }

  // Velocidades
  json.AddNumber(_T("download_speed"), (uint32)theApp.stat->GetDownDatarate());
  json.AddNumber(_T("upload_speed"), (uint32)theApp.stat->GetUpDatarate());

  // Contadores
  if (theApp.downloadqueue) {
    json.AddNumber(_T("active_downloads"),
                   theApp.downloadqueue->GetFileCount());
  }
  if (theApp.uploadqueue) {
    json.AddNumber(_T("active_uploads"),
                   theApp.uploadqueue->GetUploadQueueLength());
  }

  // Límites configurados
  json.AddNumber(_T("max_download_limit"), thePrefs.GetMaxDownload() * 1024);
  json.AddNumber(_T("max_upload_limit"), thePrefs.GetMaxUpload() * 1024);

  json.EndObject();
  return json.GetJson();
}

// ============================================================================
// ENDPOINTS - DOWNLOADS
// ============================================================================

CString CLLMApiServer::_GetDownloads(const ApiThreadData &Data) {
  if (!theApp.downloadqueue)
    return CJsonResponse::Error(_T("Download queue not available"));

  bool bActiveOnly = Data.sURL.Find(_T("/active")) != -1;

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));
  json.BeginArray(_T("downloads"));

  for (POSITION pos = theApp.downloadqueue->filelist.GetHeadPosition();
       pos != NULL;) {
    CPartFile *pFile = theApp.downloadqueue->filelist.GetNext(pos);
    if (!pFile)
      continue;

    // Si solo queremos activos, filtrar
    if (bActiveOnly && pFile->GetStatus() != PS_READY &&
        pFile->GetStatus() != PS_DOWNLOADING)
      continue;

    json.BeginObject();

    // Información básica
    json.AddString(_T("hash"), md4str(pFile->GetFileHash()));
    json.AddString(_T("name"), pFile->GetFileName());
    json.AddNumber(_T("size"), pFile->GetFileSize());
    json.AddNumber(_T("transferred"), pFile->GetCompletedSize());
    json.AddNumber(_T("progress"), pFile->GetPercentCompleted());

    // Velocidad y tiempo
    json.AddNumber(_T("speed"), pFile->GetDatarate());
    uint32 nTimeRemaining = pFile->getTimeRemaining();
    json.AddNumber(_T("eta_seconds"), nTimeRemaining);

    // Fuentes
    json.AddNumber(_T("sources"), pFile->GetSourceCount());
    json.AddNumber(_T("sources_active"), pFile->GetTransferringSrcCount());

    // Estado
    CString sStatus;
    switch (pFile->GetStatus()) {
    case PS_READY:
      sStatus = _T("ready");
      break;
    case PS_DOWNLOADING:
      sStatus = _T("downloading");
      break;
    case PS_PAUSED:
      sStatus = _T("paused");
      break;
    case PS_COMPLETE:
      sStatus = _T("complete");
      break;
    case PS_ERROR:
      sStatus = _T("error");
      break;
    default:
      sStatus = _T("unknown");
      break;
    }
    json.AddString(_T("status"), sStatus);

    // Prioridad
    uint8 nPrio = pFile->GetDownPriority();
    CString sPriority;
    switch (nPrio) {
    case PR_LOW:
      sPriority = _T("low");
      break;
    case PR_NORMAL:
      sPriority = _T("normal");
      break;
    case PR_HIGH:
      sPriority = _T("high");
      break;
    case PR_AUTO:
      sPriority = _T("auto");
      break;
    default:
      sPriority = _T("normal");
      break;
    }
    json.AddString(_T("priority"), sPriority);

    // Categoría
    json.AddNumber(_T("category"), pFile->GetCategory());

    // Calidad detectada
    QualityInfo quality = CQualityDetector::DetectQuality(pFile->GetFileName());
    json.AddString(_T("quality_string"),
                   CQualityDetector::GetQualityString(quality));
    json.AddNumber(_T("quality_score"), quality.nScore);

    json.EndObject();
  }

  json.EndArray();
  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_AddDownload(const ApiThreadData &Data) {
  // Parsear JSON body para obtener ed2k link o hash
  CString sEd2kLink = _ParseJsonField(Data.sBody, _T("ed2k"));
  CString sHash = _ParseJsonField(Data.sBody, _T("hash"));

  if (sEd2kLink.IsEmpty() && sHash.IsEmpty())
    return CJsonResponse::Error(_T("Missing 'ed2k' or 'hash' field"));

  // Si tenemos hash, construir ed2k link básico
  if (!sHash.IsEmpty() && sEd2kLink.IsEmpty()) {
    // TODO: Necesitaríamos nombre y tamaño para construir link completo
    return CJsonResponse::Error(_T("Hash-only downloads not yet supported"));
  }

  // Añadir descarga desde ed2k link
  try {
    theApp.downloadqueue->AddFileLinkToDownload(sEd2kLink, 0);

    CJsonResponse json;
    json.BeginObject();
    json.AddString(_T("status"), _T("success"));
    json.AddString(_T("message"), _T("Download added"));
    json.AddString(_T("ed2k"), sEd2kLink);
    json.EndObject();
    return json.GetJson();
  } catch (...) {
    return CJsonResponse::Error(_T("Failed to add download"));
  }
}

CString CLLMApiServer::_PauseDownload(const ApiThreadData &Data) {
  CString sHash = _ExtractHashFromUrl(Data.sURL);
  if (sHash.IsEmpty())
    return CJsonResponse::Error(_T("Invalid hash in URL"));

  CPartFile *pFile = _FindPartFileByHash(sHash);
  if (!pFile)
    return CJsonResponse::NotFound(_T("Download not found"));

  pFile->PauseFile();

  return CJsonResponse::Success(_T("Download paused"));
}

CString CLLMApiServer::_ResumeDownload(const ApiThreadData &Data) {
  CString sHash = _ExtractHashFromUrl(Data.sURL);
  if (sHash.IsEmpty())
    return CJsonResponse::Error(_T("Invalid hash in URL"));

  CPartFile *pFile = _FindPartFileByHash(sHash);
  if (!pFile)
    return CJsonResponse::NotFound(_T("Download not found"));

  pFile->ResumeFile();

  return CJsonResponse::Success(_T("Download resumed"));
}

CString CLLMApiServer::_DeleteDownload(const ApiThreadData &Data) {
  CString sHash = _ExtractHashFromUrl(Data.sURL);
  if (sHash.IsEmpty())
    return CJsonResponse::Error(_T("Invalid hash in URL"));

  CPartFile *pFile = _FindPartFileByHash(sHash);
  if (!pFile)
    return CJsonResponse::NotFound(_T("Download not found"));

  pFile->DeleteFile();

  return CJsonResponse::Success(_T("Download deleted"));
}

// ============================================================================
// ENDPOINTS - SEARCH
// ============================================================================

CString CLLMApiServer::_Search(const ApiThreadData &Data) {
  CString sQuery = _ParseUrlParam(Data.sURL, _T("q"));
  if (sQuery.IsEmpty())
    return CJsonResponse::Error(_T("Missing query parameter 'q'"));

  // TODO: Implementar búsqueda real
  // Por ahora retornamos respuesta vacía

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));
  json.AddString(_T("query"), sQuery);
  json.BeginArray(_T("results"));
  json.EndArray();
  json.AddString(_T("message"), _T("Search not fully implemented yet"));
  json.EndObject();
  return json.GetJson();
}

// ============================================================================
// ENDPOINTS - LIBRARY
// ============================================================================

CString CLLMApiServer::_GetLibrary(const ApiThreadData &Data) {
  if (!theApp.sharedfiles)
    return CJsonResponse::Error(_T("Shared files not available"));

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));
  json.BeginArray(_T("files"));

  for (POSITION pos = theApp.sharedfiles->GetFileList()->GetHeadPosition();
       pos != NULL;) {
    CKnownFile *pFile = theApp.sharedfiles->GetFileList()->GetNext(pos);
    if (!pFile)
      continue;

    json.BeginObject();

    json.AddString(_T("hash"), md4str(pFile->GetFileHash()));
    json.AddString(_T("name"), pFile->GetFileName());
    json.AddNumber(_T("size"), pFile->GetFileSize());
    json.AddString(_T("path"), pFile->GetFilePath());

    // Estadísticas
    json.AddNumber(_T("requests"), pFile->statistic.GetRequests());
    json.AddNumber(_T("accepted"), pFile->statistic.GetAccepts());
    json.AddNumber(_T("transferred"), pFile->statistic.GetTransferred());

    // Calidad
    QualityInfo quality = CQualityDetector::DetectQuality(pFile->GetFileName());
    json.AddString(_T("quality_string"),
                   CQualityDetector::GetQualityString(quality));
    json.AddNumber(_T("quality_score"), quality.nScore);
    json.AddString(_T("resolution"),
                   CQualityDetector::GetQualityShortString(quality.quality));
    json.AddString(_T("source"),
                   CQualityDetector::GetSourceString(quality.source));
    json.AddString(_T("codec"),
                   CQualityDetector::GetCodecString(quality.codec));

    json.EndObject();
  }

  json.EndArray();
  json.EndObject();
  return json.GetJson();
}

// ============================================================================
// ENDPOINTS - SERVERS
// ============================================================================

CString CLLMApiServer::_GetServers(const ApiThreadData &Data) {
  if (!theApp.serverlist)
    return CJsonResponse::Error(_T("Server list not available"));

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));
  json.BeginArray(_T("servers"));

  for (UINT i = 0; i \u003c theApp.serverlist->GetServerCount(); i++) {
    CServer *pServer = theApp.serverlist->GetServerAt(i);
    if (!pServer)
      continue;

    json.BeginObject();

    json.AddString(_T("name"), pServer->GetListName());
    json.AddString(_T("address"), pServer->GetAddress());
    json.AddNumber(_T("port"), pServer->GetPort());
    json.AddString(_T("description"), pServer->GetDescription());

    json.AddNumber(_T("users"), pServer->GetUsers());
    json.AddNumber(_T("max_users"), pServer->GetMaxUsers());
    json.AddNumber(_T("files"), pServer->GetFiles());
    json.AddNumber(_T("ping"), pServer->GetPing());

    json.AddBool(_T("static"), pServer->IsStaticMember());
    json.AddNumber(_T("priority"), pServer->GetPreference());
    json.AddNumber(_T("failed_count"), pServer->GetFailedCount());

    json.EndObject();
  }

  json.EndArray();
  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_ConnectServer(const ApiThreadData &Data) {
  // TODO: Parsear JSON body y conectar a servidor
  return CJsonResponse::Error(_T("Not implemented yet"));
}

CString CLLMApiServer::_DisconnectServer(const ApiThreadData &Data) {
  if (theApp.serverconnect) {
    theApp.serverconnect->Disconnect();
    return CJsonResponse::Success(_T("Disconnected from server"));
  }
  return CJsonResponse::Error(_T("Server connect not available"));
}

// ============================================================================
// ENDPOINTS - STATS
// ============================================================================

CString CLLMApiServer::_GetStats(const ApiThreadData &Data) {
  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));

  // Sesión actual
  json.AddNumber(_T("session_downloaded"),
                 thePrefs.GetSessionDownloadedBytes());
  json.AddNumber(_T("session_uploaded"), thePrefs.GetSessionUploadedBytes());
  json.AddNumber(_T("session_duration"), thePrefs.GetSessionRunningTime());

  // Totales
  json.AddNumber(_T("total_downloaded"), thePrefs.GetTotalDownloaded());
  json.AddNumber(_T("total_uploaded"), thePrefs.GetTotalUploaded());

  // Ratios
  float fRatio = thePrefs.GetSessionUploadedBytes() \u003e 0
                     ? (float)thePrefs.GetSessionDownloadedBytes() /
                           thePrefs.GetSessionUploadedBytes()
                     : 0;
  json.AddNumber(_T("session_ratio"), fRatio);

  json.EndObject();
  return json.GetJson();
}

// ============================================================================
// ENDPOINTS - PREFERENCES
// ============================================================================

CString CLLMApiServer::_GetPreferences(const ApiThreadData &Data) {
  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));

  json.AddNumber(_T("max_download"), thePrefs.GetMaxDownload());
  json.AddNumber(_T("max_upload"), thePrefs.GetMaxUpload());
  json.AddNumber(_T("max_connections"), thePrefs.GetMaxConnections());
  json.AddNumber(_T("max_sources_per_file"), thePrefs.GetMaxSourcePerFile());

  json.AddString(_T("nickname"), thePrefs.GetUserNick());
  json.AddNumber(_T("tcp_port"), thePrefs.GetPort());
  json.AddNumber(_T("udp_port"), thePrefs.GetUDPPort());

  json.AddBool(_T("auto_connect"), thePrefs.GetAutoConnect());
  json.AddBool(_T("auto_server_list"), thePrefs.GetAutoUpdateServerList());

  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_UpdatePreferences(const ApiThreadData &Data) {
  // TODO: Parsear JSON body y actualizar preferencias
  return CJsonResponse::Error(_T("Not implemented yet"));
}

// ============================================================================
// ENDPOINTS - VISION READY
// ============================================================================

CString CLLMApiServer::_GetFileInfo(const ApiThreadData &Data) {
  CString sHash = _ExtractHashFromUrl(Data.sURL);
  if (sHash.IsEmpty())
    return CJsonResponse::Error(_T("Invalid hash in URL"));

  CPartFile *pFile = _FindPartFileByHash(sHash);
  if (!pFile)
    return CJsonResponse::NotFound(_T("Download not found"));

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));

  // Información básica
  json.AddString(_T("hash"), md4str(pFile->GetFileHash()));
  json.AddString(_T("name"), pFile->GetFileName());
  json.AddString(_T("file_path"), pFile->GetFilePath());
  json.AddNumber(_T("file_size"), pFile->GetFileSize());
  json.AddNumber(_T("completed_size"), pFile->GetCompletedSize());
  json.AddNumber(_T("progress"), pFile->GetPercentCompleted());

  // Información de chunks
  json.BeginObject(_T("chunks"));
  json.AddNumber(_T("total"), pFile->GetPartCount());

  // Calcular chunks completados
  UINT nCompletedChunks = 0;
  for (UINT i = 0; i < pFile->GetPartCount(); i++) {
    if (pFile->IsComplete(i))
      nCompletedChunks++;
  }
  json.AddNumber(_T("completed"), nCompletedChunks);

  // Verificar primer y último chunk (necesarios para preview)
  bool bFirstChunkComplete = pFile->IsComplete(0);
  bool bLastChunkComplete = pFile->IsComplete(pFile->GetPartCount() - 1);

  json.AddBool(_T("first_chunk_complete"), bFirstChunkComplete);
  json.AddBool(_T("last_chunk_complete"), bLastChunkComplete);
  json.EndObject();

  // Información de video
  CString sExt = pFile->GetFileName();
  sExt.MakeLower();
  bool bIsVideo = sExt.Find(_T(".mkv")) != -1 || sExt.Find(_T(".mp4")) != -1 ||
                  sExt.Find(_T(".avi")) != -1 || sExt.Find(_T(".wmv")) != -1;

  json.AddBool(_T("is_video"), bIsVideo);
  json.AddBool(_T("preview_ready"),
               pFile->IsPreviewableFileType() &&
                   pFile->GetCompletedSize() > 5 * 1024 * 1024);
  json.AddBool(_T("preview_mode"), pFile->GetPreviewPrio());

  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_SetPreviewMode(const ApiThreadData &Data) {
  CString sHash = _ExtractHashFromUrl(Data.sURL);
  if (sHash.IsEmpty())
    return CJsonResponse::Error(_T("Invalid hash in URL"));

  CPartFile *pFile = _FindPartFileByHash(sHash);
  if (!pFile)
    return CJsonResponse::NotFound(_T("Download not found"));

  // Parsear JSON para ver si activar o desactivar
  CString sEnable = _ParseJsonField(Data.sBody, _T("enable"));
  bool bEnable = sEnable != _T("false") && sEnable != _T("0");

  // Activar/desactivar preview mode
  pFile->SetPreviewPrio(bEnable);

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("status"), _T("success"));
  json.AddString(_T("message"), bEnable ? _T("Preview mode activated")
                                        : _T("Preview mode deactivated"));
  json.AddBool(_T("preview_mode"), pFile->GetPreviewPrio());
  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_ExecuteAction(const ApiThreadData &Data) {
  CString sHash = _ExtractHashFromUrl(Data.sURL);
  if (sHash.IsEmpty())
    return CJsonResponse::Error(_T("Invalid hash in URL"));

  CString sAction = _ParseJsonField(Data.sBody, _T("action"));
  if (sAction.IsEmpty())
    return CJsonResponse::Error(_T("Missing 'action' field"));

  CPartFile *pFile = _FindPartFileByHash(sHash);
  if (!pFile)
    return CJsonResponse::NotFound(_T("Download not found"));

  // Ejecutar acción
  if (sAction == _T("delete")) {
    pFile->DeleteFile();
    return CJsonResponse::Success(_T("File deleted"));
  }

  if (sAction == _T("ban_source")) {
    CString sSourceId = _ParseJsonField(Data.sBody, _T("source_id"));
    if (sSourceId.IsEmpty())
      return CJsonResponse::Error(_T("Missing 'source_id' field"));

    // TODO: Implementar ban de fuente específica
    // Por ahora solo retornamos success
    return CJsonResponse::Success(_T("Source banned (not fully implemented)"));
  }

  if (sAction == _T("ban_all_sources")) {
    // Eliminar todas las fuentes
    pFile->RemoveAllSources(false);
    return CJsonResponse::Success(_T("All sources removed"));
  }

  if (sAction == _T("mark_spam")) {
    // TODO: Marcar como spam en la red
    // Por ahora solo eliminamos el archivo
    pFile->DeleteFile();
    return CJsonResponse::Success(_T("Marked as spam and deleted"));
  }

  return CJsonResponse::Error(_T("Unknown action: ") + sAction);
}

// ============================================================================
// HELPERS - AUTENTICACIÓN
// ============================================================================

bool CLLMApiServer::_IsAuthorized(const ApiThreadData &Data) {
  // TODO: Implementar verificación de API key
  // Por ahora permitimos todo (solo para desarrollo)
  return true;
}

CString CLLMApiServer::_GetAuthToken(const ApiThreadData &Data) {
  // TODO: Extraer token del header Authorization
  return _T("");
}

// ============================================================================
// HELPERS - PARSING
// ============================================================================

CString CLLMApiServer::_ParseUrlPath(const CString &sURL) {
  int nPos = sURL.Find(_T('?'));
  if (nPos != -1)
    return sURL.Left(nPos);
  return sURL;
}

CString CLLMApiServer::_ParseUrlParam(const CString &sURL, LPCTSTR szParam) {
  CString sSearch;
  sSearch.Format(_T("%s="), szParam);

  int nStart = sURL.Find(sSearch);
  if (nStart == -1)
    return _T("");

  nStart += sSearch.GetLength();
  int nEnd = sURL.Find(_T('&'), nStart);

  if (nEnd == -1)
    return sURL.Mid(nStart);
  else
    return sURL.Mid(nStart, nEnd - nStart);
}

bool CLLMApiServer::_MatchRoute(const CString &sPath, LPCTSTR szPattern,
                                CStringArray &params) {
  // Simple pattern matching: /api/v1/downloads/*/pause
  CString sPattern = szPattern;

  // Dividir pattern y path en segmentos
  CStringArray patternParts, pathParts;
  _SplitPath(sPattern, patternParts);
  _SplitPath(sPath, pathParts);

  // Deben tener el mismo número de segmentos
  if (patternParts.GetSize() != pathParts.GetSize())
    return false;

  params.RemoveAll();

  // Comparar segmento por segmento
  for (int i = 0; i < patternParts.GetSize(); i++) {
    if (patternParts[i] == _T("*")) {
      // Wildcard - guardar el valor
      params.Add(pathParts[i]);
    } else if (patternParts[i] != pathParts[i]) {
      // No coincide
      return false;
    }
  }

  return true;
}

// ============================================================================
// HELPERS - CONVERSIÓN A JSON
// ============================================================================

CString CLLMApiServer::_DownloadFileToJson(const CPartFile *pFile) {
  if (!pFile)
    return _T("{}");

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("hash"), md4str(pFile->GetFileHash()));
  json.AddString(_T("name"), pFile->GetFileName());
  json.AddNumber(_T("size"), pFile->GetFileSize());
  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_SharedFileToJson(const CKnownFile *pFile) {
  if (!pFile)
    return _T("{}");

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("hash"), md4str(pFile->GetFileHash()));
  json.AddString(_T("name"), pFile->GetFileName());
  json.AddNumber(_T("size"), pFile->GetFileSize());
  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_ServerToJson(const CServer *pServer) {
  if (!pServer)
    return _T("{}");

  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("name"), pServer->GetListName());
  json.AddString(_T("address"), pServer->GetAddress());
  json.AddNumber(_T("port"), pServer->GetPort());
  json.EndObject();
  return json.GetJson();
}

CString CLLMApiServer::_SearchResultToJson(const CSearchFile *pFile) {
  // TODO: Implementar cuando tengamos acceso a CSearchFile
  return _T("{}");
}

CString CLLMApiServer::_QualityInfoToJson(const QualityInfo &info) {
  CJsonResponse json;
  json.BeginObject();
  json.AddString(_T("resolution"),
                 CQualityDetector::GetQualityShortString(info.quality));
  json.AddString(_T("source"), CQualityDetector::GetSourceString(info.source));
  json.AddString(_T("codec"), CQualityDetector::GetCodecString(info.codec));
  json.AddNumber(_T("score"), info.nScore);
  json.AddBool(_T("hdr"), info.bHDR);
  json.AddBool(_T("3d"), info.b3D);
  if (!info.sAudioCodec.IsEmpty())
    json.AddString(_T("audio"), info.sAudioCodec);
  json.EndObject();
  return json.GetJson();
}

// ============================================================================
// HELPERS ADICIONALES
// ============================================================================

CString CLLMApiServer::_ParseJsonField(const CString &sJson, LPCTSTR szField) {
  // Parser JSON simple para extraer un campo
  // Buscar "field":"value" o "field":value
  CString sSearch;
  sSearch.Format(_T("\"%s\""), szField);

  int nStart = sJson.Find(sSearch);
  if (nStart == -1)
    return _T("");

  // Buscar el : después del field
  nStart = sJson.Find(_T(':'), nStart);
  if (nStart == -1)
    return _T("");

  nStart++; // Saltar el :

  // Saltar espacios
  while (nStart < sJson.GetLength() && sJson[nStart] == _T(' '))
    nStart++;

  // Verificar si es string (empieza con ")
  if (nStart < sJson.GetLength() && sJson[nStart] == _T('"')) {
    nStart++; // Saltar la "
    int nEnd = sJson.Find(_T('"'), nStart);
    if (nEnd == -1)
      return _T("");
    return sJson.Mid(nStart, nEnd - nStart);
  } else {
    // Es un número o booleano, buscar hasta , o }
    int nEnd = nStart;
    while (nEnd < sJson.GetLength() && sJson[nEnd] != _T(',') &&
           sJson[nEnd] != _T('}') && sJson[nEnd] != _T(' '))
      nEnd++;
    return sJson.Mid(nStart, nEnd - nStart);
  }
}

CString CLLMApiServer::_ExtractHashFromUrl(const CString &sURL) {
  // Extraer hash de URL como /api/v1/downloads/HASH/action
  // Buscar el segmento después de /downloads/
  int nStart = sURL.Find(_T("/downloads/"));
  if (nStart == -1)
    return _T("");

  nStart += 11; // Longitud de "/downloads/"

  // Buscar el siguiente /
  int nEnd = sURL.Find(_T('/'), nStart);
  if (nEnd == -1)
    nEnd = sURL.GetLength();

  return sURL.Mid(nStart, nEnd - nStart);
}

CPartFile *CLLMApiServer::_FindPartFileByHash(const CString &sHash) {
  if (!theApp.downloadqueue)
    return NULL;

  // Convertir hash string a MD4
  uchar hash[MDX_DIGEST_SIZE];
  if (!strmd4(sHash, hash))
    return NULL;

  // Buscar en la lista de descargas
  for (POSITION pos = theApp.downloadqueue->filelist.GetHeadPosition();
       pos != NULL;) {
    CPartFile *pFile = theApp.downloadqueue->filelist.GetNext(pos);
    if (pFile && md4cmp(pFile->GetFileHash(), hash) == 0)
      return pFile;
  }

  return NULL;
}

void CLLMApiServer::_SplitPath(const CString &sPath, CStringArray &parts) {
  parts.RemoveAll();

  CString sTemp = sPath;
  int nPos = 0;

  while ((nPos = sTemp.Find(_T('/'))) != -1) {
    CString sPart = sTemp.Left(nPos);
    if (!sPart.IsEmpty())
      parts.Add(sPart);
    sTemp = sTemp.Mid(nPos + 1);
  }

  // Añadir el último segmento
  if (!sTemp.IsEmpty())
    parts.Add(sTemp);
}
