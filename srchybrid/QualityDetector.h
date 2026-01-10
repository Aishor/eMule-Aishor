// QualityDetector.h - Detector de calidad de archivos multimedia
// Parte de eMule-Aishor R1.3 - Integración LLM
#pragma once

// Enumeración de calidades de video
enum VideoQuality {
  VQ_UNKNOWN = 0,
  VQ_CAM = 1,      // CAM, TS, TC
  VQ_SCREENER = 2, // SCR, DVDSCR
  VQ_480P = 3,     // 480p, SD
  VQ_720P = 4,     // 720p, HD
  VQ_1080P = 5,    // 1080p, Full HD
  VQ_1440P = 6,    // 1440p, 2K
  VQ_2160P = 7,    // 2160p, 4K, UHD
  VQ_4320P = 8     // 4320p, 8K
};

// Enumeración de fuentes de video
enum VideoSource {
  VS_UNKNOWN = 0,
  VS_CAM = 1,    // Cámara en cine
  VS_WEBRIP = 2, // WEB-Rip
  VS_WEBDL = 3,  // WEB-DL
  VS_HDTV = 4,   // HDTV
  VS_DVDRIP = 5, // DVDRip
  VS_BRRIP = 6,  // BRRip
  VS_BLURAY = 7, // BluRay
  VS_REMUX = 8   // Remux (máxima calidad)
};

// Enumeración de codecs de video
enum VideoCodec {
  VC_UNKNOWN = 0,
  VC_XVID = 1,
  VC_DIVX = 2,
  VC_H264 = 3, // x264, AVC
  VC_H265 = 4, // x265, HEVC
  VC_VP9 = 5,
  VC_AV1 = 6
};

// Estructura con información de calidad detectada
struct QualityInfo {
  VideoQuality quality;
  VideoSource source;
  VideoCodec codec;
  int nYear;           // Año de la película/serie
  bool bHDR;           // HDR10, Dolby Vision, etc.
  bool b3D;            // 3D
  CString sAudioCodec; // DTS, AC3, AAC, etc.
  int nScore;          // Puntuación de calidad (0-100)

  QualityInfo()
      : quality(VQ_UNKNOWN), source(VS_UNKNOWN), codec(VC_UNKNOWN), nYear(0),
        bHDR(false), b3D(false), nScore(0) {}
};

// Clase para detectar calidad de archivos multimedia
class CQualityDetector {
public:
  // Detectar calidad desde nombre de archivo
  static QualityInfo DetectQuality(const CString &sFileName);

  // Comparar dos calidades (retorna true si q1 es mejor que q2)
  static bool IsBetterQuality(const QualityInfo &q1, const QualityInfo &q2);

  // Obtener descripción textual de la calidad
  static CString GetQualityString(const QualityInfo &info);
  static CString GetQualityShortString(VideoQuality quality);
  static CString GetSourceString(VideoSource source);
  static CString GetCodecString(VideoCodec codec);

  // Calcular puntuación de calidad
  static int CalculateScore(const QualityInfo &info);

private:
  // Helpers de detección
  static VideoQuality DetectResolution(const CString &sFileName);
  static VideoSource DetectSource(const CString &sFileName);
  static VideoCodec DetectCodec(const CString &sFileName);
  static int DetectYear(const CString &sFileName);
  static bool DetectHDR(const CString &sFileName);
  static bool Detect3D(const CString &sFileName);
  static CString DetectAudioCodec(const CString &sFileName);

  // Helper para buscar patrones
  static bool ContainsPattern(const CString &sText, LPCTSTR szPattern);
  static bool ContainsAnyPattern(const CString &sText,
                                 const CStringArray &patterns);
};
