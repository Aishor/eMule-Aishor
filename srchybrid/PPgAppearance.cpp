//this file is part of eMule
//Copyright (C)2002-2024 Merkur ( strEmail.Format("%s@%s", "devteam", "emule-project.net") / https://www.emule-project.net )
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#include "stdafx.h"
#include "emule.h"
#include "PPgAppearance.h"
#include <dlgs.h>
#include "Preferences.h"
#include "OtherFunctions.h"
#include "emuledlg.h"
#include "HelpIDs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNAMIC(CPPgAppearance, CPropertyPage)

BEGIN_MESSAGE_MAP(CPPgAppearance, CPropertyPage)
	ON_BN_CLICKED(IDC_SELECT_APP_FONT, OnBnClickedSelectAppFont)
	ON_BN_CLICKED(IDC_SELECT_LIST_FONT, OnBnClickedSelectListFont)
	ON_BN_CLICKED(IDC_SELECT_LOG_FONT, OnBnClickedSelectLogFont)
	ON_CBN_SELCHANGE(IDC_ICON_SCALE, OnSettingsChange)
	ON_BN_CLICKED(IDC_DPI_AWARE, OnSettingsChange)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

CPPgAppearance::CPPgAppearance()
	: CPropertyPage(CPPgAppearance::IDD)
{
}

void CPPgAppearance::DoDataExchange(CDataExchange *pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ICON_SCALE, m_ctrlIconScale);
}

void CPPgAppearance::LoadSettings()
{
	// Icon Scale ComboBox
	m_ctrlIconScale.SetCurSel(thePrefs.GetIconScale());

	// DPI Aware checkbox
	CheckDlgButton(IDC_DPI_AWARE, static_cast<UINT>(thePrefs.IsDPIAware()));

	UpdateFontSamples();
}

void CPPgAppearance::UpdateFontSamples()
{
	// Update App Font sample
	m_fontAppSample.DeleteObject();
	LOGFONT lfApp = {};
	HDC hDC = ::GetDC(NULL);
	lfApp.lfHeight = -MulDiv(thePrefs.GetAppFontSize() / 10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	lfApp.lfWeight = FW_NORMAL;
	_tcscpy_s(lfApp.lfFaceName, thePrefs.GetAppFontName());
	m_fontAppSample.CreateFontIndirect(&lfApp);
	
	CString strAppSample;
	strAppSample.Format(_T("%s, %dpt"), (LPCTSTR)thePrefs.GetAppFontName(), thePrefs.GetAppFontSize() / 10);
	SetDlgItemText(IDC_APP_FONT_SAMPLE, strAppSample);
	CWnd *pAppSample = GetDlgItem(IDC_APP_FONT_SAMPLE);
	if (pAppSample)
		pAppSample->SetFont(&m_fontAppSample);

	// Update List Font sample
	m_fontListSample.DeleteObject();
	LOGFONT lfList = {};
	hDC = ::GetDC(NULL);
	lfList.lfHeight = -MulDiv(thePrefs.GetListFontSize() / 10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	lfList.lfWeight = FW_NORMAL;
	_tcscpy_s(lfList.lfFaceName, thePrefs.GetListFontName());
	m_fontListSample.CreateFontIndirect(&lfList);

	CString strListSample;
	strListSample.Format(_T("%s, %dpt"), (LPCTSTR)thePrefs.GetListFontName(), thePrefs.GetListFontSize() / 10);
	SetDlgItemText(IDC_LIST_FONT_SAMPLE, strListSample);
	CWnd *pListSample = GetDlgItem(IDC_LIST_FONT_SAMPLE);
	if (pListSample)
		pListSample->SetFont(&m_fontListSample);

	// Update Log Font sample  
	m_fontLogSample.DeleteObject();
	LOGFONT lfLog = {};
	hDC = ::GetDC(NULL);
	lfLog.lfHeight = -MulDiv(thePrefs.GetLogFontSize() / 10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	lfLog.lfWeight = FW_NORMAL;
	lfLog.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	_tcscpy_s(lfLog.lfFaceName, thePrefs.GetLogFontName());
	m_fontLogSample.CreateFontIndirect(&lfLog);

	CString strLogSample;
	strLogSample.Format(_T("%s, %dpt"), (LPCTSTR)thePrefs.GetLogFontName(), thePrefs.GetLogFontSize() / 10);
	SetDlgItemText(IDC_LOG_FONT_SAMPLE, strLogSample);
	CWnd *pLogSample = GetDlgItem(IDC_LOG_FONT_SAMPLE);
	if (pLogSample)
		pLogSample->SetFont(&m_fontLogSample);
}

BOOL CPPgAppearance::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	InitWindowStyles(this);

	// Populate Icon Scale combo
	m_ctrlIconScale.ResetContent();
	m_ctrlIconScale.AddString(GetResString(IDS_AUTOMATIC));  // Auto
	m_ctrlIconScale.AddString(_T("100% (16px)"));  // Small
	m_ctrlIconScale.AddString(_T("150% (24px)"));  // Medium
	m_ctrlIconScale.AddString(_T("200% (32px)"));  // Large

	LoadSettings();
	Localize();

	return TRUE;
}

BOOL CPPgAppearance::OnApply()
{
	// Save Icon Scale
	thePrefs.m_iIconScale = m_ctrlIconScale.GetCurSel();

	// Save DPI Aware
	thePrefs.m_bDPIAware = IsDlgButtonChecked(IDC_DPI_AWARE) != 0;

	SetModified(FALSE);
	return CPropertyPage::OnApply();
}

void CPPgAppearance::Localize()
{
	if (m_hWnd) {
		SetWindowText(GetResString(IDS_APPEARANCE));
		
		SetDlgItemText(IDC_GRP_TYPOGRAPHY, GetResString(IDS_TYPOGRAPHY));
		SetDlgItemText(IDC_LBL_APP_FONT, GetResString(IDS_APP_FONT));
		SetDlgItemText(IDC_LBL_LIST_FONT, GetResString(IDS_LIST_FONT));
		SetDlgItemText(IDC_LBL_LOG_FONT, GetResString(IDS_LOG_FONT));
		
		SetDlgItemText(IDC_SELECT_APP_FONT, GetResString(IDS_SELECT_FONT) + _T("..."));
		SetDlgItemText(IDC_SELECT_LIST_FONT, GetResString(IDS_SELECT_FONT) + _T("..."));
		SetDlgItemText(IDC_SELECT_LOG_FONT, GetResString(IDS_SELECT_FONT) + _T("..."));
		
		SetDlgItemText(IDC_GRP_DISPLAY, GetResString(IDS_ICONOGRAPHY));
		SetDlgItemText(IDC_LBL_ICON_SCALE, GetResString(IDS_ICON_SCALE));
		SetDlgItemText(IDC_DPI_AWARE, GetResString(IDS_DPI_AWARE));
		SetDlgItemText(IDC_LBL_DPI_NOTE, GetResString(IDS_NEEDS_RESTART));
	}
}

void CPPgAppearance::OnBnClickedSelectAppFont()
{
	LOGFONT lf = {};
	HDC hDC = ::GetDC(NULL);
	lf.lfHeight = -MulDiv(thePrefs.GetAppFontSize() / 10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	_tcscpy_s(lf.lfFaceName, thePrefs.GetAppFontName());

	CFontDialog dlg(&lf, CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT);
	if (dlg.DoModal() == IDOK) {
		thePrefs.m_strAppFontName = dlg.GetFaceName();
		thePrefs.m_iAppFontSize = dlg.GetSize();
		UpdateFontSamples();
		SetModified();
		// Aplicar fuente inmediatamente a la UI si es necesario
		// theApp.emuledlg->ApplyAppFont(&lf);
	}
}

void CPPgAppearance::OnBnClickedSelectListFont()
{
	LOGFONT lf = {};
	HDC hDC = ::GetDC(NULL);
	lf.lfHeight = -MulDiv(thePrefs.GetListFontSize() / 10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	_tcscpy_s(lf.lfFaceName, thePrefs.GetListFontName());

	CFontDialog dlg(&lf, CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT);
	if (dlg.DoModal() == IDOK) {
		thePrefs.m_strListFontName = dlg.GetFaceName();
		thePrefs.m_iListFontSize = dlg.GetSize();
		UpdateFontSamples();
		SetModified();
	}
}

void CPPgAppearance::OnBnClickedSelectLogFont()
{
	LOGFONT lf = {};
	HDC hDC = ::GetDC(NULL);
	lf.lfHeight = -MulDiv(thePrefs.GetLogFontSize() / 10, GetDeviceCaps(hDC, LOGPIXELSY), 72);
	::ReleaseDC(NULL, hDC);
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	_tcscpy_s(lf.lfFaceName, thePrefs.GetLogFontName());

	CFontDialog dlg(&lf, CF_SCREENFONTS | CF_INITTOLOGFONTSTRUCT | CF_FIXEDPITCHONLY);
	if (dlg.DoModal() == IDOK) {
		thePrefs.m_strLogFontName = dlg.GetFaceName();
		thePrefs.m_iLogFontSize = dlg.GetSize();
		UpdateFontSamples();
		SetModified();
		// Apply to Log windows immediately
		theApp.emuledlg->ApplyLogFont(&lf);
	}
}

void CPPgAppearance::OnHelp()
{
	theApp.ShowHelp(eMule_FAQ_Preferences_Display);
}

BOOL CPPgAppearance::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return (wParam == ID_HELP) ? OnHelpInfo(NULL) : __super::OnCommand(wParam, lParam);
}

BOOL CPPgAppearance::OnHelpInfo(HELPINFO*)
{
	OnHelp();
	return TRUE;
}
