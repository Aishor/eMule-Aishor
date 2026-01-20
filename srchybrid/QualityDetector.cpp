// QualityDetector.cpp - Implementación del detector de calidad
// Parte de eMule-Aishor R1.3 - Integración LLM
#include "stdafx.h"
#include "QualityDetector.h"

// Eliminamos include de regex si no es soportado o necesario por ahora, 
// o lo usamos con precaución. MFC CString no siempre amigable.
// #include <regex> 

// Detectar calidad completa desde nombre de archivo
QualityInfo CQualityDetector::DetectQuality(const CString& sFileName) {
  QualityInfo info;

  CString sUpper = sFileName;
  sUpper.MakeUpper();

  info.quality = DetectResolution(sUpper);
  info.source = DetectSource(sUpper);
  info.codec = DetectCodec(sUpper);
  info.nYear = DetectYear(sUpper);
  info.bHDR = DetectHDR(sUpper);
  info.b3D = Detect3D(sUpper);
  info.sAudioCodec = DetectAudioCodec(sUpper);
  info.nScore = CalculateScore(info);

  return info;
}

// Detectar resolución
VideoQuality CQualityDetector::DetectResolution(const CString& sFileName) {
  if (ContainsPattern(sFileName, _T("2160P")) ||
      ContainsPattern(sFileName, _T("4K")) ||
      ContainsPattern(sFileName, _T("UHD")))
    return VQ_2160P;

  if (ContainsPattern(sFileName, _T("4320P")) ||
      ContainsPattern(sFileName, _T("8K")))
    return VQ_4320P;

  if (ContainsPattern(sFileName, _T("1440P")) ||
      ContainsPattern(sFileName, _T("2K")))
    return VQ_1440P;

  if (ContainsPattern(sFileName, _T("1080P")) ||
      ContainsPattern(sFileName, _T("FULLHD")) ||
      ContainsPattern(sFileName, _T("FHD")))
    return VQ_1080P;

  if (ContainsPattern(sFileName, _T("720P")) ||
      ContainsPattern(sFileName, _T("HD")))
    return VQ_720P;

  if (ContainsPattern(sFileName, _T("480P")) ||
      ContainsPattern(sFileName, _T("SD")))
    return VQ_480P;

  if (ContainsPattern(sFileName, _T("SCR")) ||
      ContainsPattern(sFileName, _T("SCREENER")) ||
      ContainsPattern(sFileName, _T("DVDSCR")))
    return VQ_SCREENER;

  if (ContainsPattern(sFileName, _T("CAM")) ||
      ContainsPattern(sFileName, _T("TS")) ||
      ContainsPattern(sFileName, _T("TC")) ||
      ContainsPattern(sFileName, _T("TELESYNC")))
    return VQ_CAM;

  return VQ_UNKNOWN;
}

// Detectar fuente
VideoSource CQualityDetector::DetectSource(const CString& sFileName) {
  if (ContainsPattern(sFileName, _T("REMUX")))
    return VS_REMUX;

  if (ContainsPattern(sFileName, _T("BLURAY")) ||
      ContainsPattern(sFileName, _T("BLU-RAY")) ||
      ContainsPattern(sFileName, _T("BDRIP")) ||
      ContainsPattern(sFileName, _T("BD-RIP")))
    return VS_BLURAY;

  if (ContainsPattern(sFileName, _T("BRRIP")) ||
      ContainsPattern(sFileName, _T("BR-RIP")))
    return VS_BRRIP;

  if (ContainsPattern(sFileName, _T("DVDRIP")) ||
      ContainsPattern(sFileName, _T("DVD-RIP")))
    return VS_DVDRIP;

  if (ContainsPattern(sFileName, _T("HDTV")))
    return VS_HDTV;

  if (ContainsPattern(sFileName, _T("WEB-DL")) ||
      ContainsPattern(sFileName, _T("WEBDL")))
    return VS_WEBDL;

  if (ContainsPattern(sFileName, _T("WEBRIP")) ||
      ContainsPattern(sFileName, _T("WEB-RIP")))
    return VS_WEBRIP;

  if (ContainsPattern(sFileName, _T("CAM")) ||
      ContainsPattern(sFileName, _T("TS")) ||
      ContainsPattern(sFileName, _T("TC")))
    return VS_CAM;

  return VS_UNKNOWN;
}

// Detectar codec
VideoCodec CQualityDetector::DetectCodec(const CString& sFileName) {
  if (ContainsPattern(sFileName, _T("AV1")))
    return VC_AV1;

  if (ContainsPattern(sFileName, _T("VP9")))
    return VC_VP9;

  if (ContainsPattern(sFileName, _T("H265")) ||
      ContainsPattern(sFileName, _T("X265")) ||
      ContainsPattern(sFileName, _T("HEVC")))
    return VC_H265;

  if (ContainsPattern(sFileName, _T("H264")) ||
      ContainsPattern(sFileName, _T("X264")) ||
      ContainsPattern(sFileName, _T("AVC")))
    return VC_H264;

  if (ContainsPattern(sFileName, _T("DIVX")))
    return VC_DIVX;

  if (ContainsPattern(sFileName, _T("XVID")))
    return VC_XVID;

  return VC_UNKNOWN;
}

// Detectar año (busca patrón 19XX o 20XX)
int CQualityDetector::DetectYear(const CString& sFileName) {
  // Buscar patrón de año: 1900-2099
  for (int i = 0; i < sFileName.GetLength() - 3; i++) {
    if (_istdigit(sFileName[i]) && _istdigit(sFileName[i + 1]) && _istdigit(
            sFileName[i + 2]) && _istdigit(sFileName[i + 3])) {

      CString sYear = sFileName.Mid(i, 4);
      int nYear = _ttoi(sYear);

      if (nYear >= 1900 && nYear <= 2099)
        return nYear;
    }
  }
  return 0;
}

// Detectar HDR
bool CQualityDetector::DetectHDR(const CString& sFileName) {
  return ContainsPattern(sFileName, _T("HDR")) ||
         ContainsPattern(sFileName, _T("HDR10")) ||
         ContainsPattern(sFileName, _T("DOLBY VISION")) ||
         ContainsPattern(sFileName, _T("DV"));
}

// Detectar 3D
bool CQualityDetector::Detect3D(const CString& sFileName) {
  return ContainsPattern(sFileName, _T("3D")) ||
         ContainsPattern(sFileName, _T("HSBS")) ||
         ContainsPattern(sFileName, _T("H-SBS"));
}

// Detectar codec de audio
CString CQualityDetector::DetectAudioCodec(const CString& sFileName) {
  if (ContainsPattern(sFileName, _T("ATMOS")))
    return _T("Atmos");
  if (ContainsPattern(sFileName, _T("DTS-HD")) ||
      ContainsPattern(sFileName, _T("DTSHD")))
    return _T("DTS-HD");
  if (ContainsPattern(sFileName, _T("DTS")))
    return _T("DTS");
  if (ContainsPattern(sFileName, _T("TRUEHD")))
    return _T("TrueHD");
  if (ContainsPattern(sFileName, _T("DD5.1")) ||
      ContainsPattern(sFileName, _T("AC3")))
    return _T("AC3");
  if (ContainsPattern(sFileName, _T("AAC")))
    return _T("AAC");
  if (ContainsPattern(sFileName, _T("MP3")))
    return _T("MP3");

  return _T("Unknown");
}

// Calcular puntuación de calidad (0-100)
int CQualityDetector::CalculateScore(const QualityInfo& info) {
  int nScore = 0;

  // Resolución (0-40 puntos)
  switch (info.quality) {
  case VQ_4320P:
    nScore += 40;
    break;
  case VQ_2160P:
    nScore += 35;
    break;
  case VQ_1440P:
    nScore += 30;
    break;
  case VQ_1080P:
    nScore += 25;
    break;
  case VQ_720P:
    nScore += 18;
    break;
  case VQ_480P:
    nScore += 10;
    break;
  case VQ_SCREENER:
    nScore += 5;
    break;
  case VQ_CAM:
    nScore += 1;
    break;
  default:
    break;
  }

  // Fuente (0-30 puntos)
  switch (info.source) {
  case VS_REMUX:
    nScore += 30;
    break;
  case VS_BLURAY:
    nScore += 25;
    break;
  case VS_BRRIP:
    nScore += 20;
    break;
  case VS_WEBDL:
    nScore += 18;
    break;
  case VS_DVDRIP:
    nScore += 15;
    break;
  case VS_HDTV:
    nScore += 12;
    break;
  case VS_WEBRIP:
    nScore += 10;
    break;
  case VS_CAM:
    nScore += 1;
    break;
  default:
    break;
  }

  // Codec (0-15 puntos)
  switch (info.codec) {
  case VC_AV1:
    nScore += 15;
    break;
  case VC_H265:
    nScore += 12;
    break;
  case VC_VP9:
    nScore += 10;
    break;
  case VC_H264:
    nScore += 8;
    break;
  case VC_DIVX:
    nScore += 4;
    break;
  case VC_XVID:
    nScore += 2;
    break;
  default:
    break;
  }

  // Bonificaciones
  if (info.bHDR)
    nScore += 10;
  if (info.sAudioCodec == _T("Atmos"))
    nScore += 5;
  else if (info.sAudioCodec == _T("DTS-HD"))
    nScore += 4;
  else if (info.sAudioCodec == _T("DTS"))
    nScore += 3;

  return min(nScore, 100);
}

// Comparar calidades
bool CQualityDetector::IsBetterQuality(const QualityInfo& q1,
                                       const QualityInfo& q2) {
  return q1.nScore > q2.nScore;
}

// Obtener descripción textual
CString CQualityDetector::GetQualityString(const QualityInfo& info) {
  CString sResult;
  sResult.Format(_T("%s %s %s"), GetQualityShortString(info.quality),
                 GetSourceString(info.source), GetCodecString(info.codec));

  if (info.bHDR)
    sResult += _T(" HDR");
  if (info.b3D)
    sResult += _T(" 3D");
  if (!info.sAudioCodec.IsEmpty() && info.sAudioCodec !=
      _T("Unknown"))
    sResult += _T(" ") + info.sAudioCodec;

  return sResult;
}

CString CQualityDetector::GetQualityShortString(VideoQuality quality) {
  switch (quality) {
  case VQ_4320P:
    return _T("8K");
  case VQ_2160P:
    return _T("4K");
  case VQ_1440P:
    return _T("2K");
  case VQ_1080P:
    return _T("1080p");
  case VQ_720P:
    return _T("720p");
  case VQ_480P:
    return _T("480p");
  case VQ_SCREENER:
    return _T("Screener");
  case VQ_CAM:
    return _T("CAM");
  default:
    return _T("Unknown");
  }
}

CString CQualityDetector::GetSourceString(VideoSource source) {
  switch (source) {
  case VS_REMUX:
    return _T("Remux");
  case VS_BLURAY:
    return _T("BluRay");
  case VS_BRRIP:
    return _T("BRRip");
  case VS_WEBDL:
    return _T("WEB-DL");
  case VS_DVDRIP:
    return _T("DVDRip");
  case VS_HDTV:
    return _T("HDTV");
  case VS_WEBRIP:
    return _T("WEBRip");
  case VS_CAM:
    return _T("CAM");
  default:
    return _T("");
  }
}

CString CQualityDetector::GetCodecString(VideoCodec codec) {
  switch (codec) {
  case VC_AV1:
    return _T("AV1");
  case VC_H265:
    return _T("H.265");
  case VC_VP9:
    return _T("VP9");
  case VC_H264:
    return _T("H.264");
  case VC_DIVX:
    return _T("DivX");
  case VC_XVID:
    return _T("XviD");
  default:
    return _T("");
  }
}

// Helper para buscar patrones
bool CQualityDetector::ContainsPattern(const CString& sText,
                                       LPCTSTR szPattern) {
  return sText.Find(szPattern) != -1;
}

bool CQualityDetector::ContainsAnyPattern(const CString& sText,
                                          const CStringArray& patterns) {
  for (int i = 0; i < patterns.GetSize(); i++) {
    if (ContainsPattern(sText, patterns[i]))
      return true;
  }
  return false;
}
