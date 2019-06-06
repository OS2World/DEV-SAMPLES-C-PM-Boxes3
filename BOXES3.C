/*----------------------------------------------------------------------
   BOXES3.C -- OS/2 PM program that draws nested boxes in client window
               (c) 1988, Ziff Communications Company
               PC Magazine * Charles Petzold, 5/88
  ----------------------------------------------------------------------*/

#define INCL_WIN
#include <os2.h>
#include <stddef.h>
#include "boxes3.h"

SHORT usNumBoxes  = 50 ;      /* number of boxes to draw (initially)    */
SHORT usIncrement = 20 ;      /* rotate each 1/20th of side (initially) */

MRESULT EXPENTRY ClientWndProc (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY AboutDlgProc  (HWND, USHORT, MPARAM, MPARAM) ;
MRESULT EXPENTRY NumIncDlgProc (HWND, USHORT, MPARAM, MPARAM) ;

int main (void)
     {
     static CHAR szClientClass [] = "Boxes3" ;
     HAB         hab ;
     HMQ         hmq ;
     HWND        hwndFrame, hwndClient ;
     QMSG        qmsg ;

     hab = WinInitialize (0) ;
     hmq = WinCreateMsgQueue (hab, 0) ;

     WinRegisterClass (
                    hab,                /* Anchor block handle             */
                    szClientClass,      /* Name of class being registered  */
                    ClientWndProc,      /* Window procedure for class      */
                    CS_SIZEREDRAW,      /* Class style                     */
                    0) ;                /* Extra bytes to reserve          */

     hwndFrame = WinCreateStdWindow (
                    HWND_DESKTOP,       /* Parent window handle            */
                    WS_VISIBLE          /* Style of frame window           */
                         | FS_SIZEBORDER
                         | FS_TITLEBAR
                         | FS_SYSMENU
                         | FS_MINMAX
                         | FS_MENU,
                    szClientClass,      /* Client window class name        */
                    szClientClass,      /* Title bar text                  */
                    0L,                 /* Style of client window          */
                    NULL,               /* Module handle for resources     */
                    ID_MAINMENU,        /* ID of resources                 */
                    &hwndClient) ;      /* Pointer to client window handle */

     while (WinGetMsg (hab, &qmsg, NULL, 0, 0))
          WinDispatchMsg (hab, &qmsg) ;

     WinDestroyWindow (hwndFrame) ;
     WinDestroyMsgQueue (hmq) ;
     WinTerminate (hab) ;

     return 0 ;
     }

          /*-------------------------
             Client window procedure
            -------------------------*/

MRESULT EXPENTRY ClientWndProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static LONG  alColors [] = { CLR_BLACK, CLR_BLUE, CLR_RED,    CLR_PINK,
                                  CLR_GREEN, CLR_CYAN, CLR_YELLOW, CLR_WHITE,
                                  CLR_DARKGRAY, CLR_DARKBLUE,  CLR_DARKRED,
                                  CLR_DARKPINK, CLR_DARKGREEN, CLR_DARKCYAN,
                                  CLR_BROWN,    CLR_LIGHTGRAY } ;

     static SHORT xClient, yClient, idBackground = IDM_BACKGROUND + 7,
                                    idForeground = IDM_FOREGROUND ;
     HPS          hps;
     HWND         hwndMenu ;
     POINTL       aptl[5] ;
     SHORT        sRep, sSide ;

     switch (msg)
          {
          case WM_COMMAND:
               hwndMenu = WinWindowFromID (
                              WinQueryWindow (hwnd, QW_PARENT, FALSE),
                              FID_MENU) ;

               if (COMMANDMSG(&msg)->cmd >= IDM_BACKGROUND &&
                   COMMANDMSG(&msg)->cmd <= IDM_BACKGROUND + 15)
                    {
                    WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                   MPFROM2SHORT (idBackground, TRUE),
                                   MPFROM2SHORT (MIA_CHECKED, 0)) ;

                    idBackground = COMMANDMSG(&msg)->cmd ;

                    WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                   MPFROM2SHORT (idBackground, TRUE),
                                   MPFROM2SHORT (MIA_CHECKED, MIA_CHECKED)) ;

                    WinInvalidateRect (hwnd, NULL, FALSE) ;
                    }

               else if (COMMANDMSG(&msg)->cmd >= IDM_FOREGROUND &&
                        COMMANDMSG(&msg)->cmd <= IDM_FOREGROUND + 15)
                    {
                    WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                   MPFROM2SHORT (idForeground, TRUE),
                                   MPFROM2SHORT (MIA_CHECKED, 0)) ;

                    idForeground = COMMANDMSG(&msg)->cmd ;

                    WinSendMsg (hwndMenu, MM_SETITEMATTR,
                                   MPFROM2SHORT (idForeground, TRUE),
                                   MPFROM2SHORT (MIA_CHECKED, MIA_CHECKED)) ;

                    WinInvalidateRect (hwnd, NULL, FALSE) ;
                    }

               else switch (COMMANDMSG(&msg)->cmd)
                    {
                    case IDM_ABOUT:
                         WinDlgBox (HWND_DESKTOP, hwnd, AboutDlgProc,
                                        NULL, IDD_ABOUTBOX, NULL) ;
                         break ;

                    case IDM_NUMINC:
                         if (WinDlgBox (HWND_DESKTOP, hwnd, NumIncDlgProc,
                                        NULL, IDD_NUMINCBOX, NULL))

                              WinInvalidateRect (hwnd, NULL, FALSE) ;
                         break ;

                    default:
                         break ;
                    }
               break ;

          case WM_SIZE:
               xClient = SHORT1FROMMP (mp2) ;
               yClient = SHORT2FROMMP (mp2) ;
               break ;

          case WM_PAINT:
               hps = WinBeginPaint (hwnd, NULL, NULL) ;

               aptl[0].x = aptl[1].x = 0 ;
               aptl[2].x = aptl[3].x = xClient - 1 ;
               aptl[0].y = aptl[3].y = 0 ;
               aptl[1].y = aptl[2].y = yClient - 1 ;

               GpiSetColor (hps, alColors[idBackground - IDM_BACKGROUND]) ;

               GpiMove (hps, aptl) ;
               GpiBox (hps, DRO_FILL, aptl + 2, 0L, 0L) ;

               GpiSetColor (hps, alColors[idForeground - IDM_FOREGROUND]) ;

               for (sRep = 0 ; sRep < usNumBoxes ; sRep ++)
                    {
                    aptl[4] = aptl[0] ;

                    GpiMove (hps, aptl) ;
                    GpiPolyLine (hps, 4L, aptl + 1) ;

                    for (sSide = 0 ; sSide < 4 ; sSide++)
                         {
                         aptl[sSide].x = ((usIncrement - 1) * aptl[sSide].x +
                                             aptl[sSide + 1].x +
                                             usIncrement / 2) / usIncrement ;

                         aptl[sSide].y = ((usIncrement - 1) * aptl[sSide].y +
                                             aptl[sSide + 1].y +
                                             usIncrement / 2) / usIncrement ;
                         }
                    }
               WinEndPaint (hps) ;
               break ;

          default:
               return WinDefWindowProc (hwnd, msg, mp1, mp2) ;
          }
     return FALSE ;
     }

          /*----------------------------
             About box dialog procedure
            ----------------------------*/

MRESULT EXPENTRY AboutDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     switch (msg)
          {
          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                    case DID_CANCEL:
                         WinDismissDlg (hwnd, TRUE) ;
                         break ;

                    default:
                         break ;
                    }
               break ;

          default:
               return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
          }
     return FALSE ;
     }

          /*---------------------------------------
             Number and increment dialog procedure
            ---------------------------------------*/

MRESULT EXPENTRY NumIncDlgProc (HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2)
     {
     static USHORT usLocalNumBoxes, usLocalIncrement ;

     switch (msg)
          {
          case WM_INITDLG:
               WinSendDlgItemMsg (hwnd, IDD_NUMBOXES, EM_SETTEXTLIMIT,
                                  MPFROMSHORT(2), NULL) ;

               WinSendDlgItemMsg (hwnd, IDD_INCREMENT, EM_SETTEXTLIMIT,
                                  MPFROMSHORT(2), NULL) ;

               WinSetDlgItemShort (hwnd, IDD_NUMBOXES,  usNumBoxes,  FALSE) ;
               WinSetDlgItemShort (hwnd, IDD_INCREMENT, usIncrement, FALSE) ;
               break ;

          case WM_COMMAND:
               switch (COMMANDMSG(&msg)->cmd)
                    {
                    case DID_OK:
                         if (WinQueryDlgItemShort (hwnd, IDD_NUMBOXES,
                                        &usLocalNumBoxes, FALSE) &&

                                   WinQueryDlgItemShort (hwnd, IDD_INCREMENT,
                                        &usLocalIncrement, FALSE) &&

                                   usLocalNumBoxes > 0 && usLocalIncrement > 0)
                              {
                              usNumBoxes  = usLocalNumBoxes ;
                              usIncrement = usLocalIncrement ;

                              WinDismissDlg (hwnd, TRUE) ;
                              }
                         else
                              WinMessageBox (HWND_DESKTOP, hwnd, 
                                   "Values must be greater than zero.",
                                   "BOXES3", 0, MB_ICONHAND | MB_OK) ;
                         break ;

                    case DID_CANCEL:
                         WinDismissDlg (hwnd, FALSE) ;
                         break ;

                    default:
                         break ;
                    }
               break ;

          default:
               return WinDefDlgProc (hwnd, msg, mp1, mp2) ;
          }
     return FALSE ;
     }
