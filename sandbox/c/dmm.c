#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif//WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WindowsX.h>
#include <tlhelp32.h>


// The name of the DMMViewer executable
const char* dmm_process_name = "dmmviewer.exe";
// The number of the version of DMMViewer this program is built for
const char* dmm_version = "1.1.17.2";

// Range of memory to search
// Could potentially be narrowed down
const LPCVOID memory_min = (LPCVOID)0x00000000;
const LPCVOID memory_max = (LPCVOID)0xffffffff;

// Control identifiers for the important parts of the window
const int dmm_button_parent_code = 0xE805;
const int dmm_button_first_code = 0x8023;
const int dmm_button_next_code = 0x8020;
const int dmm_button_zoom_code = 0x800A;
const int dmm_area_parent_code = 0xE900;
const int dmm_area_main_code = 0xE900;

// structs for storing .bmp header information
// char arrays to prevent padding, etc.
struct BMPHeader {
	char x[14];
};

struct BMPDIB {
	char x[54];
};

int create_bmp_header(struct BMPHeader* header, struct BMPDIB* dib, int w, int h) {
	// Create bmp headers in the given structs
	// returns the number of padding bits on each horizontal line

	// Calculate the length in bytes of one line (1 byte per color channel)
	int bytew = w * 3;
	// Each line must be aligned to 4 bytes
	if (bytew & 0x0003) {
		bytew |= 0x0003;
		++bytew;
	}

	memset(header->x, 0, sizeof(header->x));
	memset(dib->x, 0, sizeof(dib->x));
	// .bmp magic numbers
	header->x[0] = 'B';
	header->x[1] = 'M';

	// Store...
	// The size of the file
	*((int*)(&header->x[2])) = sizeof(header->x) + sizeof(dib->x) + bytew * h;
	// The bitmap offset (i.e. the size of the combined headers)
	*((int*)(&header->x[10])) = sizeof(header->x) + sizeof(dib->x);
	// The size of the DIB
	*((int*)(&dib->x[0])) = sizeof(dib->x);
	// The width of the image, in pixels
	*((int*)(&dib->x[4])) = w;
	// The height of the image, in pixels
	*((int*)(&dib->x[8])) = h;
	// The number of color planes (always 1) and the number of bits per pixel
	*((int*)(&dib->x[12])) = 1 | 24 << 16;
	// The size of the image, in bytes
	*((int*)(&dib->x[20])) = bytew * h;

	return bytew - (w * 3);
}

// Compact some stuff so it can all be sent by a pointer to get_process_window
struct process_window_helper {
	// Set this before calling get_process_window as the process to match
	DWORD pid;
	// get_process_window will set this pointing to a visible window belonging to the process
	HWND hwnd;
};

BOOL CALLBACK get_process_window(HWND hwnd, LPARAM lParam) {
	// Find all windows belonging to the process
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	if (((struct process_window_helper*)lParam)->pid == pid) {
		// Any given process may have many invisible top level windows
		// DMMViewer has about 4, but should only have one visible (the main window)
		if (IsWindowVisible(hwnd)) {
			((struct process_window_helper*)lParam)->hwnd = hwnd;
			return FALSE;
		}
	}

	return TRUE;
}

void windows_error(const LPTSTR function_name) {
	// Retrieve the system error message for the last-error code
	
	LPVOID msg_buffer;
	DWORD error_code = GetLastError();
		
	if (error_code == 0) {
		// No error available
		return;
	}
	
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error_code,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &msg_buffer,
		0, NULL);
	
	fprintf(stderr, "Error in function %s(): %lu: %s\n", function_name, error_code, (LPTSTR) &msg_buffer);
	
	LocalFree(msg_buffer);
}

int main(int argc, char** argv) {
	char* output_prefix = NULL;
	int range_min = -1;
	int range_max = -1;
	int offset = -1;
	int padwidth = -1;
	BOOL debug = FALSE;

	HANDLE system_snapshot;
	PROCESSENTRY32 process;

	int i;

	// Parse command line options
	// Partially untested, partially unimplemented
	for (i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "-p") == 0 ||
			strcmp(argv[i], "--prefix") == 0) {
				// Output file prefix
				// e.g. ./dmmrip -p ms1212_
				// results in
				// ms1212_001.bmp, ms1212_002.bmp, etc.
				// Default is no prefix
				if (output_prefix) {
					fprintf(stderr, "Multiple output prefixes specified\n");
				} else if (i + 1 >= argc) {
					fprintf(stderr, "Output prefix expected\n");
				} else {
					size_t prefix_length = strlen(argv[i + 1]);
					output_prefix = malloc(sizeof(char) * (prefix_length + 1));
					strcpy(output_prefix, argv[i + 1]);
				}
				++i;
		} else if (strcmp(argv[i], "-r") == 0 ||
			strcmp(argv[i], "--range") == 0) {
				// Page range (inclusive, one indexed)
				// e.g. ./dmmrip -r 1 10
				// will rip the first ten pages
				// Default is entire file
				if (range_min >= 0 && range_max >= 0) {
					fprintf(stderr, "Multiple ranges specified\n");
				} else if (i + 2 >= argc) {
					fprintf(stderr, "Two range values expected\n");
				} else {
					printf("Range is not yet supported\n");
					range_min = atoi(argv[i + 1]);
					range_max = atoi(argv[i + 2]);
					if (range_min < 0 || range_max < range_min) {
						fprintf(stderr, "Invalid range specified\n");
						range_min = range_max = -1;
					}
				}
				i += 2;
		} else if (strcmp(argv[i], "-o") == 0 ||
			strcmp(argv[i], "--offset") == 0) {
				// Naming offset
				// e.g. ./dmmrip -p file -r 11 20 -o 1
				// will start at page 11 but name the image file001.bmp
				// Default is the first page in the range
				if (offset >= 0) {
					fprintf(stderr, "Multiple offsets specified\n");
				} else if (i + 1 >= argc) {
					fprintf(stderr, "Offset expected\n");
				} else {
					offset = atoi(argv[i + 1]);
					if (offset < 0) {
						fprintf(stderr, "Invalid offset\n");
						offset = -1;
					}
				}
				++i;
		} else if (strcmp(argv[i], "-w") == 0 ||
			strcmp(argv[i], "--width") == 0) {
				// File number minimum width
				// e.g. ./dmmrip -w 5
				// will name files 00001.bmp, 00002.bmp, etc.
				// Default is 3
				if (padwidth > 0) {
					fprintf(stderr, "Multiple widths specified\n");
				} else if (i + 1 >= argc) {
					fprintf(stderr, "Width expected\n");
				} else {
					padwidth = atoi(argv[i + 1]);
					if (padwidth < 0) {
						fprintf(stderr, "Invalid width\n");
						padwidth = -1;
					}
				}
				++i;
		} else if (strcmp(argv[i], "-d") == 0 ||
			strcmp(argv[i], "--debug") == 0) {
				// Enables debugging text
				if (debug) {
					fprintf(stderr, "Multiple debug declarations\n");
				} else {
					debug = TRUE;
				}
		}
	}

	if (output_prefix == NULL) {
		output_prefix = "";
	}
	if (range_min < 0) {
		range_min = 1;
	}
	if (range_max < 0) {
		range_max = -1;
	}
	if (offset < 0) {
		offset = range_min;
	}
	if (padwidth < 0) {
		padwidth = 3;
	}

	// Main program

	// Get a snapshot of all the running processes
	system_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	process.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(system_snapshot, &process) == TRUE) {
		// Iterate through all the processes
		while (Process32Next(system_snapshot, &process) == TRUE) {
			// Find the DMM Viewer process by name
			if (_stricmp(process.szExeFile, dmm_process_name) == 0) {
				// Open the process with permission to read memory and process info
				HANDLE dmm_process = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, process.th32ProcessID);
				struct process_window_helper pwh;

				SYSTEM_INFO system_info;
				MEMORY_BASIC_INFORMATION mbi;
				DWORD page_granularity;

				HWND dmm_window_main;
				HWND dmm_button_parent;
				HWND dmm_button_first, dmm_button_next;
				HWND dmm_button_zoom;
				HWND dmm_area_parent;
				HWND dmm_area_main;

				int page;
				BOOL done = FALSE;

				RECT original_window_rect;
				WINDOWPLACEMENT original_window_placement;
				WINDOWPLACEMENT temp_window_placement;

				if (dmm_process == NULL) {
					fprintf(stderr, "Could not open process %s\n", process.szExeFile);
					windows_error("OpenProcess");
					exit(EXIT_FAILURE);
				}

				pwh.pid = process.th32ProcessID;
				pwh.hwnd = (HWND)0xdeaddead;
				EnumWindows(&get_process_window, (LPARAM)&pwh);

				// Find all the subwindow by control ID
				dmm_window_main = pwh.hwnd;
				dmm_button_parent = GetDlgItem(dmm_window_main, dmm_button_parent_code);
				dmm_button_first = GetDlgItem(dmm_button_parent, dmm_button_first_code);
				dmm_button_next = GetDlgItem(dmm_button_parent, dmm_button_next_code);
				dmm_button_zoom = GetDlgItem(dmm_button_parent, dmm_button_zoom_code);
				dmm_area_parent = GetDlgItem(dmm_window_main, dmm_area_parent_code);
				dmm_area_main = GetDlgItem(dmm_area_parent, dmm_area_main_code);

				if (debug) {
					printf("dmm_window_main: %p\n", dmm_window_main);
					printf("dmm_button_parent: %p\n", dmm_button_parent);
					printf("dmm_button_first: %p\n", dmm_button_first);
					printf("dmm_button_next: %p\n", dmm_button_next);
					printf("dmm_button_zoom: %p\n", dmm_button_zoom);
					printf("dmm_area_main: %p\n", dmm_area_main);
				}

				// Store original window position and state
				// Window will be shrunken to ensure window size is smaller than the image
				// This is necessary so that the immage in memory will not be padded
				original_window_placement.length = sizeof(original_window_placement);
				GetWindowPlacement(dmm_window_main, &original_window_placement);
				temp_window_placement = original_window_placement;
				temp_window_placement.showCmd = SW_SHOW;
				SetWindowPlacement(dmm_window_main, &temp_window_placement);

				GetWindowRect(dmm_window_main, &original_window_rect);
				
				SetWindowPos(dmm_window_main, 0, 0, 0, 256, 256, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

				GetSystemInfo(&system_info);

				// Find the size of a page, so we only have to check memory on each page boundary
				page_granularity = system_info.dwAllocationGranularity;

				// Send a virtual click to the first page button
				SendMessage(dmm_button_first, BM_CLICK, 0, 0);

				// Continue until the next button becomes disabled, indicating the last image
				for (page = 0; !done; ++page) {
					LPVOID address = (LPVOID)memory_min;
					LPVOID image_address = address;
					DWORD image_size = 0;
					SCROLLINFO sih, siv;
					int w, h;
					int* image;

					int number_length;
					char* output_name;
					FILE* output_file;

					struct BMPHeader header;
					struct BMPDIB dib;

					int buffer_width;

					int i;

					// If the next button is disabled, we reached the end; grab one last image and finish
					done = GetWindowLong(dmm_button_next, GWL_STYLE) & WS_DISABLED;

					// In my testing, the image is always found in the largest memory block allocated
					// by the process. This isn't necessarily going to always be true, so this will likely
					// fail on small images. Fortunately, comic pages tend to be rather large.
					// We can further narrow the possibilities down by looking at other characteristics of the memory
					// The memory type is always Private and set to Read/Write protection
					while (VirtualQueryEx(dmm_process, address, &mbi, sizeof(mbi))) {
						if (mbi.State == MEM_COMMIT && mbi.Type == MEM_PRIVATE && mbi.Protect == PAGE_READWRITE && mbi.RegionSize > image_size) {
							image_address = mbi.BaseAddress;
							image_size = mbi.RegionSize;
						}
						address = (char*)mbi.BaseAddress + mbi.RegionSize;
					}

					if (debug) {
						printf("Most likely image location at %p (%lu bytes)\n", image_address, image_size);
					}

					// As a hack, the dimensions of the images are determined by
					// looking at the size of the scroll area
					// This requires that the image area be smaller than the image
					sih.cbSize = sizeof(sih);
					sih.fMask = SIF_ALL;
					siv.cbSize = sizeof(siv);
					siv.fMask = SIF_ALL;

					GetScrollInfo(dmm_area_main, SB_HORZ, &sih);
					GetScrollInfo(dmm_area_main, SB_VERT, &siv);

					w = sih.nMax - sih.nMin + 1;
					h = siv.nMax - siv.nMin + 1;

					if (debug) {
						printf("%i x %i pixels\n", w, h);
					}

					// Allocate memory to copy image into
					image = malloc(sizeof(int) * w * h);
					// Read the image memory (image is stored in 4 byte pixels, 0xrrggbbFF)
					if (ReadProcessMemory(dmm_process, image_address, (LPVOID)image, w * h * 4, NULL) == 0) {
						if (debug) {	
							printf("Image uses %i of %lu allocated bytes\n", w * h * 4, image_size);
						}
						printf("On page %i:\n", page);
						windows_error("ReadProcessMemory");
						continue;
					}
					// Once image memory is ours, 'click' to the next image
					SendMessage(dmm_button_next, BM_CLICK, 0, 0);
					number_length = (int)log10((double)page + offset);
					number_length = number_length > padwidth ? number_length : padwidth;
					output_name = malloc(sizeof(char) * (strlen(output_prefix) + number_length + strlen(".bmp") + 1));

					sprintf(output_name, "%s%0*i.bmp", output_prefix, padwidth, page + offset);

					output_file = fopen(output_name, "wb+");

					if (!output_file) {
						fprintf(stderr, "Failed to create file '%s'\n", output_name);

						continue;
					}
					
					buffer_width = create_bmp_header(&header, &dib, w, h);
					fwrite(header.x, sizeof(header.x), 1, output_file);
					fwrite(dib.x, sizeof(dib.x), 1, output_file);

					// .bmps are mirrored vertically
					for (i = h - 1; i >= 0; --i) {
						int j;
						for (j = 0; j < w; ++j) {
							fwrite(((char*)(image + i * w + j)), 3, 1, output_file);
						}
						// Write any padding bytes
						for (j = 0; j < buffer_width; ++j) {
							fputc(0xff, output_file);
						}
					}

					fclose(output_file);

					free(image);
				}

				// Restore window size and state
				SetWindowPos(dmm_window_main, 0, 0, 0, original_window_rect.right - original_window_rect.left, original_window_rect.top - original_window_rect.bottom, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
				SetWindowPlacement(dmm_window_main, &original_window_placement);

				CloseHandle(dmm_process);

				break;
			}
		}
	} else {
		windows_error("Process32First");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
