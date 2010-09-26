METASOURCES = AUTO
INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/newmat \
	-I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
AM_CXXFLAGS = `pkg-config --cflags fftw3`
bin_PROGRAMS = mbgc_run
mbgc_run_SOURCES = main.cpp
mbgc_run_LDADD = $(top_builddir)/lib/TMSPFace/libTMSPFace.a \
	$(top_builddir)/lib/xmlparser/libTMSPxmlparser.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
mbgc_run_LDFLAGS = `pkg-config --libs fftw3` -ljpeg -lpthread
