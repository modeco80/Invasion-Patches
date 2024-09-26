# Invasion-Patches

This repository contains:

- a very-WIP replacement for GVRInputRaw.dll, a DLL used for.. input, in Global VR arcade games. 
	- This is a stub, so it doesn't do anything useful. I'm going to work on dinput support or something later.
- a auto-patching launcher for Invasion Earth, which allows it to run in a window (alongside other nicities)


# Building

uhh... yeah, that's going to be a bit fun for now

basically you need to take some files from vs 2022 and put them in the following places in ~/vs2022/:

/ucrt - ucrt. put include/lib folders here
/winsdk - put platform sdk include/ and /lib here

this is messy but it works and unlike mingw actually seems to understand what not linking unnesscary code is, so /shrug

then run `just build` to build both the input dll and the launcher
