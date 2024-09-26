// CRT code to call static constructors and destructors.
//
// This function provides two "public" API's for the CRT to use:
// 
// __lilycrt_init() - Call static initalizers. Used before calling lily_main().
// __lilycrt_exit() - Call static terminators. Used when lily_main() returns.
//

#define _CRTALLOC(x) __declspec(allocate(x))

typedef int(__cdecl* _PIFV)(void);
typedef void(__cdecl* _PVFV)(void);

// Sections

#pragma section(".CRT$XIA",    long, read) // First C Initializer
#pragma section(".CRT$XIZ",    long, read) // Last C Initializer
#pragma section(".CRT$XCA",    long, read) // First C++ Initializer
#pragma section(".CRT$XCZ",    long, read) // Last C++ Initializer
#pragma section(".CRT$XPA",    long, read) // First Pre-Terminator
#pragma section(".CRT$XPZ",    long, read) // Last Pre-Terminator
#pragma section(".CRT$XTA",    long, read) // First Terminator
#pragma section(".CRT$XTZ",    long, read) // Last Terminator

#pragma section(".rdata$T",    long, read)

// all initalizers
extern "C" _CRTALLOC(".CRT$XIA") _PIFV __xi_a[] = { nullptr };
extern "C" _CRTALLOC(".CRT$XIZ") _PIFV __xi_z[] = { nullptr };
extern "C" _CRTALLOC(".CRT$XCA") _PVFV __xc_a[] = { nullptr };
extern "C" _CRTALLOC(".CRT$XCZ") _PVFV __xc_z[] = { nullptr };
extern "C" _CRTALLOC(".CRT$XPA") _PVFV __xp_a[] = { nullptr }; 
extern "C" _CRTALLOC(".CRT$XPZ") _PVFV __xp_z[] = { nullptr };
extern "C" _CRTALLOC(".CRT$XTA") _PVFV __xt_a[] = { nullptr };
extern "C" _CRTALLOC(".CRT$XTZ") _PVFV __xt_z[] = { nullptr };

#pragma comment(linker, "/merge:.CRT=.rdata")

namespace {
    static int InitTermI(_PIFV* first, _PIFV* last) {
        for (_PIFV* it = first; it != last; ++it) {
            if (*it == nullptr)
                continue;

            if (const auto res = (**it)(); res != 0)
                return res;
        }

        return 0;
    }

    static void InitTerm(_PVFV* first, _PVFV* last) {
        for (_PVFV* it = first; it != last; ++it) {
            if (*it == nullptr)
                continue;

            (**it)();
        }
    }
}

// Public APIs:

extern "C" void __lilycrt_call_init() {
    auto res = InitTermI(__xi_a, __xi_z);
	(void)res; // FIXME


    InitTerm(__xc_a, __xc_z);
}

extern "C" void __lilycrt_call_fini() {
    InitTerm(__xp_a, __xp_z);
    InitTerm(__xt_a, __xt_z);
}