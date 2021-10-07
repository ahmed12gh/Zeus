// In The Name Of God , Bless this code
// & show me every error and bug i made ,
// & and give me the power to Not cry

#define UNICODE
#define _UNICODE // must be defined before importing windows.h


#ifdef DEBUG 
    #define debug 1 
#else 
    #define debug 0 
#endif

#include <time.h> 
#include <stdio.h>
#include <windows.h>
#include "resources.h"
#include <tchar.h>
#include <richedit.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "ini.h"
#include "main.h"

#define Font L"Hack"
#define FontNameI 40
// TODO : Clean code put some in funtions
//        2. confg file


HWND wd_handle;
WNDPROC OldInputControlProc;
WNDPROC OldOutputControlProc;
INITCOMMONCONTROLSEX initCommControls;
float scale = 1 ; 

varable VarList[VARABLEs_LIMIT];
int listlastIndex = 0;

// configration
char configFile [MAX_PATH];
char HistoryFile [MAX_PATH];
int WindowWidth = 500;
int WindowHeight = 400;

long height ;
int fontsize = 14;
char font [FontNameI] = "Segoe UI";

COLORREF Result_color = RGB(133, 90, 200);

bool CopyCertainResultToClipbarod = true ;
bool historyFileSetting = true ; 

void copyToClipBoared( HWND Hwnd , char * text){

    size_t size = strlen(text); 
    if(size < 2) return ;  

    if(!OpenClipboard(Hwnd)) return; 
    EmptyClipboard() ; 

    HGLOBAL globe =  GlobalAlloc(GMEM_FIXED , LENNUMBER) ; 
    memcpy(globe , text , LENNUMBER) ; 

    SetClipboardData(CF_TEXT , globe ) ; 
    CloseClipboard() ; 

}


COLORREF charArrToRgb(const char *arr)
{
    printf(arr) ;
    int red, green, blue;
    char tempArr [15];
    strcpy(tempArr, arr);

    red = Arr2I(strtok(tempArr, ","));
    green = Arr2I(strtok(NULL, ","));
    blue = Arr2I(strtok(NULL, ","));

    printf("\n %d , %d , %d \n" , red , green , blue) ;
    return RGB(red, green, blue);
}

void SetStdOutToNewConsole()
{
    // allocate a console for this app
    AllocConsole();
    // redirect unbuffered STDOUT to the console
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    int fileDescriptor = _open_osfhandle((intptr_t)consoleHandle, _A_SYSTEM);
    FILE *fp = _fdopen(fileDescriptor, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);
    // give the console window a nicer title
    SetConsoleTitle(L"Debug Output");
    // give the console window a bigger buffer size
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(consoleHandle, &csbi))
    {
        COORD bufferSize;
        bufferSize.X = csbi.dwSize.X;
        bufferSize.Y = 9999;
        SetConsoleScreenBufferSize(consoleHandle, bufferSize);
    }
}

void AppendTextToControl(HWND hControl, wchar_t *text)
{
    // getting the curser
    DWORD startPos, endPos;
    SendMessage(hControl, EM_GETSEL, (WPARAM)&startPos, (WPARAM)&endPos);

    // moving the curserto the end ;
    int controlLen = GetWindowTextLength(hControl) + 1; // end
    SendMessage(hControl, EM_SETSEL, controlLen, controlLen);

    // setting the text at teh end
    SendMessage(hControl, EM_REPLACESEL, TRUE, (LPARAM)text);
}

void openTextFile(char* fn ) {
     // create the file if does not exist  
	if (is_str(fn , configFile)){ // if the file is the config file 
		setConfig(configFile) ; 
	}
	else{ // if the history file 
	    FILE* f = fopen(fn,"a+") ;
	    fclose(f) ;
	} 
	char command [18+MAX_PATH] = "\"powershell.exe\" " ; 
	strcat(command , fn) ; 
	system(command);
} 

LRESULT CALLBACK outputProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    // TODO  : deny \n delete or backdelete only when removeing the whole line
    switch (msg)
    {
    case WM_CHAR:
    {
        switch (wp)
        {
        case VK_RETURN:
            // recalculate every single formula from certain line and down
            break;

        default:
            CallWindowProc(OldOutputControlProc, hwnd, msg, wp, lp);
            break;
        }
    }break;

    default:
        CallWindowProc(OldOutputControlProc, hwnd, msg, wp, lp);
        break;
    }
}

HWND CreateRichEdit(HWND Parent, int ID)
{
    // for now Output is readonly until i  recalculating  working
    HWND OutputControl = CreateWindowEx( 0 , RICHEDIT_CLASS, _T(""), WS_CHILD | WS_VISIBLE | ES_AUTOVSCROLL | ES_AUTOHSCROLL  | ES_MULTILINE | WS_VSCROLL | WS_HSCROLL | WS_BORDER | ES_READONLY,
                                        0, 0, 100, 100, Parent, (HMENU)ID, GetModuleHandle(NULL), NULL);
    LONG lExStyle = GetWindowLong(OutputControl, GWL_EXSTYLE);
    lExStyle &= ~WS_EX_CLIENTEDGE;
    SetWindowLong(OutputControl, GWL_EXSTYLE, lExStyle);

    // formating for the output control
    CHARFORMAT2 cf = {sizeof(CHARFORMAT2)};
    cf.yHeight = fontsize * (20) * scale ;  // twips 
    cf.dwMask = CFM_FACE |  CFM_SIZE | CFM_EFFECTS  ; // cfm_effects it what enable wWeight proprtey 
    cf.wWeight = 300 ; 
    wchar_t LF [FontNameI] ;
    mbstowcs(LF , font , FontNameI) ;
    wsprintf(cf.szFaceName , L"%s" , LF) ;

    OldOutputControlProc = (WNDPROC)SetWindowLongPtr(OutputControl, GWL_WNDPROC, (LONG_PTR)outputProc);

    SendMessage(OutputControl, EM_SETCHARFORMAT, SCF_ALL, (LPARAM)&cf);
    return OutputControl;
}

LRESULT CALLBACK InputProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
    switch (msg)
    {
    case WM_KEYDOWN:
        CallWindowProc(OldInputControlProc, hwnd, msg, wp, lp);
        break;

    case WM_CHAR:
    {
        switch (wp)

        {
        case VK_RETURN:
        {
            // getting the handler
            HWND hOut = GetDlgItem(wd_handle, Output_ID);

            // get the text to send it
            int Tlength = GetWindowTextLength(hwnd) + 1;
            wchar_t Text[Tlength];
            char cText[Tlength+ (formulaLen * LENNUMBER)];
            ZeroMemory(cText , Tlength+ (formulaLen * LENNUMBER)) ; 
            GetWindowText(hwnd, Text, Tlength);
            wprintf(L"InputProc() : %s \n ", Text);

            wcstombs(cText, Text, Tlength + 1);
            if (Tlength > 1){
                AppendTextToControl(hOut, Text);
                if (historyFileSetting) writeToFile(HistoryFile , cText ) ;
            }

            removeChar(cText , ' '); // remove spaces from formula 
            printf("text after removeing spaces from it : %s\n" , cText) ; 
            MultplieCloseVar(cText) ; 
            
            // check for varables
            if (charInStr(cText, '='))
            {
                //  redefing a varable
                varable var = check_Varables(cText, Tlength);
                replace_Varables_by_values(VarList, listlastIndex, cText, Tlength, 0, NULL, -1);
                NUMBER value = do_formulaArr(decode_formula(cText));

                varable *v = search_ListPtr(VarList, var.name, 1);
                printf("inputproc(): varable name is %s \n", v->name);
                if (is_str("-1", v->name) == true)
                {
                    // didnot found . new varable
                    printf("inputproc(): adding a varable \n ");
                    var.value = Num2F(value);
                    appendToVarList(VarList, &listlastIndex, var);
                    free(v);
                }
                else if (is_str("-1", v->name) == false)
                {

                    //change
                    printf("inputproc() : change a varable value \n");
                    v->value = Num2F(value);
                }

                printf("inputwndproc (): varlist index %d is '%s' = %f \n ", listlastIndex - 1, VarList[listlastIndex - 1].name, VarList[listlastIndex - 1].value);
            }
            replace_Varables_by_values(VarList, listlastIndex, cText, Tlength, 0, NULL, -1);

            printf("Inputproc(): varables are replaced text is %s \n", cText);
            wprintf(L"InputProc() : %s \n ", Text);

            NUMBER result = do_formulaArr(decode_formula(cText)); // acutual calaclating for result
            
            printf("input win proc() : text after removing the varable declartion from it %s \n ", cText);
            printf("input win proc() : result after solving  \n ");
            PrintNum(result);

            // Converting char to wchar_t
            int size = result.lastfilledIndex ;
            wchar_t TcharResult[size + 1];
            mbstowcs(TcharResult, result.arr, size + 1);

            wprintf(L"InputProc() : %s = %s \n", Text, TcharResult);
            printf("%ld result color value in input proc\n" , Result_color) ;

            // color the result 
            CHARFORMAT2 charFormat = {sizeof(CHARFORMAT2)};
            charFormat.dwMask = CFM_COLOR | CFM_BOLD;
            charFormat.dwEffects = CFM_BOLD;
            charFormat.crTextColor = Result_color;

            // appending the text
            if (Tlength > 1)
            {
                // add the result
                AppendTextToControl(hOut, L" = ");
                AppendTextToControl(hOut, TcharResult);
                AppendTextToControl(hOut, L"\n");

                if (historyFileSetting){
                    writeToFile(HistoryFile , " = " ) ;
                    writeToFile(HistoryFile , result.arr ) ;
                    writeToFile(HistoryFile , "\n" ) ;
                }  

                if(CopyCertainResultToClipbarod ) copyToClipBoared(wd_handle , result.arr) ;   

                // color the result
                int len = GetWindowTextLength(hOut);
                int lineNumber = SendMessage(hOut, EM_LINEFROMCHAR, -1, (LPARAM)NULL); // set the text to color
                SendMessage(hOut, EM_SETSEL, len - size - 3 - lineNumber, len - 1 - lineNumber);
                SendMessage(hOut, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&charFormat);
            }

            SetWindowText(hwnd, NULL); // clear the input
            SetFocus(hOut) ; 
            SendMessage(hOut, EM_LINESCROLL , INFINITE, INFINITE);
            SetFocus(hwnd) ;
            break;
        }

        case 0x7F: // Ctrl + Backspace
            SetWindowText(hwnd, NULL);
            break;

        default:
            CallWindowProc(OldInputControlProc, hwnd, msg, wp, lp);
            break;
        }
        break;
    }

    default:
        CallWindowProc(OldInputControlProc, hwnd, msg, wp, lp);
        break;
    }
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT Msg, WPARAM Wparam, LPARAM Lparam)
{

    switch (Msg)
    {
    case WM_CREATE:
    {

        // get date of today to history file 
        if (historyFileSetting) {
            time_t t = time(NULL); 
            struct tm  *tm = localtime(&t);
            char str [68]  ;  
            strftime(str , 68 , "\n[ %Y- %m- %d ]\n" , tm) ;
            writeToFile(HistoryFile , str) ;
        }

        FillNullVarableList(VarList); // clear varable list from any thing 

        HDC hdc = GetDC(NULL);

        // getting a  font
        HFONT Defaultfont;
        height = -MulDiv(fontsize, GetDeviceCaps(hdc, LOGPIXELSY), 72) * scale;
        ReleaseDC(NULL, hdc);

        wchar_t  LFont [FontNameI]  ;
        mbstowcs(LFont , font , FontNameI) ;
        printf("wndproc(): font = %s" , font);
        wprintf(L" Lfont = %s\n" ,LFont) ;

        Defaultfont = CreateFont(height, 0, 0, 0, 300, FALSE, 0, 0, 0, 0, 0, 0, 0, LFont );

        // set window Icon 
        HICON Icon, BigIcon;
        Icon = LoadImage(NULL, MainIcon_P, IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
            if (Icon)
                SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)Icon);
            
        BigIcon = LoadImage(NULL, MainIcon_P, IMAGE_ICON, 128, 128, LR_LOADFROMFILE);
            if (BigIcon)
                SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)BigIcon);
            
        

        LoadLibrary(L"Riched20.dll");

        HWND InputControl = CreateWindowEx(WS_EX_CLIENTEDGE, _T("edit"), _T(""), WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                           0, 250, 500, 50, hwnd, (HMENU)Input_ID, GetModuleHandle(NULL), NULL);

        HWND OutputControl = CreateRichEdit(hwnd, Output_ID);

        OldInputControlProc = (WNDPROC)SetWindowLongPtr(InputControl, GWL_WNDPROC, (LONG_PTR)InputProc); // seting the winproc funtion for Input handler

        SendMessage(InputControl, WM_SETFONT, (WPARAM)Defaultfont, MAKELPARAM(FALSE, 0)); // for edit control
        SetFocus(InputControl);
    }
    break;

    case WM_SIZE:
    {
        RECT rect;
        GetClientRect(hwnd, &rect);

        HWND hInputControl = GetDlgItem(hwnd, Input_ID);
        HWND outputControl = GetDlgItem(hwnd, Output_ID);
        // resize

        int h ;  
        if(fontsize <= 18 ) h = (height * -1) + (int)(-height*1  ); // height is negtaive
        if(fontsize > 18 ) h = (height * -1) + (int)(-height*0.5   ); 
        int w = (int) 3 * scale ; // how far the border from edge of window
        //                                x               y                    x2                        y2
        SetWindowPos(outputControl, NULL, rect.left + w , rect.top + w         , rect.right -w*2          , rect.bottom - h - w , 0);
        SetWindowPos(hInputControl, NULL, rect.left + w , rect.bottom - h + w  , rect.right -w*2          , h - w*2   , 0);
    }
    break;

    case WM_COMMAND:
    {           
        // menu items action ids
        switch (LOWORD(Wparam))
        {
        case OpenConifFile_ID:
            openTextFile(configFile) ;
            break;
        
        case Clear_Input_ID :{
            HWND input = GetDlgItem(hwnd , Input_ID) ;
            SetWindowText(input , NULL) ; 
            }break;
        
        case Clear_Output_ID : {
            HWND output = GetDlgItem(hwnd , Output_ID) ;
            SetWindowText(output , NULL) ; 
            }break ; 
        
        case OpenHistoryFile_ID : 
            openTextFile(HistoryFile) ; 
            break; 
        
        case deleteHistoryFile_ID:
            DeleteFileA(HistoryFile) ;
            break;

        case Rest_Config_file_ID :
            DeleteFileA(configFile) ; 
            setConfig(configFile) ; 

        default:
            DefWindowProc(hwnd, Msg, Wparam, Lparam);
            break;
        }
        break; // ** keep the loop other wise the program crash
    }


    case WM_CLOSE:    
        DestroyWindow(hwnd);
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        DefWindowProc(hwnd, Msg, Wparam, Lparam);
        break;
    }
}

void confgPath(char *filename , const char* FileToCreate)
{
 
    char fn [MAX_PATH] ;
    GetModuleFileNameA(NULL , fn , MAX_PATH) ; 

    char * tok = strtok(fn , "\\") ; 
    char Path [MAX_PATH][25] ;
    int index = 0 ; 

    // split by \\ 
    strcpy(Path[index++] , tok) ; 
    char* t = strtok(NULL , "\\") ;

    while ( t != NULL)
    {
        strcpy (Path[index++] , t) ; 
        t = strtok(NULL , "\\") ;   
    }

    // rejoing the string
    strcpy(Path[index-1] , FileToCreate ) ; // change from exe path to config path 
    ZeroMemory(filename , MAX_PATH )  ; // clear it from anything 

    strcat(filename , tok ) ; 
    for(int i = 0 ; i <  index ; i++ ){
        strcat(filename , "/") ; 
        strcat(filename , Path[i]); 
    } 

}

void setConfig(char * fn ){
    
    FILE *file;
    file = fopen(fn , "r") ;

    printf("%s = %ld \n", fn , file);

    if (file == NULL )
    {   
        fclose(file) ; 

        file = fopen(fn, "a+");
        if(file == NULL) printf("failed\n") ;

        fprintf(file, ";this is a config file for KEYCAL-C app \n");

        fprintf(file, "[window] \n");
        fprintf(file, ";the width and height in pixels\n");
        fprintf(file, "width=500 \n");
        fprintf(file, "height=400 \n");
        fprintf(file, ";scale can be any value between 0.5 and 2 \n");
        fprintf(file, "scale=1 \n");

        fprintf(file, "[text] \n");
        fprintf(file, "font=Segoe UI\n");
        fprintf(file, "fontsize=14\n");
        fprintf(file, ";color values use the rgb format as known red , green , blue as long as this value dont excced 255\n ");
        fprintf(file, "result color=133, 179, 181\n");
        fprintf(file,"[settings]\n") ; 
        fprintf(file,";either 'true' or 'false' or 'on' or 'off' \n") ; 
        fprintf(file ,"copy to clipboared = true \n") ;    
        fprintf(file ,"history file = true \n") ; 

        fclose(file) ; 
    }

    if( (file != NULL) ){
        ini_t *config = ini_load(fn) ;

        WindowWidth = Arr2I(NotNullorValue(ini_get(config, "window", "width") , "400"))  ;
        WindowHeight = Arr2I(NotNullorValue(ini_get(config, "window", "height") , "600"))  ;
        scale = Arr2F(NotNullorValue(ini_get(config, "window", "scale") , "1"))  ;

        if(scale > 2) scale = 2 ;
        else if (scale < 0.5) scale = 0.5 ;   

        fontsize = Arr2I(NotNullorValue(ini_get(config, "text", "fontsize") , "20"))  ;
        const char * f = NotNullorValue( ini_get(config , "text" , "font") , "Segoe UI") ;
        if(f != "") sprintf(font ,"%s", f) ; 

        Result_color = charArrToRgb( NotNullorValue( ini_get(config, "text", "result color") , "133 , 179 , 181"));

        historyFileSetting = CharToBool( NotNullorValue(ini_get(config ,"settings" , "history file") , "true" ))  ; 
        CopyCertainResultToClipbarod = CharToBool( NotNullorValue(ini_get(config ,"settings" , "copy to clipboared") , "true" ))  ; 
        
        printf("historu file %d\n" , historyFileSetting) ; 
        ini_free(config) ;
        fclose(file) ; 

    }
    else{
        printf("setconfig():failed \n");
        fclose(file) ; 
    }
        fclose(file) ; 

}


int WINAPI WinMain(HINSTANCE Hinstance, HINSTANCE PrevInstance, LPSTR CmdArgsStr, int nCmdShow)
{
    
    if(debug) SetStdOutToNewConsole();
    
  
    confgPath(configFile , "Config.ini");
    confgPath(HistoryFile , "History.txt");
    setConfig(configFile) ;

    WNDCLASSEX wd;
    MSG Msg;

    wchar_t szWindowClass[] = L"Zeus"; // can't bigger than 256 char
    wchar_t Title[] = L"Zeus";

    // 1. add values to class (struct)
    wd.cbSize = sizeof(WNDCLASSEX);     // the byte size of window always set to wndclassex
    wd.style = CS_HREDRAW | CS_VREDRAW; // redraw if move or adjust in width or height | could do more stuff
    wd.cbClsExtra = 0;                  // bytes the system give to the program
    wd.cbWndExtra = 0;                  // == but for the window instance only
    wd.lpfnWndProc = WinProc;           // set windows prouduce to callback after getting message
    wd.hInstance = Hinstance;           //handle to the instance that contains the window procedure for the class
    // wd.hIcon = LoadIcon(wd.hInstance, MAKEINTRESOURCE(MAINICON_ID));   // icon in the windows explorer
    wd.hIcon = (HICON)LoadImage(wd.hInstance, MAKEINTRESOURCE(MAINICON_ID), IMAGE_ICON, 512, 512, 0); // icon in the windows explorer
    wd.hIconSm = (HICON)LoadImage(wd.hInstance, MAKEINTRESOURCE(SmallMAINICON_ID), IMAGE_ICON, 128, 128, 0); // icon in the windows explorer
    wd.hCursor = LoadCursor(NULL, IDC_ARROW);                                                         // mouse
    wd.hbrBackground  = (HBRUSH)COLOR_WINDOW;                                                                   //(HBRUSH)(COLOR_WINDOW);                         // baxckground color
    wd.lpszMenuName = MAKEINTRESOURCE(Menu_ID);                                                       // set the window menu
    wd.lpszClassName = szWindowClass;                                                                 // I dont know :()

    if (!RegisterClassEx(&wd))
    {
        MessageBox(NULL, _T("I DONT KNOW SOME THING IS WRONG "), _T("ERROR"), 0);
        return 1;
    }

    wd_handle = CreateWindowEx(
        WS_EX_OVERLAPPEDWINDOW, // more styling i dont know about
        szWindowClass,          // name of program
        Title,
        WS_OVERLAPPEDWINDOW,          // type of window to create
        CW_USEDEFAULT, CW_USEDEFAULT, // x, y
        (int) WindowWidth * scale , (int) WindowHeight * scale,                // width , height
        NULL, NULL,                   // window parent  , menu
        Hinstance, NULL               // I Dont Know
    );

    if (!wd_handle)
    {
        MessageBox(NULL, _T("hwnd did not get created for some reason"), _T("error"), 0);
        return 1;
    }

    initCommControls.dwSize = sizeof(initCommControls);
    initCommControls.dwICC = ICC_WIN95_CLASSES | ICC_USEREX_CLASSES;
    InitCommonControlsEx(&initCommControls);

    ShowWindow(wd_handle, nCmdShow);
    UpdateWindow(wd_handle);



    while (GetMessage(&Msg, NULL, 0, 0) )
    { // (write to  , window handle recive from , low filter value , high filter value )
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    return (int)Msg.wParam;
}
