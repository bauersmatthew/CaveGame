
#include "platform_win32.h"

platform_api platform_build_api() {

	platform_api ret;

	ret.platform_create_window			= &platform_create_window;
	ret.platform_destroy_window			= &platform_destroy_window;
	ret.platform_swap_buffers			= &platform_swap_buffers;
	ret.platform_process_messages		= &platform_process_messages;
	ret.platform_wait_message			= &platform_wait_message;
	ret.platform_load_library			= &platform_load_library;
	ret.platform_free_library			= &platform_free_library;
	ret.platform_get_proc_address 		= &platform_get_proc_address;
	ret.platform_get_file_attributes 	= &platform_get_file_attributes;
	ret.platform_test_file_written		= &platform_test_file_written;
	ret.platform_copy_file				= &platform_copy_file;
	ret.platform_heap_alloc				= &platform_heap_alloc;
	ret.platform_heap_free				= &platform_heap_free;
	ret.platform_get_bin_path			= &platform_get_bin_path;
	ret.platform_create_thread			= &platform_create_thread;
	ret.platform_this_thread_id			= &platform_this_thread_id;
	ret.platform_terminate_thread		= &platform_terminate_thread;
	ret.platform_exit_this_thread		= &platform_exit_this_thread;
	ret.platform_thread_sleep			= &platform_thread_sleep;

	return ret;
}

void platform_thread_sleep(i32 ms) {
	Sleep(ms);
}

platform_error platform_terminate_thread(platform_thread* thread, i32 exit_code) {

	platform_error ret;

	if(TerminateThread(thread->handle, (DWORD)exit_code) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;		
	}

	return ret;
}

void platform_exit_this_thread(i32 exit_code) {
	
	ExitThread((DWORD)exit_code);
}

platform_thread_id platform_this_thread_id() {

	platform_thread_id ret;

	ret.id = GetCurrentThreadId();

	return ret;
}

platform_error platform_create_thread(platform_thread* thread, i32 (*proc)(void*), void* param, bool start_suspended) {

	platform_error ret;

	thread->handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)proc, param, start_suspended ? CREATE_SUSPENDED : 0, &thread->id.id);

	if(thread->handle == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;			
	}

	return ret;
}

platform_error platform_get_bin_path(string* path) {

	platform_error ret;

	*path = make_string(MAX_PATH, &platform_heap_alloc);

	DWORD len = GetModuleFileNameA(NULL, (LPSTR)path->c_str, MAX_PATH);

	if(len == 0) {
		ret.good = false;
		ret.error = GetLastError();

		free_string(*path, &platform_heap_free);

		return ret;
	}

	if(path->c_str[len - 1] == '\0') {
		ret.good = false;
		ret.error = GetLastError();

		free_string(*path, &platform_heap_free);

		return ret;
	}

	path->len = len;

	return ret;
}

void* platform_heap_alloc(u64 bytes) {

	HANDLE heap = GetProcessHeap();
	return HeapAlloc(heap, HEAP_ZERO_MEMORY, bytes);
}

void platform_heap_free(void* mem) {

	HANDLE heap = GetProcessHeap();
	HeapFree(heap, 0, mem);
}

platform_error platform_copy_file(string source, string dest, bool overwrite) {

	platform_error ret;

	if(CopyFile(source.c_str, dest.c_str, !overwrite) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;			
	}

	return ret;
}

bool platform_test_file_written(platform_file_attributes* first, platform_file_attributes* second) {

	return CompareFileTime(&first->attrib.ftLastWriteTime, &second->attrib.ftLastWriteTime) == -1;
}

platform_error platform_get_file_attributes(platform_file_attributes* attrib, string file_path) {

	platform_error ret;

	if(GetFileAttributesExA(file_path.c_str, GetFileExInfoStandard, (LPVOID)&attrib->attrib) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;				
	}

	return ret;
}

platform_error platform_get_proc_address(void** address, platform_dll* dll, string name) {

	platform_error ret;

	*address = GetProcAddress(dll->dll_handle, name.c_str);

	if(*address == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;				
	}

	return ret;
}

platform_error platform_load_library(platform_dll* dll, string file_path) {

	platform_error ret;

	dll->dll_handle = LoadLibraryA(file_path.c_str);

	if(dll->dll_handle == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;				
	}

	return ret;
}

platform_error platform_free_library(platform_dll* dll) {
	
	platform_error ret;

	if(FreeLibrary(dll->dll_handle) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;				
	}

	return ret;
}

platform_error platform_wait_message() {

	platform_error ret;

	if(WaitMessage() == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;		
	}

	return ret;
}

platform_error platform_swap_buffers(platform_window* window) {

	platform_error ret;

	if(SwapBuffers(window->device_context) == FALSE) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;		
	}

	return ret;
}

bool platform_process_messages(platform_window* window) {
	
	if(!global_platform_running) {
		return false;
	}

	MSG msg;

	while(PeekMessageA(&msg, window->handle, 0, 0, PM_REMOVE) != 0) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	return true;
}

LRESULT CALLBACK window_proc(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam) {
	
	if(msg == WM_DESTROY || msg == WM_CLOSE || msg == WM_QUIT) {
		
		global_platform_running = false;
		return 0;
	}

	return DefWindowProcA(handle, msg, wParam, lParam);
}

platform_error platform_create_window(platform_window* window, string title, u32 width, u32 height) {

	platform_error ret;

	window->title = make_copy_string(title, &platform_heap_alloc);
	window->width = width;
	window->height = height;

	HINSTANCE instance = GetModuleHandleA(NULL);

	if(instance == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;
	}

	window->window_class = {
		sizeof(WNDCLASSEX),
		CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
		(WNDPROC)window_proc,
		0, 0,
		instance,
		0, 0, 0, 0,
		window->title.c_str, 0
	};

	if(RegisterClassExA(&window->window_class) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;
	}

	window->handle = CreateWindowExA(WS_EX_ACCEPTFILES, window->title.c_str, window->title.c_str, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
			           				 CW_USEDEFAULT, CW_USEDEFAULT, window->width, window->height, 0, 0,
			           				 instance, 0);

	if(window->handle == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;
	}

	window->device_context = GetDC(window->handle);

	if(window->device_context == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;
	}

	window->pixel_format = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1, 
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 
		32, 8, 0, 8, 0, 8, 0, 8, 0,
		0, 0, 0, 0,
		24, 8, 0, 0, 0, 0, 0
	};

	int pixel_index = ChoosePixelFormat(window->device_context, &window->pixel_format);

	if(pixel_index == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;
	}

	if(SetPixelFormat(window->device_context, pixel_index, 0) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;		
	}

	window->gl_temp = wglCreateContext(window->device_context);

	if(window->gl_temp == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;				
	}

	if(wglMakeCurrent(window->device_context, window->gl_temp) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;		
	}

	i32 attribs[9] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
    	WGL_CONTEXT_MINOR_VERSION_ARB, 5,
    	WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    	WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    	0
	};

	wglCreateContextAttribsARB = (wgl_create_context_attribs_arb*)wglGetProcAddress("wglCreateContextAttribsARB");

	if(wglCreateContextAttribsARB == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;		
	}

	window->gl_context = wglCreateContextAttribsARB(window->device_context, 0, &attribs[0]);

	if(window->gl_context == NULL) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;	
	}

	if(wglMakeCurrent(window->device_context, window->gl_context) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;	
	}

	return ret;
}

platform_error platform_destroy_window(platform_window* window) {

	platform_error ret;

	free_string(window->title, &platform_heap_free);

	if(wglDeleteContext(window->gl_temp) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;	
	}
	if(wglDeleteContext(window->gl_context) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;	
	}
	if(DestroyWindow(window->handle) == 0) {
		ret.good = false;
		ret.error = GetLastError();
		return ret;	
	}
	return ret;
}