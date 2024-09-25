# gvrinput

This is a very-WIP replacement for GVRInputRaw.dll, a DLL used for.. input, in Global VR arcade games. 

Right now this builds a stub which implements the input API, but doesn't actually do anything useful.

# Building

uhh... yeah, that's going to be a bit fun for now

basically you need to take some files from vs 2022 and put them in the following places in ~/vs2022/:

/ucrt - ucrt. put include/lib folders here
/winsdk - put platform sdk include/ and /lib here

this is messy but it works and unlike mingw actually seems to understand what not linking unnesscary code is, so /shrug

then run ./makedll.sh. I promise I'll put in a makefile or something much better once this turns into something useful...
