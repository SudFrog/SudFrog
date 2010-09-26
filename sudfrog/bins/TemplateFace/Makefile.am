METASOURCES = AUTO
bin_PROGRAMS = TemplateFace
TemplateFace_SOURCES = main.cpp
INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/TMSPFace \
	-I$(top_srcdir)/lib/newmat -I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
TemplateFace_LDADD = $(top_builddir)/lib/TMSPFace/libTMSPFace.a \
	$(top_builddir)/lib/xmlparser/libTMSPxmlparser.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
TemplateFace_LDFLAGS = `pkg-config --libs fftw3` -lpthread -ljpeg
AM_CXXFLAGS = `pkg-config --cflags fftw3`
