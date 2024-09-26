
// func defs
extern "C" __cdecl int _snprintf(char* buffer, unsigned int size, const char* format, ...);

namespace util {

	// use va-list version @ some point. for now this works
	template<class ...Args>
	void OutputDebugF(char* const pszFormat, Args&&... args) {
		char buf[512]{};
		
		int res = _snprintf(&buf[0], sizeof(buf)-1, pszFormat, args...);

		//if(res > 0)
		//	return;
		
		OutputDebugString(&buf[0]);
	}
}