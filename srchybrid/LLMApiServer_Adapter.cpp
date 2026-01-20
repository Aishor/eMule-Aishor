
#include "stdafx.h"
#include "LLMApiServer.h"
#include "WebServer.h"

// Adaptador: Procesa requests desde el servidor web principal
bool CLLMApiServer::ProcessRequest(const ThreadData &Data) {
    ApiThreadData apiData;
    apiData.sURL = Data.sURL;
    apiData.pThis = Data.pThis;
    apiData.pSocket = Data.pSocket;
    apiData.inadr = Data.inadr;
    
    // Solo procesamos si realmente empieza por /api/v1/
    if (apiData.sURL.Find(_T("/api/v1")) != 0)
        return false;

    _ProcessApiRequest(apiData);
    return true;
}
