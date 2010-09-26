#include "loadfromxml.h"
#include <iostream>

#define CLEARLINE

string getvideoname ( string str,string ext )
{

  size_t found;
  found=str.find_last_of ( "/\\" );
  string filen=str.substr ( found+1 );
//found=filen.find_last_of("/\\");
  found=filen.find_last_of ( ext );
  return filen.substr ( 0,found-ext.length() +1 );
}

/*vector < vector <XmlFace> >  MbgcVideoMetaXml ( char * pFilename ,char *metafile )
{
  vector < vector <XmlFace> > list;
  vector <XmlFace> Videofiles;
  TiXmlDocument * XDp_doc,* XDp_meta;
  TiXmlElement * XEp_main,* XEp_meta;
  TiXmlNode * XEp_Image;
  string filen;
  XmlFace FaceSub;
  char Request[255];

  XDp_doc = new TiXmlDocument;
  if ( ! XDp_doc -> LoadFile ( pFilename ) ) {
    cerr << "Can't find "<< pFilename << "\n";
    return list;
  }

  XDp_meta = new TiXmlDocument;
  if ( ! XDp_meta -> LoadFile ( metafile ) ) {
    cerr << "Can't find "<< metafile << "\n";
    return list;
  }

  XEp_main=XDp_doc->RootElement ();
  XEp_meta=XDp_meta->RootElement ();

  TinyXPath::xpath_processor xp_proc ( XEp_main, "//presentation" );
  int u_nb = xp_proc.u_compute_xpath_node_set ();
  cout << u_nb << " Video found" << endl;
  cout << "Starting parsing the corresponding images ! it could take a while!!!" << endl;
  int prec=cout.precision();
  int w=cout.width();
  cout.precision ( 2 );
  cout.fill ( ' ' );

  for ( int i=0;i<u_nb;i++ ) {
    XEp_Image=xp_proc.XNp_get_xpath_node ( i );
    TiXmlElement *Xelement=XEp_Image->ToElement();
    filen=getvideoname ( Xelement->Attribute ( "file-name" ),".ts" );
    sprintf ( Request,"//video[@name=\"%s\"]/image[@use=\"yes\"]",filen.c_str() );
    TinyXPath::xpath_processor xp_video ( XEp_meta, Request );
    int vid=xp_video.u_compute_xpath_node_set();
    cout.width ( 50 );
    cout << "\rparsed " << (i+1) << "/" << u_nb ;
    cout.flush();

    Videofiles.clear();
    for ( int j=0;j<vid;j++ ) {
      XEp_Image=xp_video.XNp_get_xpath_node ( j );
      TiXmlElement *Xelement=XEp_Image->ToElement();
      string imagename=Xelement->Attribute ( "name" );
      if ( imagename!="" ) {
        //printf ( "\t%s\n",imagename.c_str()  );
        FaceSub.metadata.clear();
        FaceSub.filename=imagename;
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xnose" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ynose" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xmouth" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ymouth" ) ) );
        Videofiles.push_back ( FaceSub );
      }
    }
    list.push_back ( Videofiles );
  }
  cout.width ( w );
  cout.precision ( prec );
  cout << endl;
  return list;
}*/

vector < vector <XmlFace> >  MbgcVideoMetaXml ( char * pFilename ,char *metafile )
{
  vector < vector <XmlFace> > list;
  vector <XmlFace> Videofiles;
  TiXmlDocument * XDp_doc,* XDp_meta;
  TiXmlElement * XEp_main,* XEp_meta;
  TiXmlNode * XEp_Image;
  string filen;
  XmlFace FaceSub;
  char Request[255];

  XDp_doc = new TiXmlDocument;
  if ( ! XDp_doc -> LoadFile ( pFilename ) ) {
    cerr << "Can't find "<< pFilename << "\n";
    return list;
  }

  XDp_meta = new TiXmlDocument;
  if ( ! XDp_meta -> LoadFile ( metafile ) ) {
    cerr << "Can't find "<< metafile << "\n";
    return list;
  }

  XEp_main=XDp_doc->RootElement ();
  XEp_meta=XDp_meta->RootElement ();

  TinyXPath::xpath_processor xp_proc ( XEp_main, "//presentation" );
  int u_nb = xp_proc.u_compute_xpath_node_set ();
  cout << u_nb << " Video found" << endl;
  cout << "Starting parsing the corresponding images ! it could take a while!!!" << endl;
  int prec=cout.precision();
  int w=cout.width();
  cout.precision ( 2 );
  cout.fill ( ' ' );
  TiXmlElement *FirstPerson = XEp_meta->FirstChildElement();
  for ( int i=0;i<u_nb;i++ ) {
    XEp_Image=xp_proc.XNp_get_xpath_node ( i );
    TiXmlElement *Xelement=XEp_Image->ToElement();
    filen=getvideoname ( Xelement->Attribute ( "file-name" ),".ts" );
    sprintf ( Request,"//video[@name=\"%s\"]/image[@use=\"yes\"]",filen.c_str() );

    TinyXPath::xpath_processor xp_video ( FirstPerson, Request );

    int vid=xp_video.u_compute_xpath_node_set();
    cout.width ( 50 );
    cout << "\rparsed " << ( i+1 ) << "/" << u_nb ;
    cout.flush();

    Videofiles.clear();
    for ( int j=0;j<vid;j++ ) {
      XEp_Image=xp_video.XNp_get_xpath_node ( j );
      TiXmlElement *Xelement=XEp_Image->ToElement();
      string imagename=Xelement->Attribute ( "name" );
      if ( imagename!="" ) {
        //printf ( "\t%s\n",imagename.c_str()  );
        FaceSub.metadata.clear();
        FaceSub.filename=imagename;
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xnose" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ynose" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xmouth" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ymouth" ) ) );
        Videofiles.push_back ( FaceSub );
      }
    }
    list.push_back ( Videofiles );
    FirstPerson=FirstPerson->NextSiblingElement();
  }
  cout.width ( w );
  cout.precision ( prec );
  cout << endl;
  return list;
}


vector <XmlFace>  MbgcStillMetaXml ( char * pFilename,char *metafile )
{
  vector <XmlFace> list;
  TiXmlDocument * XDp_doc,* XDp_meta;
  TiXmlElement * XEp_main,* XEp_meta;
  TiXmlNode * XEp_Image;
  string filen;
  XmlFace FaceSub;
  char Request[255];

  XDp_doc = new TiXmlDocument;
  if ( ! XDp_doc -> LoadFile ( pFilename ) ) {
    cerr << "Can't find "<< pFilename << "\n";
    return list;
  }

  XDp_meta = new TiXmlDocument;
  if ( ! XDp_meta -> LoadFile ( metafile ) ) {
    cerr << "Can't find "<< metafile << "\n";
    return list;
  }

  XEp_main=XDp_doc->RootElement ();
  XEp_meta=XDp_meta->RootElement ();

  TinyXPath::xpath_processor xp_proc ( XEp_main, "//presentation" );
  int u_nb = xp_proc.u_compute_xpath_node_set ();

  int prec=cout.precision();
  int w=cout.width();
  cout.precision ( 2 );
  cout.fill ( ' ' );
  for ( int i=0;i<u_nb;i++ ) {
    XEp_Image=xp_proc.XNp_get_xpath_node ( i );
    TiXmlElement *Xelement=XEp_Image->ToElement();
    filen=Xelement->Attribute ( "file-name" );
    sprintf ( Request,"//image[@name=\"%s\" and @use=\"yes\"]",filen.c_str() );

//     cout.width ( 50 );
//     cout << "\rparsing " << 100.0*i/u_nb  << "%";

    cout.width ( 50 );
    cout << "\rparsed " << ( i+1 ) << "/" << u_nb ;
    cout.flush();
    TinyXPath::xpath_processor xp_video ( XEp_meta, Request );
    int vid=xp_video.u_compute_xpath_node_set();
    if ( vid>0 ) {
      XEp_Image=xp_video.XNp_get_xpath_node ( 0 );
      Xelement=XEp_Image->ToElement();
      string imagename=Xelement->Attribute ( "name" );
      if ( imagename!="" ) {
        //printf ( "\t%s\n",imagename.c_str()  );
        FaceSub.metadata.clear();
        FaceSub.filename=imagename;
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xnose" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ynose" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xmouth" ) ) );
        FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ymouth" ) ) );
        list.push_back ( FaceSub );
      }
    }
  }
  cout.width ( w );
  cout.precision ( prec );
  cout << endl;
  return list;
}

vector <XmlFace>  MbgcVideoMetaXml ( char *metafile )
{
  vector <XmlFace> list;
  TiXmlDocument * XDp_meta;
  TiXmlElement * XEp_meta;
  TiXmlNode * XEp_Image;
  string filen;
  XmlFace FaceSub;
  char Request[255];


  XDp_meta = new TiXmlDocument;
  if ( ! XDp_meta -> LoadFile ( metafile ) ) {
    cerr << "Can't find "<< metafile << "\n";
    return list;
  }

  XEp_meta=XDp_meta->RootElement ();

  sprintf ( Request,"//image[@use=\"yes\"]" );
  TinyXPath::xpath_processor xp_video ( XEp_meta, Request );
  int vid=xp_video.u_compute_xpath_node_set();
  for ( int j=0;j<vid;j++ ) {
    XEp_Image=xp_video.XNp_get_xpath_node ( j );
    string imagename=XEp_Image->ToElement()->Attribute ( "name" );
    if ( imagename!="" ) {
      FaceSub.metadata.clear();
      FaceSub.filename=imagename;
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "XleftEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "YleftEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "XrightEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "YrightEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Xnose" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Ynose" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Xmouth" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Ymouth" ) ) );
      list.push_back ( FaceSub );
      //break;
    }
  }
  //}

  return list;
}

vector <XmlFace>  MbgcStillMetaXml ( char *metafile )
{
  vector <XmlFace> list;
  TiXmlDocument * XDp_meta;
  TiXmlElement* XEp_meta;
  TiXmlNode * XEp_Image;
  string filen;
  XmlFace FaceSub;
  char Request[255];


  XDp_meta = new TiXmlDocument;
  if ( ! XDp_meta -> LoadFile ( metafile ) ) {
    cerr << "Can't find "<< metafile << "\n";
    return list;
  }

  XEp_meta=XDp_meta->RootElement ();

  sprintf ( Request,"//image[@use=\"yes\"]" );
  TinyXPath::xpath_processor xp_video ( XEp_meta, Request );
  int vid=xp_video.u_compute_xpath_node_set();
  for ( int j=0;j<vid;j++ ) {
    XEp_Image=xp_video.XNp_get_xpath_node ( j );
    //TiXmlElement*Xelement=XEp_Image->ToElement();
    string imagename=XEp_Image->ToElement()->Attribute ( "name" );
    if ( imagename!="" ) {
      //printf ( "\t%s\n",imagename.c_str()  );
      FaceSub.metadata.clear();
      FaceSub.filename=imagename;
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "XleftEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "YleftEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "XrightEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "YrightEye" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Xnose" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Ynose" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Xmouth" ) ) );
      FaceSub.metadata.push_back ( atof ( XEp_Image->ToElement()->Attribute ( "Ymouth" ) ) );
      list.push_back ( FaceSub );
      //break;
    }
  }
  //}

  return list;
}

vector<XmlFace> loadXml ( char * pFilename )
{

  TiXmlDocument * XDp_doc;
  TiXmlElement * XEp_main;
  TiXmlNode * XEp_Image;
//  TiXmlElement *Xelement;
  const char *imagename;

  vector < XmlFace > Userfiles;
  XmlFace FaceSub;

  XDp_doc = new TiXmlDocument;
  if ( ! XDp_doc -> LoadFile ( pFilename ) ) {
    cerr << "Can't find "<< pFilename << "\n";
    return Userfiles;
  }

  XEp_main=XDp_doc->RootElement ();
  TinyXPath::xpath_processor xp_proc ( XEp_main, "//image" );
  int u_nb = xp_proc.u_compute_xpath_node_set ();

  for ( int i=0;i<u_nb;i++ ) {

    XEp_Image=xp_proc.XNp_get_xpath_node ( i );
    TiXmlElement *Xelement=XEp_Image->ToElement();
    imagename=Xelement->Attribute ( "name" );
    if ( imagename!="" ) {
      FaceSub.metadata.clear();
      FaceSub.filename=imagename;
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XleftEye" ) ) );
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YleftEye" ) ) );
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "XrightEye" ) ) );
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "YrightEye" ) ) );
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xnose" ) ) );
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ynose" ) ) );
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Xmouth" ) ) );
      FaceSub.metadata.push_back ( atof ( Xelement->Attribute ( "Ymouth" ) ) );
      Userfiles.push_back ( FaceSub );
    }

  }

  return Userfiles;
}

vector <string>  loadnamefromXml ( char * pFilename )
{
  vector <string> list;
  TiXmlDocument * XDp_doc;
  TiXmlElement * XEp_main;
  TiXmlNode * XEp_Image;

//	TiXmlElement *image;
//	char Request[255];
  XDp_doc = new TiXmlDocument;
  if ( ! XDp_doc -> LoadFile ( pFilename ) ) {
    cerr << "Can't find "<< pFilename << "\n";
    return list;
  }

  XEp_main=XDp_doc->RootElement ();

  TinyXPath::xpath_processor xp_proc ( XEp_main, "//image" );
  int u_nb = xp_proc.u_compute_xpath_node_set ();

  for ( int i=0;i<u_nb;i++ ) {
    XEp_Image=xp_proc.XNp_get_xpath_node ( i );
    TiXmlElement *Xelement=XEp_Image->ToElement();
    list.push_back ( Xelement->Attribute ( "name" ) );
  }

  return list;
}

vector <vector < XmlFace > > loadLDAXml ( char * pFilename, int ncon,int noncon )
{

  vector <vector < XmlFace > > ListFilesbyUser;

  TiXmlDocument * XDp_doc;
  TiXmlElement * XEp_main;
  TiXmlNode * XEp_User;
//    TiXmlNode * XEp_Image;
  TiXmlElement *image;
  char Request[255];
  const char *imagename;

  vector < XmlFace > Userfiles;
  XmlFace FaceSub;

  XDp_doc = new TiXmlDocument;
  if ( ! XDp_doc -> LoadFile ( pFilename ) ) {
    cerr << "Can't find "<< pFilename << "\n";
    return ListFilesbyUser;
  }

  XEp_main=XDp_doc->RootElement ();

  TinyXPath::xpath_processor xp_proc ( XEp_main, "//user" );

// build the list of all element nodes in the tree
  int u_nb = xp_proc . u_compute_xpath_node_set ();
//  printf ("%d\n", u_nb);
  imagename=new char[255];
  for ( int i=0;i<u_nb;i++ ) {
    XEp_User=xp_proc.XNp_get_xpath_node ( i );
    TiXmlElement *Xelement=XEp_User->ToElement();
    int id=atoi ( Xelement->Attribute ( "id" ) );

    sprintf ( Request,"//user[@id=%d]/image[@type=\"controlled\"]",id );
//         printf("%s\n",Request);
    TinyXPath::xpath_processor xp_node ( XEp_User, Request );
    int im_nb=xp_node.u_compute_xpath_node_set();
//   printf ("user %d number of images %d\n", id,im_nb);
    Userfiles.clear();                                  // clear for evry subject
    if ( im_nb >ncon )
      im_nb=ncon;

    for ( int j=0;j<im_nb;j++ ) {
      image=xp_node.XNp_get_xpath_node ( j )->ToElement();;
//=XEp_Image->
      imagename=image->Attribute ( "name" );
      if ( imagename!="" ) {
        FaceSub.metadata.clear();
        FaceSub.filename=imagename;
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "XleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "YleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "XrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "YrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Xnose" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Ynose" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Xmouth" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Ymouth" ) ) );
        Userfiles.push_back ( FaceSub );
      }

    }

    sprintf ( Request,"//user[@id=%d]/image[@type=\"noncontrolled\"]",id );
//         printf("%s\n",Request);
    TinyXPath::xpath_processor xp_node1 ( XEp_User, Request );
    im_nb=xp_node1.u_compute_xpath_node_set();
//   printf ("user %d number of images %d\n", id,im_nb);
    if ( im_nb >noncon )
      im_nb=noncon;
    //  Userfiles.clear();                                  // clear for evry subject
    for ( int j=0;j<im_nb;j++ ) {
      image=xp_node1.XNp_get_xpath_node ( j )->ToElement();;
//=XEp_Image->
      imagename=image->Attribute ( "name" );
      if ( imagename!="" ) {
        FaceSub.metadata.clear();
        FaceSub.filename=imagename;
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "XleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "YleftEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "XrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "YrightEye" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Xnose" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Ynose" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Xmouth" ) ) );
        FaceSub.metadata.push_back ( atof ( image->Attribute ( "Ymouth" ) ) );
        Userfiles.push_back ( FaceSub );
      }

    }

    ListFilesbyUser.push_back ( Userfiles );

  }

  return ListFilesbyUser;
}
