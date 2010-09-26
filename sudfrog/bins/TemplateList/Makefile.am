INCLUDES = -I$(top_srcdir)/lib -I$(top_srcdir)/lib/TMSPFace \
	-I$(top_srcdir)/lib/newmat -I$(top_srcdir)/lib/xmlparser -I$(top_srcdir)/lib/include
METASOURCES = AUTO
bin_PROGRAMS = TemplateList
TemplateList_SOURCES = main.cpp
TemplateList_LDADD = $(top_builddir)/lib/TMSPFace/libTMSPFace.a \
	$(top_builddir)/lib/xmlparser/libTMSPxmlparser.a $(top_builddir)/lib/newmat/libTMSPnewmat.a
TemplateList_LDFLAGS = `pkg-config --libs fftw3` -lpthread -ljpeg
AM_CXXFLAGS = `pkg-config --cflags fftw3`
