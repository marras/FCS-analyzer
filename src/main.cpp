#include "program.h"
#include <iostream>

#ifdef __WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif //__WIN32

#include <stdio.h>
#include <malloc.h>
 
int main(int argc, char *argv[])
{
 #ifdef __WIN32
    // Create the application's console
    if ( AllocConsole() == 0 ) {
       ::MessageBox( 0, L"Can't allocate console!\n", L"** FCS_analyzer **", MB_OK | MB_ICONSTOP );
       return false;
     }

    //(?) add better error handling code later...   freopen( "CON", "w", stderr);
    freopen( "CON", "w", stdout);
//    std::ios::sync_with_stdio();
 
    // This is often nifty to know...
    char* cwd = new char[ MAX_PATH + 1 ];
    GetCurrentDirectoryA( MAX_PATH, cwd );
    std::cout << "CWD = " << cwd << std::endl;
    delete [] cwd;
 #endif //__WIN32

    //QApplication a(argc, argv);
    QApplication app(argc, argv);

    Program * okno = new Program;
    okno->show();

    app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );
    int app_rv = app.exec();

    return app_rv;
}
