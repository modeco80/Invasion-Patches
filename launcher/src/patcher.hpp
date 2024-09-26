#pragma once

struct PatchResolutionOptions {
	bool fullscreen;
	unsigned int width;
	unsigned int height;

	/// Checks if the resolution is the default specified in the game's code.
	/// If so, then we do not need to actually patch the resolution.
	bool IsDefaultRes() const {
		return width == 640 && height == 480;
	}
};

struct PatchOptions {
	PatchResolutionOptions res;
};

enum class PatchResult {
	PatchResultOK,
	PatchInvasionMissing, //< Invasion binary doesn't exist
	PatchNotInvasion, //< isn't Invasion: Earth binary
	PatchFailedGeneric
};

// patches ./Invasion.exe statically, outputs to ./Invasion-tmp.exe
PatchResult InvPatch_Patch(const PatchOptions& opts);