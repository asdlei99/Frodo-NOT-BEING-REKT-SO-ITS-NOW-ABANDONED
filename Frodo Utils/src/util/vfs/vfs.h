#pragma once

#pragma warning(disable : 4251)

#include <Windows.h>
#include <fdu.h>
#include <util/string.h>
#include <util/list.h>
#include <util/map.h>

#define FD_VFS_MAX_MOUNT_POINTS 128

namespace FD {

class FDUAPI VFS {
private:
	static VFS* instance;

public:

	static inline void Init() { VFS::instance = new VFS(); }
	static inline void Dispose() { delete VFS::instance; }

	static __forceinline VFS* Get() { return VFS::instance; }

private:
	Map<String, String> mountPoints;


public:
	VFS();
	~VFS();

	void Mount(const String& name, const String& path);
	void UnMount(const String& name);
	byte* ReadFile(const String& filename, uint_t* fileSize = nullptr);
	String ReadTextFile(const String& filename);

	String ResolvePath(const String& vpath);

};

}