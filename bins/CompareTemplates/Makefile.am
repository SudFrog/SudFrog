INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/TMSPFace \
	-I$(top_srcdir)/lib/newmat -I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
METASOURCES = AUTO
bin_PROGRAMS = CompareTemplate
CompareTemplate_SOURCES = main.cpp
CompareTemplate_LDADD = $(top_builddir)/lib/TMSPFace/libTMSPFace.a \
	$(top_builddir)/lib/xmlparser/libTMSPxmlparser.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
CompareTemplate_LDFLAGS = `pkg-config --libs fftw3` -ljpeg
AM_CXXFLAGS = `pkg-config --cflags fftw3`
