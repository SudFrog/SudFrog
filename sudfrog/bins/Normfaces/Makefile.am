METASOURCES = AUTO
bin_PROGRAMS = Normfaces
Normfaces_SOURCES = main.cpp
INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/newmat \
	-I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
AM_CXXFLAGS = `pkg-config --cflags fftw3`
Normfaces_LDADD = $(top_builddir)/lib/xmlparser/libTMSPxmlparser.a \
	$(top_builddir)/lib/TMSPFace/libTMSPFace.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
Normfaces_LDFLAGS = -ljpeg -lpthread
