BEGIN_PROJECT("executable")


SET_PRIVATE_HEADERS(
	Compression.h
	DefaultPacker.h
	#GpuPacker.h
	IPacker.h
)


SET_SOURCE_FILES(
	Compression.cpp
	DefaultPacker.cpp
	#GpuPacker.cpp
	main.cpp
)

END_PROJECT()