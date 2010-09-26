#ifndef LOADFROMXML_H
#define LOADFROMXML_H 1

#include "xpath_processor.h"
#include <string>
#include <vector>

using namespace std;

struct XmlFace
{
    string filename;
    vector <float> metadata;
};

std::vector <string>  MbgcLoadXml ( char * pFilename );
vector < vector <XmlFace> >  MbgcVideoMetaXml ( char * pFilename ,char *metafile);
vector <XmlFace>  MbgcStillMetaXml ( char * pFilename,char *metafile );

vector <XmlFace>  MbgcVideoMetaXml ( char *metafile );
vector <XmlFace>  MbgcStillMetaXml ( char *metafile );

std::vector<XmlFace> loadXml(char * pFilename);
std::vector < string > loadXmlname(char *pFilename);
std::vector< vector < XmlFace > > loadLDAXml(char * pFilename);
std::vector <vector < XmlFace > > loadLDAXml(char * pFilename, char *quality);
std::vector <vector < XmlFace > > loadLDAXml(char * pFilename, int ncon,int noncon);
std::vector < string > loadnamefromXml(char *pFilename);
#endif
