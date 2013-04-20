	#include <windows.h>
	#include <gl\gl.h>
	#include <gl\glu.h>
	#include <gl\glext.h>
	#include <math.h>
	#include <stdio.h>
	#include <TCHAR.h>
	
	#ifdef _UNICODE
	#undef _UNICODE
	#endif
	
	#ifdef UNICODE
	#undef UNICODE
	#endif
	
	#define bool BOOL
	#define BITS 32

	HGLRC hRC=NULL; // ������ɫ������
	HDC hDC=NULL; // ˽��GDI�豸������
	HWND hWnd=NULL; // �������ǵĴ��ھ��
	HINSTANCE hInstance; // ��������ʵ��

	GLvoid BuildFont(int size);
	GLvoid showText(int x, int y, const char* text);
	GLuint g_lists;

	bool keys[256]; // ���ڼ������̵�����
	bool active=TRUE; // ���ڵĻ��־��ȱʡΪTRUE

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 

	GLvoid ReSizeGLScene(GLsizei width, GLsizei height) { // ���ò���ʼ��GL���ڴ�С
		if (height==0) { // ��ֹ�����
			height=1; // ��Height��Ϊ1
		}
		glViewport(0, 0, width, height);  // ���õ�ǰ���ӿ�(Viewport)  
		// ���㴰�ڵ���۱���
		glMatrixMode(GL_PROJECTION); // ѡ��ͶӰ����
		glLoadIdentity(); // ����ͶӰ����
		gluOrtho2D(-width/2, width/2, -height/2, height/2);
		glMatrixMode(GL_MODELVIEW); // ѡ��ģ�͹۲����
		glLoadIdentity(); // ����ģ�͹۲����
	}

	int InitGL(GLvoid) { 
		glShadeModel(GL_SMOOTH);  // ������Ӱƽ�� 
		glClearColor(1.0, 1.0f, 1.0f, 0.0f); 

		glEnable(GL_LINE_STIPPLE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		g_lists = glGenLists(1);
		return TRUE;  // ��ʼ�� OK
	} 

	int DrawGLScene(GLvoid) { // �����￪ʼ�������еĻ���
		glClear(GL_COLOR_BUFFER_BIT );  // �����Ļ����Ȼ���

		glLineWidth(1);
		glNewList(g_lists, GL_COMPILE_AND_EXECUTE);
		glColor3f(0, 0, 1);

		showText(-110, 110, "�εδ��������ˢˢ��");
		showText(100, -110, "�¸¸�asawhaga��");
			glColor3f(1, 0, 1);
			glBegin(GL_LINES);
				glVertex2f(300, 0);
				glVertex2f(-300, 0);
			glEnd();
		glEndList();

		return TRUE; // ��������
	}  

	GLvoid KillGLWindow(GLvoid) { 

		if (hRC) { // ����ӵ����ɫ��������? 
			if (!wglMakeCurrent(NULL,NULL)) { // �����ܷ��ͷ�DC��RC������? 
				MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(hRC)) { 
				MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}
			hRC=NULL;  // ��RC��Ϊ NULL
		}

		if (hDC && !ReleaseDC(hWnd,hDC)) { // �����ܷ��ͷ� DC? 
			MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hDC=NULL;  // �� DC ��Ϊ NULL
		}

		if (hWnd && !DestroyWindow(hWnd)) { // �ܷ����ٴ���?
			MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hWnd=NULL; // �� hWnd ��Ϊ NULL
		}

		if (!UnregisterClass("OpenGL",hInstance)) { // �ܷ�ע����?
			MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hInstance=NULL; // �� hInstance ��Ϊ NULL
		}
	}

	BOOL CreateGLWindow(const char* title, const int width, const int height) {
		GLuint PixelFormat; 
		WNDCLASS wc;  // ������ṹ 

		DWORD dwExStyle; // ��չ���ڷ��
		DWORD dwStyle;  // ���ڷ��

		RECT WindowRect; // ȡ�þ��ε����ϽǺ����½ǵ�����ֵ
		WindowRect.left=(long)0; // ��Left   ��Ϊ 0
		WindowRect.right=(long)width; // ��Right  ��ΪҪ��Ŀ��
		WindowRect.top=(long)0; // ��Top    ��Ϊ 0
		WindowRect.bottom=(long)height;   // ��Bottom ��ΪҪ��ĸ߶� 

		hInstance = GetModuleHandle(NULL);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = (WNDPROC) WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "OpenGL";
		if (!RegisterClass(&wc)) { // ����ע�ᴰ����
			MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; //�˳�������FALSE
		}
		else { 
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // ��չ������
			dwStyle=WS_OVERLAPPEDWINDOW; // ������
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);  // �������ڴﵽ����Ҫ��Ĵ�С 

		if (!(hWnd=CreateWindowEx( dwExStyle,
						"OpenGL",
						title,
						WS_CLIPSIBLINGS |
						WS_CLIPCHILDREN |
						dwStyle,
						0, 0,
						WindowRect.right-WindowRect.left,
						WindowRect.bottom-WindowRect.top,
						NULL,
						NULL,
						hInstance,
						NULL))) 

		{
			KillGLWindow(); // ������ʾ��
			MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // ���� FALSE
		}

		static PIXELFORMATDESCRIPTOR pfd = { //pfd ���ߴ���������ϣ���Ķ���
			sizeof(PIXELFORMATDESCRIPTOR), //���߸�ʽ�������Ĵ�С
			1, // �汾��
			PFD_DRAW_TO_WINDOW | // ��ʽ����֧�ִ���
				PFD_SUPPORT_OPENGL | // ��ʽ����֧��OpenGL
				PFD_DOUBLEBUFFER, // ����֧��˫����

			PFD_TYPE_RGBA, // ���� RGBA ��ʽ

			BITS, // ѡ��ɫ����� 
			0, 0, 0, 0, 0, 0, // ���Ե�ɫ��λ
			0, // ��Alpha����
			0, // ����Shift Bit 
			0, // �޾ۼ�����
			0, 0, 0, 0, // ���Ծۼ�λ
			16, // 16λ Z-���� (��Ȼ���)
			0, // ��ģ�建�� 
			0, // �޸�������
			PFD_MAIN_PLANE, // ����ͼ��
			0, // ����
			0, 0, 0 // ���Բ�����
		}; 

		if (!(hDC=GetDC(hWnd))) { //ȡ���豸��������ô?
			KillGLWindow(); // ������ʾ��
			MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // ���� FALSE
		}

		if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) { // Windows �ҵ���Ӧ�����ظ�ʽ����?
			KillGLWindow(); // ������ʾ��
			MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // ���� FALSE
		}

		if(!SetPixelFormat(hDC,PixelFormat,&pfd)) { // �ܹ��������ظ�ʽô?
			KillGLWindow(); // ������ʾ��
			MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // ���� FALSE
		}

		if (!(hRC=wglCreateContext(hDC))) { // �ܷ�ȡ����ɫ������?
			KillGLWindow(); // ������ʾ��
			MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // ���� FALSE
		}

		if(!wglMakeCurrent(hDC,hRC)) { // ���Լ�����ɫ������
			KillGLWindow(); // ������ʾ��
			MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // ���� FALSE
		}

		ShowWindow(hWnd,SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
		ReSizeGLScene(width, height); 

		if (!InitGL()) { // ��ʼ���½���GL����
			KillGLWindow(); // ������ʾ��
			MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // ���� FALSE
		}

		return TRUE; // �ɹ�
	}

	GLvoid showText(int x, int y, const char* text) {
		glRasterPos2f(x, y);
		HDC hdc = wglGetCurrentDC();
		// base = glGenLists(256);
		HFONT g_font = CreateFont(
			-16,
			0,
			0,0,FW_BOLD,0,0,0,GB2312_CHARSET,OUT_TT_PRECIS,0,ANTIALIASED_QUALITY ,0, "����"
		);
		SelectObject(hDC, g_font);

		int len = 0;
		int i = 0;
		GLuint base;
		base = glGenLists(1);
		for(i=0; text[i]!='\0'; ++i) {
			if( IsDBCSLeadByte(text[i]) ) {
				wchar_t wstring;
				MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &text[i], 2, &wstring, 1);
				if (! wglUseFontBitmapsW(hDC, wstring, 1, base)) {
					wglUseFontBitmapsW(hDC, wstring, 1, base);
				}
				++i;
			} else {
				wglUseFontBitmaps(hDC, text[i], 1, base);
			}
			glCallList(base);
			// glCallList(base);
			++len;
		}
		printf("\ni = %d\n", i);
		// �����ǵ��ͷ��ڴ�
		// free(wstring);
		glDeleteLists(base, 1);
		DeleteObject(g_font);
	}

	LRESULT CALLBACK WndProc( HWND hWnd, 
			UINT uMsg, 
			WPARAM wParam, 
			LPARAM lParam) 
	{ 
		switch (uMsg) { 
			case WM_ACTIVATE: 
				{
					if (!HIWORD(wParam)) {
						active=TRUE; 
					} else {
						active=FALSE; 
					}
					return 0; 
				} 
			case WM_SYSCOMMAND:
				{
					switch (wParam) {
						case SC_SCREENSAVE: 
						case SC_MONITORPOWER: 
							return 0; 
					}
					break; 
				} 
			case WM_CLOSE:
				{
					PostQuitMessage(0); 
					return 0; 
				} 
			case WM_KEYDOWN: 
				{
					keys[wParam] = TRUE; 
					return 0; 
				} 
			case WM_KEYUP: 
				{
					keys[wParam] = FALSE; 
					return 0; 
				} 
			case WM_SIZE: 
				{
					ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  
					return 0; 
				}
		} 
		//�� DefWindowProc��������δ�������Ϣ��
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	int WINAPI WinMain( HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, 
			int nCmdShow) 
	{ 
		MSG msg; 
		BOOL done=FALSE;  

		// ����OpenGL����
		if (!CreateGLWindow("NeHe's OpenGL Framework",640,480)) {
			return 0; // ʧ���˳�
		}
		DrawGLScene();
		SwapBuffers(hDC);
		while(!done) { // ����ѭ��ֱ�� done=TRUE
			if (PeekMessage(&msg,NULL,0,0,PM_REMOVE)) { 
				if (msg.message==WM_QUIT) {
					done=TRUE;
				} else { 
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			} else { 
				if (active) {
					if (keys[VK_ESCAPE]) {
						done=TRUE;
					} else { 
						// DrawGLScene();
						// SwapBuffers(hDC);
					}
				}
				if (keys[VK_F1]) {
					keys[VK_F1]=FALSE;
					KillGLWindow();
					// �ؽ� OpenGL ����
					if (!CreateGLWindow("NeHe's OpenGL Framework",640,480)) {
						return 0;
					}
				} 
			}
		}
		// �رճ���
		KillGLWindow();
		return (msg.wParam);
	} 
