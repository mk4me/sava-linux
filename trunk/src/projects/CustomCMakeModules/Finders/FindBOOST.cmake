# przygotowanie do szukania
FIND_INIT(BOOST boost)

# funkcja wykrywaj¹ce wersjê BOOST
function(BOOST_FIND_VERSION path)    
	# inicjalizacja
	set(BOOST_VERSION "Unknown" CACHE STRING "Unknown version")
	# próba odczytania wersji z pliku
	if (EXISTS ${path})
		file(READ "${path}" _boost_Version_contents)
		string(REGEX REPLACE ".*#define.*BOOST_LIB_VERSION.*([0-9]+_[0-9]+).*"
            "\\1" _boost_version ${_boost_Version_contents})
		if (BOOST_VERSION STREQUAL "Unknown")
			set(BOOST_VERSION "${_boost_version}" CACHE STRING "The version of BOOST_VERSION${suffix} which was detected" FORCE)
		endif()
    endif()
	# czy siê uda³o?
	if (BOOST_VERSION STREQUAL "Unknown")
		message("Unknown version of BOOST_VERSION. File ${path} could not be read. This may result in further errors.")
	endif()
endfunction(BOOST_FIND_VERSION)

# wykrycie wersji boost
BOOST_FIND_VERSION("${BOOST_INCLUDE_DIR}/boost/version.hpp")

set(boost_ver "-${BOOST_VERSION}")

#ustawianie tego parametry ze wzgledu na platforme i generator	
set(BOOST_CMPL_inner "")

if(MSVC)
	string(REGEX REPLACE ".*Visual Studio ([0-9]+[0-9]+).*"
            "\\1" loc_boost_cmpl ${CMAKE_GENERATOR})			
	set(BOOST_CMPL "-vc${loc_boost_cmpl}0" CACHE STRING "Visual Studio Target Toolset version")	
	set(BOOST_CMPL_inner ${BOOST_CMPL})
endif()

# szukanie
FIND_SHARED(BOOST_SYSTEM "boost_system<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_system<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_FILESYSTEM "boost_filesystem<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_filesystem<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_PROGRAM_OPTIONS "boost_program_options<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_program_options<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_SERIALIZATION "boost_serialization<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_serialization<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_DATE_TIME "boost_date_time<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_date_time<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_TIMER "boost_timer<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_timer<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_CHRONO "boost_chrono<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_chrono<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_THREAD "boost_thread<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_thread<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
FIND_SHARED(BOOST_REGEX "boost_regex<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_regex<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
#FIND_SHARED(BOOST_PYTHON "boost_python<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>" "boost_python<${BOOST_CMPL_inner},?><-mt,?><-gd,?><${boost_ver},?>")
# koniec
FIND_FINISH(BOOST)

# Wy³¹czamy automatyczne linkowanie boosta
set(BOOST_COMPILER_DEFINITIONS BOOST_ALL_NO_LIB BOOST_PROGRAM_OPTIONS_DYN_LINK)
