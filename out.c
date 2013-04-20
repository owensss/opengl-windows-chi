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

	HGLRC hRC=NULL; // 永久着色描述表
	HDC hDC=NULL; // 私有GDI设备描述表
	HWND hWnd=NULL; // 保存我们的窗口句柄
	HINSTANCE hInstance; // 保存程序的实例

	GLvoid BuildFont(int size);
	GLvoid showText(int x, int y, const char* text);
	GLuint g_lists;

	bool keys[256]; // 用于键盘例程的数组
	bool active=TRUE; // 窗口的活动标志，缺省为TRUE

	LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); 

	GLvoid ReSizeGLScene(GLsizei width, GLsizei height) { // 重置并初始化GL窗口大小
		if (height==0) { // 防止被零除
			height=1; // 将Height设为1
		}
		glViewport(0, 0, width, height);  // 重置当前的视口(Viewport)  
		// 计算窗口的外观比例
		glMatrixMode(GL_PROJECTION); // 选择投影矩阵
		glLoadIdentity(); // 重置投影矩阵
		gluOrtho2D(-width/2, width/2, -height/2, height/2);
		glMatrixMode(GL_MODELVIEW); // 选择模型观察矩阵
		glLoadIdentity(); // 重置模型观察矩阵
	}

	int InitGL(GLvoid) { 
		glShadeModel(GL_SMOOTH);  // 启用阴影平滑 
		glClearColor(1.0, 1.0f, 1.0f, 0.0f); 

		glEnable(GL_LINE_STIPPLE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_COLOR_ARRAY);
		g_lists = glGenLists(1);
		return TRUE;  // 初始化 OK
	} 

	int DrawGLScene(GLvoid) { // 从这里开始进行所有的绘制
		glClear(GL_COLOR_BUFFER_BIT );  // 清除屏幕和深度缓存

		glLineWidth(1);
		glNewList(g_lists, GL_COMPILE_AND_EXECUTE);
		glColor3f(0, 0, 1);

		showText(-110, 110, "滴滴答答呱呱呱刷刷怕");
		showText(100, -110, "嘎嘎嘎asawhaga了");
			glColor3f(1, 0, 1);
			glBegin(GL_LINES);
				glVertex2f(300, 0);
				glVertex2f(-300, 0);
			glEnd();
		glEndList();

		return TRUE; // 继续运行
	}  

	GLvoid KillGLWindow(GLvoid) { 

		if (hRC) { // 我们拥有着色描述表吗? 
			if (!wglMakeCurrent(NULL,NULL)) { // 我们能否释放DC和RC描述表? 
				MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}

			if (!wglDeleteContext(hRC)) { 
				MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			}
			hRC=NULL;  // 将RC设为 NULL
		}

		if (hDC && !ReleaseDC(hWnd,hDC)) { // 我们能否释放 DC? 
			MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hDC=NULL;  // 将 DC 设为 NULL
		}

		if (hWnd && !DestroyWindow(hWnd)) { // 能否销毁窗口?
			MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hWnd=NULL; // 将 hWnd 设为 NULL
		}

		if (!UnregisterClass("OpenGL",hInstance)) { // 能否注销类?
			MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
			hInstance=NULL; // 将 hInstance 设为 NULL
		}
	}

	BOOL CreateGLWindow(const char* title, const int width, const int height) {
		GLuint PixelFormat; 
		WNDCLASS wc;  // 窗口类结构 

		DWORD dwExStyle; // 扩展窗口风格
		DWORD dwStyle;  // 窗口风格

		RECT WindowRect; // 取得矩形的左上角和右下角的坐标值
		WindowRect.left=(long)0; // 将Left   设为 0
		WindowRect.right=(long)width; // 将Right  设为要求的宽度
		WindowRect.top=(long)0; // 将Top    设为 0
		WindowRect.bottom=(long)height;   // 将Bottom 设为要求的高度 

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
		if (!RegisterClass(&wc)) { // 尝试注册窗口类
			MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; //退出并返回FALSE
		}
		else { 
			dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // 扩展窗体风格
			dwStyle=WS_OVERLAPPEDWINDOW; // 窗体风格
		}

		AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);  // 调整窗口达到真正要求的大小 

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
			KillGLWindow(); // 重置显示区
			MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // 返回 FALSE
		}

		static PIXELFORMATDESCRIPTOR pfd = { //pfd 告诉窗口我们所希望的东东
			sizeof(PIXELFORMATDESCRIPTOR), //上诉格式描述符的大小
			1, // 版本号
			PFD_DRAW_TO_WINDOW | // 格式必须支持窗口
				PFD_SUPPORT_OPENGL | // 格式必须支持OpenGL
				PFD_DOUBLEBUFFER, // 必须支持双缓冲

			PFD_TYPE_RGBA, // 申请 RGBA 格式

			BITS, // 选定色彩深度 
			0, 0, 0, 0, 0, 0, // 忽略的色彩位
			0, // 无Alpha缓存
			0, // 忽略Shift Bit 
			0, // 无聚集缓存
			0, 0, 0, 0, // 忽略聚集位
			16, // 16位 Z-缓存 (深度缓存)
			0, // 无模板缓存 
			0, // 无辅助缓存
			PFD_MAIN_PLANE, // 主绘图层
			0, // 保留
			0, 0, 0 // 忽略层遮罩
		}; 

		if (!(hDC=GetDC(hWnd))) { //取得设备描述表了么?
			KillGLWindow(); // 重置显示区
			MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // 返回 FALSE
		}

		if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd))) { // Windows 找到相应的象素格式了吗?
			KillGLWindow(); // 重置显示区
			MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // 返回 FALSE
		}

		if(!SetPixelFormat(hDC,PixelFormat,&pfd)) { // 能够设置象素格式么?
			KillGLWindow(); // 重置显示区
			MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // 返回 FALSE
		}

		if (!(hRC=wglCreateContext(hDC))) { // 能否取得着色描述表?
			KillGLWindow(); // 重置显示区
			MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // 返回 FALSE
		}

		if(!wglMakeCurrent(hDC,hRC)) { // 尝试激活着色描述表
			KillGLWindow(); // 重置显示区
			MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // 返回 FALSE
		}

		ShowWindow(hWnd,SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);
		ReSizeGLScene(width, height); 

		if (!InitGL()) { // 初始化新建的GL窗口
			KillGLWindow(); // 重置显示区
			MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
			return FALSE; // 返回 FALSE
		}

		return TRUE; // 成功
	}

	GLvoid showText(int x, int y, const char* text) {
		glRasterPos2f(x, y);
		HDC hdc = wglGetCurrentDC();
		// base = glGenLists(256);
		HFONT g_font = CreateFont(
			-16,
			0,
			0,0,FW_BOLD,0,0,0,GB2312_CHARSET,OUT_TT_PRECIS,0,ANTIALIASED_QUALITY ,0, "楷体"
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
		// 用完后记得释放内存
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
		//向 DefWindowProc传递所有未处理的消息。
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	int WINAPI WinMain( HINSTANCE hInstance,
			HINSTANCE hPrevInstance,
			LPSTR lpCmdLine, 
			int nCmdShow) 
	{ 
		MSG msg; 
		BOOL done=FALSE;  

		// 创建OpenGL窗口
		if (!CreateGLWindow("NeHe's OpenGL Framework",640,480)) {
			return 0; // 失败退出
		}
		DrawGLScene();
		SwapBuffers(hDC);
		while(!done) { // 保持循环直到 done=TRUE
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
					// 重建 OpenGL 窗口
					if (!CreateGLWindow("NeHe's OpenGL Framework",640,480)) {
						return 0;
					}
				} 
			}
		}
		// 关闭程序
		KillGLWindow();
		return (msg.wParam);
	} 
