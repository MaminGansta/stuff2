#include "guiAlexandrov/include.h"

#include "stuff.cpp"
#include "gauss.cpp"
#include "simplex_solution.cpp"
#include "artificiant_basis_solution.cpp"
#include "graphic_solution.cpp"
#include "about_window.cpp"
#include "main_window.cpp"







//bool running = true;
//HWND listview;
//int iSelect = 1;
//
//
//LRESULT ProcessCustomDraw(LPARAM lParam) 
//{
//    LPNMLVCUSTOMDRAW lplvcd = (LPNMLVCUSTOMDRAW)lParam;
//    switch (lplvcd->nmcd.dwDrawStage)
//    {
//    case CDDS_PREPAINT: //Before the paint cycle begins
//        //request notifications for individual listview items
//        return CDRF_NOTIFYITEMDRAW;
//
//    case CDDS_ITEMPREPAINT: //Before an item is drawn
//        if (((int)lplvcd->nmcd.dwItemSpec % 2) == 0)
//        {
//            //customize item appearance
//            lplvcd->clrText = RGB(255, 0, 0);
//            lplvcd->clrTextBk = RGB(200, 200, 200);
//            return CDRF_NEWFONT;
//        }
//        else {
//            lplvcd->clrText = RGB(0, 0, 255);
//            lplvcd->clrTextBk = RGB(255, 255, 255);
//
//            return CDRF_NEWFONT;
//        }
//        break;
//
//        //Before a subitem is drawn
//    case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
//        if (iSelect == (int)lplvcd->nmcd.dwItemSpec)
//        {
//            if (0 == lplvcd->iSubItem)
//            {
//                //customize subitem appearance for column 0
//                lplvcd->clrText = RGB(255, 0, 0);
//                lplvcd->clrTextBk = RGB(255, 255, 255);
//
//                //To set a custom font:
//                //SelectObject(lplvcd->nmcd.hdc, 
//                //    <your custom HFONT>);
//
//                return CDRF_NEWFONT;
//            }
//            else if (1 == lplvcd->iSubItem)
//            {
//                //customize subitem appearance for columns 1..n
//                //Note: setting for column i 
//                //carries over to columnn i+1 unless
//                //      it is explicitly reset
//                lplvcd->clrTextBk = RGB(255, 0, 0);
//                lplvcd->clrTextBk = RGB(255, 255, 255);
//
//                return CDRF_NEWFONT;
//            }
//        }
//    }
//    return CDRF_DODEFAULT;
//}
//
//
//LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
//
//    switch (uMsg) {
//    case WM_CREATE:
//    {
//        //Initialise something
//        INITCOMMONCONTROLSEX icex; //Hehehe
//        icex.dwICC = ICC_LISTVIEW_CLASSES;
//        InitCommonControlsEx(&icex);
//
//        //Add listview
//        listview = CreateWindow(WC_LISTVIEW,
//            L"LISTVIEW",
//            WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT,
//            0, 0,
//            400,
//            400,
//            hWnd,
//            0,
//            hInst,
//            NULL);
//
//        ListView_SetExtendedListViewStyle(listview, LVS_EX_FULLROWSELECT);
//
//        //Add column
//        LVCOLUMN lvc;
//        lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
//        lvc.iSubItem = 0;
//        lvc.pszText = (wchar_t*)L"test";
//        lvc.cx = 200;
//        lvc.fmt = LVCFMT_RIGHT;
//        ListView_InsertColumn(listview, 0, &lvc);
//        ListView_InsertColumn(listview, 0, &lvc);
//
//        //Add item
//        LVITEM lvI;
//        lvI.pszText = (wchar_t*)L"item";
//        lvI.mask = LVIF_TEXT;
//        lvI.stateMask = 0;
//        lvI.iSubItem = 0;
//        lvI.iItem = 0;
//        ListView_InsertItem(listview, &lvI);
//
//        lvI.iSubItem = 1;
//        lvI.pszText = (wchar_t*)L"subitem";
//        ListView_SetItem(listview, &lvI);
//
//        //Add item
//        lvI.pszText = (wchar_t*)L"item";
//        lvI.mask = LVIF_TEXT;
//        lvI.stateMask = 0;
//        lvI.iSubItem = 0;
//        lvI.iItem = 1;
//        ListView_InsertItem(listview, &lvI);
//
//        lvI.iSubItem = 1;
//        lvI.pszText = (wchar_t*)L"subitem";
//        ListView_SetItem(listview, &lvI);
//    }
//    break;
//    case WM_CLOSE:
//        DestroyWindow(hWnd);
//        return 0;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        running = false;
//        return 0;
//    case WM_COMMAND:
//        switch (LOWORD(wParam)) {
//        }
//        break;
//    case WM_NOTIFY:
//        if (((LPNMHDR)lParam)->code == NM_CUSTOMDRAW) {
//            LPNMLVCUSTOMDRAW  lplvcd = (LPNMLVCUSTOMDRAW)lParam;
//            switch (lplvcd->nmcd.dwDrawStage) {
//            case CDDS_PREPAINT:
//                return CDRF_NOTIFYITEMDRAW;
//                break;
//            case CDDS_ITEMPREPAINT:
//                if (((int)lplvcd->nmcd.dwItemSpec % 2) == 0) {
//                    lplvcd->clrText = RGB(0, 0, 0);
//                    lplvcd->clrTextBk = RGB(255, 0, 0);
//                }
//                else {
//                    lplvcd->clrText = RGB(0, 0, 255);
//                    lplvcd->clrTextBk = RGB(255, 255, 255);
//                }
//                return CDRF_NEWFONT;
//                break;
//                //There would be some bits here for subitem drawing but they don't seem neccesary as you seem to want a full row color only
//            case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
//                return CDRF_NEWFONT;
//                break;
//            }
//
//
//            return TRUE;
//        }
//    default:
//        return DefWindowProc(hWnd, uMsg, wParam, lParam);
//    }
//
//    return 0;
//}



int WINAPI WinMain(HINSTANCE hinst, HINSTANCE cap, LPSTR csd_line, int showCmd)
{
    al_init(hinst);


    //WNDCLASSEX wc;
    //wc.cbSize = sizeof(wc);
    //wc.style = CS_HREDRAW | CS_VREDRAW;
    //wc.cbClsExtra = 0;
    //wc.cbWndExtra = 0;
    //wc.hInstance = hInst;
    //wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    //wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    //wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    //wc.lpszMenuName = NULL;
    //wc.lpszClassName = L"window";
    ////  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
    //wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    //// global callback function
    //wc.lpfnWndProc = MainWindowProc;
    //
    //
    //if (!RegisterClassEx(&wc))
    //{
    //    MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
    //    assert(false);
    //}
    //
    //HWND handle = CreateWindow(wc.lpszClassName, L"sdaf", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, (HMENU)0, (HINSTANCE)hInst, NULL);


	Main_window main_wnd;

	Window::wait_msg_proc();

	return 0;
}



