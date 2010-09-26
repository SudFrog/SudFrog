METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/newmat \
	-I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
AM_CXXFLAGS = `pkg-config --cflags fftw3`
bin_PROGRAMS = frgc_run
frgc_run_SOURCES = main.cpp
frgc_run_LDFLAGS = `pkg-config --libs fftw3` -ljpeg -lpthread
frgc_run_LDADD = $(top_builddir)/lib/TMSPFace/libTMSPFace.a \
	$(top_builddir)/lib/xmlparser/libTMSPxmlparser.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
