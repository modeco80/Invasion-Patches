#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "patcher.hpp"

BOOL FileExists(LPCTSTR szPath) {
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

// hard-coded, I know, but YOU do it better. 
// at least im trying here come on
auto LoadAddress2Offset(DWORD dwAddress) {
	return dwAddress - (0x004be000 - 0x400);
}

void PokeBytesImpl(HANDLE hFile, DWORD offset, const unsigned char* ptr, DWORD dwSize) {
	DWORD dwLastOffset = SetFilePointer(hFile, 0, nullptr, FILE_CURRENT);
	SetFilePointer(hFile, offset, nullptr, FILE_BEGIN);

	// poke into place. This call shouldn't fail
	WriteFile(hFile, ptr, dwSize, nullptr, nullptr);

	// Move back to the previous offset, just to be nice
	SetFilePointer(hFile, dwLastOffset, nullptr, FILE_BEGIN);
}

template <class TPrim>
void PokeBytes(HANDLE hFile, DWORD offset, const TPrim value) {
	PokeBytesImpl(hFile, offset, reinterpret_cast<const unsigned char*>(&value), sizeof(value));
}

bool InvPatch_PatchCrashes(HANDLE hPatchedExe) {
	// nop fill this
	auto crashGlOffset = LoadAddress2Offset(0x563d3a);
	for(int i = 0; i < 9; ++i) {
		PokeBytes(hPatchedExe, crashGlOffset + i, (unsigned char)0x90);
	}

	// patch out some AVI stuff. FIXME: Figure out why it crashes so I don't have to do this
	auto crashAviOffset1 = LoadAddress2Offset(0x4cee55);
	auto crashAviOffset2 = LoadAddress2Offset(0x4ced46);
	for(int i = 0; i < 5; ++i) {
		PokeBytes(hPatchedExe, crashAviOffset1 + i, (unsigned char)0x90);
		PokeBytes(hPatchedExe, crashAviOffset2 + i, (unsigned char)0x90);
	}
	return true;
}

bool InvPatch_PatchResolution(HANDLE hPatchedExe, const PatchResolutionOptions& res) {
	auto fullscreenOffset = LoadAddress2Offset(0x5154f4);
	if(res.fullscreen) {
		PokeBytes(hPatchedExe, fullscreenOffset, (unsigned char)1);
	} else {
		// FIXME: Patch the dwExStyle in CreateWindowEx() and co to add a titlebar.
		PokeBytes(hPatchedExe, fullscreenOffset, (unsigned char)0);
	}

	if(!res.IsDefaultRes()) {
		auto widthOffset = LoadAddress2Offset(0x5154fd);
		auto heightOffset = LoadAddress2Offset(0x515506);
		PokeBytes(hPatchedExe, widthOffset, short(res.width));
		PokeBytes(hPatchedExe, heightOffset, short(res.height));
	}

	return true;
}

PatchResult InvPatch_Patch(const PatchOptions& opts) {
	// First check if invasion.exe even exists
	if(!FileExists("invasion.exe"))
		return PatchResult::PatchInvasionMissing;

	// FIXME: Implement hash and nab the hash of invasion.exe,
	// so the patcher can bail out
	// .. this check could probably be done in a better way or at startup even

	// Copy it
	if(!CopyFileA("invasion.exe", "invasion-tmp.exe", FALSE))
		return PatchResult::PatchFailedGeneric;

	// Now let's begin the fun.
	HANDLE hPatchedExe = CreateFileA("invasion-tmp.exe", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

	// FIXME: specific result error please.
	if(hPatchedExe == INVALID_HANDLE_VALUE)
		return PatchResult::PatchFailedGeneric;

	// Handle patch stages.
	// We could probably even move these into another source file if they get too big

	if(!InvPatch_PatchCrashes(hPatchedExe))
		return PatchResult::PatchFailedGeneric;

	if(!InvPatch_PatchResolution(hPatchedExe, opts.res))
		return PatchResult::PatchFailedGeneric;

	// Flush and then close the file.
	FlushFileBuffers(hPatchedExe);
	CloseHandle(hPatchedExe);

	return PatchResult::PatchResultOK;
}