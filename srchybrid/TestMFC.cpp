#include <afxwin.h>
#include <atlstr.h>
#include <stdio.h>

void test() {
  CString s = _T("Hello");
  printf("%S", (LPCTSTR)s);
}
