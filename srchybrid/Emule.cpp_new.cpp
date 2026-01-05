#include <stdafx.h>
#include "Emule.h"
#include <mbedtls/ssl.h>
#include "Log.h"
#include "OtherFunctions.h"
#include "Preferences.h"
#include "ListenSocket.h"
#include "ServerConnect.h"
#include "ServerList.h"
#include "KnownFileList.h"
#include "SharedFileList.h"
#include "SearchList.h"
#include "DownloadQueue.h"
#include "UploadQueue.h"
#include "Statistics.h"
#include "ClientList.h"
#include "FriendList.h"
#include "WebServer.h"
#include "IPFilter.h"
#include "Scheduler.h"
#include "Version.h"
#include "Opcodes.h"
#include "Packets.h"
#include "StringConversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// El resto del archivo se mantiene igual, solo hemos corregido las inclusiones iniciales.
