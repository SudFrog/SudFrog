METASOURCES = AUTO
AM_CXXFLAGS = `pkg-config --cflags fftw3`
INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/lib -I$(top_srcdir)/lib/newmat \
	-I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
noinst_LIBRARIES = libTMSPFace.a
libTMSPFace_a_SOURCES = Gross.cpp TMSP_image.cpp comparator.cpp gaborate.cpp \
	inputs.cpp jpegutilities.cpp kernel.cpp linearreducer.cpp mask.cpp \
	miscellaneous.cpp nonlinearreducer.cpp pfcface.cpp templator.cpp timer.cpp verbose.cpp
