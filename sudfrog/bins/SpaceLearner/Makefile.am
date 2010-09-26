METASOURCES = AUTO
bin_PROGRAMS = SpaceLearner
SpaceLearner_SOURCES = main.cpp
INCLUDES = -I$(top_srcdir)/. -I$(top_srcdir)/lib -I$(top_srcdir)/lib/newmat \
	-I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include

AM_CXXFLAGS = `pkg-config --cflags fftw3`
SpaceLearner_LDADD = $(top_builddir)/lib/xmlparser/libTMSPxmlparser.a \
	$(top_builddir)/lib/TMSPFace/libTMSPFace.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
SpaceLearner_LDFLAGS = -ljpeg -lpthread `pkg-config --libs fftw3`
