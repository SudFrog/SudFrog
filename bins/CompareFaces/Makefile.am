INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/TMSPFace \
	-I$(top_srcdir)/lib/newmat -I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
METASOURCES = AUTO
bin_PROGRAMS = CompareFaces
CompareFaces_SOURCES = main.cpp
CompareFaces_LDFLAGS = `pkg-config --libs fftw3` -ljpeg
CompareFaces_LDADD = $(top_builddir)/lib/TMSPFace/libTMSPFace.a \
	$(top_builddir)/lib/xmlparser/libTMSPxmlparser.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
AM_CXXFLAGS = `pkg-config --cflags fftw3`
