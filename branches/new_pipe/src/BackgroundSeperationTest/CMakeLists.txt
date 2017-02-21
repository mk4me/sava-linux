BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
    regressionTest.h
    timingTest.h
)

SET_SOURCE_FILES(
	main.cpp
    regressionTest.cpp
    timingTest.cpp
)

END_PROJECT()
