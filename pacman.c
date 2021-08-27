#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include <stdint.h>
#include <math.h>

char *clsName = "pacman-1.0";
int xmax, ymax;
POINT maus;
uint8_t *screen;

#define POINTS 40
POINT p[POINTS];
POINT pacman;
int dir[2];

int pacmanInit() {
    int i;
    pacman.x=rand()%xmax;
    pacman.y=rand()%ymax;

    for (i=0;i<POINTS;i++) {
        p[i].x=rand()%xmax;
        p[i].y=rand()%ymax;
    }
}

void kreis(int x, int y, int rad, uint8_t r, uint8_t g, uint8_t b) {
    int i,j;
    int i0,j0;
    for (i=max(0, x-rad-1);i<min(xmax, x+rad+1);i++) {
        for (j=max(0, y-rad-1);j<min(ymax, y+rad+1);j++) {
            i0=i-x;
            j0=j-y;
            if (sqrt(i0*i0+j0*j0)<=rad && screen[4*(i+j*xmax)]==0 && screen[4*(i+j*xmax)+1]==0 && screen[4*(i+j*xmax+2)]==0) {
                screen[4*(i+j*xmax)]=b;
                screen[4*(i+j*xmax)+1]=g;
                screen[4*(i+j*xmax)+2]=r;
            }
        }
    }
}

#define OBEN    1
#define UNTEN   2
#define LINKS   4
#define RECHTS  8
int richtung=LINKS;
unsigned step=0;

void maul(int x, int y, double o, double u, double v) {
    int i,j;
    int i0,j0;

    for (i=max(0, x-20-1);i<min(xmax, x+20+1);i++) {
        for (j=max(0, y-20-1);j<min(ymax, y+20+1);j++) {
            i0=i-x;
            j0=j-y;
            if (richtung==RECHTS && sqrt(i0*i0+j0*j0)<=20 && abs(j0)<abs(i0)*o && i0>0) { // RECHTS
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }

            if (richtung==LINKS && sqrt(i0*i0+j0*j0)<=20 && abs(j0)<abs(i0)*o && i0<0) { // LINKS
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }

            if (richtung==UNTEN && sqrt(i0*i0+j0*j0)<=20 && abs(j0)>abs(i0)/u && j0>0) { // UNTEN
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }

            if (richtung==OBEN && sqrt(i0*i0+j0*j0)<=20 && abs(j0)>abs(i0)/u && j0<0) { // OBEN
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }

            if (richtung==(UNTEN|LINKS) && sqrt(i0*i0+j0*j0)<=20 && i/v<x && j0>0) { // UNTEN LINKS
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }

            if (richtung==(UNTEN|RECHTS) && sqrt(i0*i0+j0*j0)<=20 && i*v>x && j0>0) { // UNTEN RECHTS
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }
            
            if (richtung==(OBEN|RECHTS) && sqrt(i0*i0+j0*j0)<=20 && i*v>x && j0<0) { // OBEN RECHTS
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }

            if (richtung==(OBEN|LINKS) && sqrt(i0*i0+j0*j0)<=20 && i/v<x && j0<0) { // OBEN LINKS
                screen[4*(i+j*xmax)]=0;
                screen[4*(i+j*xmax)+1]=0;
                screen[4*(i+j*xmax)+2]=0;
            }
        }
    }
}

void verteiler() {
    double abstand;
    int i;
    int x0,y0;

    for (i=0;i<POINTS;i++) {
        x0=p[i].x-pacman.x;
        y0=p[i].y-pacman.y;
        abstand=sqrt(x0*x0+y0*y0);

        if (abstand>1.0) {
            kreis(p[i].x, p[i].y, 7.0, 0x99, 0x99, 0xff);
        }
    }
}

int pacmanStep() {
    int i,j,x0,y0;
    int minIdx;
    double minAbstand=xmax*ymax;
    double abstand;
    int no_move=0;

    memset(screen, 0, 4*xmax*ymax);

    for (i=0;i<POINTS;i++) {
        x0=p[i].x-pacman.x;
        y0=p[i].y-pacman.y;
        abstand=sqrt(x0*x0+y0*y0);
        if (abstand<minAbstand) {
            minIdx=i;
            minAbstand=abstand;
        }
        if (abstand<=1.0) {
            p[minIdx].x=rand()%xmax;
            p[minIdx].y=rand()%ymax;
            no_move=1;
        }
    }
    if (no_move) {
        kreis(pacman.x, pacman.y, 20, 0xcc, 0xcc, 0);
        step++;
        maul(pacman.x, pacman.y, (step&4)!=0?1.0:0.2, (step&4)!=0?1.0:0.2, (step&4)!=0?1.0:0.2);
        verteiler();
        return 0;
    }
    
    richtung=0;
    if (pacman.x<p[minIdx].x) {
        pacman.x=min(pacman.x+5, p[minIdx].x);
        richtung=richtung|RECHTS;
    }
    if (pacman.x>p[minIdx].x) {
        pacman.x=max(pacman.x-5, p[minIdx].x);
        richtung=richtung|LINKS;
    }
    if (pacman.y<p[minIdx].y) {
        pacman.y=min(pacman.y+5, p[minIdx].y);
        richtung=richtung|UNTEN;
    }
    if (pacman.y>p[minIdx].y) {
        pacman.y=max(pacman.y-5, p[minIdx].y);
        richtung=richtung|OBEN;
    }
    
    kreis(pacman.x, pacman.y, 20, 0xcc, 0xcc, 0);
    step++;
    maul(pacman.x, pacman.y, (step&4)!=0?1.0:0.2, (step&4)!=0?1.0:0.2, (step&4)!=0?1.0:0.2);
    verteiler();

    return 1;
}

enum runtype { SCR_PREV, SCR_CONFIG, SCR_RUN, SCR_ERRPREV };

LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);

enum runtype getRunType(LPSTR args, HWND *hWnd) {
    long long res;

    if (strlen(args)<2) {
        return SCR_RUN;
    }
    if (*args!='/') {
        return SCR_CONFIG;
    }
    args++;
    switch (*args) {
        case 'c':
        case 'C':
            return SCR_CONFIG;
        
        case 'p':
        case 'P':
            args++;
            if (strlen(args)==0) {
                return SCR_ERRPREV;
            }
            res=0;
            while (*args==' ') {
                args++;
            }
            while (*args!='\0') {
                char c=*args;
                int ziff;
                args++;
                if (c<'0' || c>'9') {
                    return SCR_ERRPREV;
                }
                ziff=c-'0';
                res=10*res+ziff;
            }
            *hWnd=(HWND)res;
            return SCR_PREV;
    }

    return SCR_RUN;
}

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR szCmdLine, int show) {
    MSG messages;
    WNDCLASSEX wndcl;
    HWND hWnd;


    wndcl.cbClsExtra=0;
    wndcl.cbSize=sizeof(WNDCLASSEX);
    wndcl.cbWndExtra=0;
    wndcl.hbrBackground=(HBRUSH)CreateSolidBrush(RGB(0,0,0));
    wndcl.hCursor=NULL;
    wndcl.hIcon=LoadIcon(NULL, IDI_APPLICATION);
    wndcl.hIconSm=LoadIcon(NULL, IDI_APPLICATION);
    wndcl.hInstance=hThisInstance;
    wndcl.lpfnWndProc=wndProc;
    wndcl.lpszClassName=clsName;
    wndcl.lpszMenuName=NULL;
    wndcl.style=0;

    srand(time(NULL));

    switch (getRunType(szCmdLine, &hWnd)) {
        case SCR_RUN:
            xmax = GetSystemMetrics(SM_CXSCREEN);
            ymax = GetSystemMetrics(SM_CYSCREEN);

            if (xmax==0 || ymax==0) {
                MessageBox(NULL, "Konnte Bildschirmdimension nicht abfragen.", "GetSystemMetrics", MB_ICONERROR|MB_OK);
                return -1;
            }

            if (RegisterClassEx(&wndcl)==0) {
                MessageBox(NULL, "Konnte Fensterklasse nicht registrieren.", "RegisterClassEx", MB_ICONERROR|MB_OK);
                return -1;
            }

            hWnd=CreateWindowEx(
                WS_EX_APPWINDOW, clsName, NULL, WS_VISIBLE, 0, 0,
                xmax, ymax, HWND_DESKTOP, NULL, hThisInstance,  NULL
            );
            if (hWnd==NULL) {
                MessageBox(NULL, "Fenster konnte nicht erstellt werden.", "CreateWindowEx", MB_ICONERROR|MB_OK);
                return -1;
            }

            if (GetCursorPos(&maus)==0) {
                MessageBox(NULL, "Kursorposition konnte nicht ermittelt werden.", "GetCursorPos",MB_ICONERROR|MB_OK);
                return -1;
            }
            if (ShowWindow(hWnd, show)==0) {
                MessageBox(NULL, "Bildschirmschoner konnte nicht hergestellt werden", "ShowWindow", MB_ICONERROR|MB_OK);
                return -1;
            }
            break;

        case SCR_PREV: {
            return 0; // Vorschau bisher nicht implementiert.
        }
        case SCR_CONFIG:
            MessageBox(0,"Dieser Bildschirmschoner ist nicht konfigurierbar.",0 , 0);
            return 1;

        case SCR_ERRPREV:
            MessageBox(0,"Ungültige Parameter.", 0, 0);
            break;
    }

    screen=malloc(xmax*ymax*4);
    pacmanInit();
    memset(screen, 0, xmax*ymax*4);
    dir[0]=1;
    dir[1]=1;

    while (GetMessage(&messages, NULL, 0, 0)) {
        TranslateMessage(&messages);
        DispatchMessage(&messages);
    }

    return 0;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    RECT rct;

    switch (msg) {
        case WM_CREATE: {
            UINT_PTR tid=1;
            DWORD dw;

            dw=GetWindowLong(hWnd, GWL_STYLE);
            dw=dw&(~(WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_MAXIMIZEBOX));
            SetWindowLong(hWnd, GWL_STYLE, dw);
            SetTimer(hWnd, tid, 50, NULL);
            break;
        }

        case WM_TIMER: {
            HDC hdc, hdcMem;
            HBITMAP bmp;
            HGDIOBJ obj;

            pacmanStep();
            hdc=GetDC(hWnd);
            if (hdc==NULL) {
                PostQuitMessage(-1);
            }
            hdcMem=CreateCompatibleDC(hdc);
            if (hdcMem==0) {
                PostQuitMessage(-1);
            }

            bmp=CreateBitmap(xmax, ymax, 1, 32, screen);
            obj=SelectObject(hdcMem, bmp);
            if (obj==NULL || obj==HGDI_ERROR || bmp==NULL) {
                PostQuitMessage(-1);
            }
            if (BitBlt(hdc, 0, 0, xmax, ymax, hdcMem, 0, 0, SRCCOPY)==0) {
                PostQuitMessage(-1);
            }
            if (DeleteObject(bmp)==0 || DeleteDC(hdcMem)==0 || ReleaseDC(hWnd, hdc)==0) {
                PostQuitMessage(-1);
            }
            break;
        }

        case WM_CLOSE:
            PostQuitMessage(0);
            break;


        //DefScreenSaverProc:
        case WM_ACTIVATE:
        case WM_ACTIVATEAPP:
        case WM_NCACTIVATE:
            if (wParam==FALSE) {
                SendMessage(hWnd, WM_CLOSE, 0, 0);
            }
            break;

        case WM_SETCURSOR:
            SetCursor(NULL);
            return TRUE;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_KEYDOWN:
        case WM_KEYUP:
            PostQuitMessage(0);
            break;

        case WM_MOUSEMOVE: {
            DWORD lmp=(maus.y<<16)|maus.x;
            if (lmp!=lParam) {
                PostQuitMessage(0);
            }
            return 0;
        }

        case WM_DESTROY:
            SendMessage(hWnd, WM_CLOSE, 0, 0);
            break;

        case WM_SYSCOMMAND:
            if (wParam==SC_CLOSE || wParam==SC_SCREENSAVE) {
                return FALSE;
            }
            break;

        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}