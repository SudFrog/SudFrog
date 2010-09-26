#include "jpegutilities.h"

// Error handling:
struct my_error_mgr {
  jpeg_error_mgr pub;                                     /* "public" fields */
  jmp_buf setjmp_buffer;                                  /* for return to caller */
};

typedef my_error_mgr* my_error_ptr;


/* Error exit handler */
#define ERREXIT(msg)  (exit(0))

/* Read one byte, testing for EOF */
int read_1_byte ( FILE * myfile )
{
  int c;

  c = getc ( myfile );
  if ( c == EOF )
    ERREXIT ( "Premature EOF in JPEG file" );
  return c;
}

/* Read 2 bytes, convert to unsigned int */
/* All 2-byte quantities in JPEG markers are MSB first */
unsigned int read_2_bytes ( FILE * myfile )
{
  int c1, c2;

  c1 =getc ( myfile );
  if ( c1 == EOF )
    ERREXIT ( "Premature EOF in JPEG file" );
  c2 = getc ( myfile );
  if ( c2 == EOF )
    ERREXIT ( "Premature EOF in JPEG file" );
  return ( ( ( unsigned int ) c1 ) << 8 ) + ( ( unsigned int ) c2 );
}

/*
 * The main function.
 */

int get_exif_orient ( const char *filename )
{
  FILE * myfile;		/* My JPEG file */
  unsigned char exif_data[65536L];

  int n_flag, set_flag;
  unsigned int length, i;
  int is_motorola; /* Flag for byte order */
  unsigned int offset, number_of_tags, tagnum;

  n_flag = 0; set_flag = 0;

  if ( ( myfile = fopen ( filename, "rb" ) ) == NULL ) {
    fprintf ( stderr, "can't open %s\n", filename );
    return 0;
  }

  /* Read File head, check for JPEG SOI + Exif APP1 */
  for ( i = 0; i < 4; i++ )
    exif_data[i] = ( unsigned char ) read_1_byte ( myfile );
  if ( exif_data[0] != 0xFF ||
       exif_data[1] != 0xD8 ||
       exif_data[2] != 0xFF ||
       exif_data[3] != 0xE1 ) {
    fclose ( myfile );
    return 0;
  }

  /* Get the marker parameter length count */
  length = read_2_bytes ( myfile );
  /* Length includes itself, so must be at least 2 */
  /* Following Exif data length must be at least 6 */
  if ( length < 8 ) {
    fclose ( myfile );
    return 0;
  }
  length -= 8;
  /* Read Exif head, check for "Exif" */
  for ( i = 0; i < 6; i++ )
    exif_data[i] = ( unsigned char ) read_1_byte ( myfile );
  if ( exif_data[0] != 0x45 ||
       exif_data[1] != 0x78 ||
       exif_data[2] != 0x69 ||
       exif_data[3] != 0x66 ||
       exif_data[4] != 0 ||
       exif_data[5] != 0 ) {
    fclose ( myfile );
    return 0;
  }
  /* Read Exif body */
  for ( i = 0; i < length; i++ )
    exif_data[i] = ( unsigned char ) read_1_byte ( myfile );

  if ( length < 12 ) {
    fclose ( myfile );
    return 0; /* Length of an IFD entry */
  }
  /* Discover byte order */
  if ( exif_data[0] == 0x49 && exif_data[1] == 0x49 )
    is_motorola = 0;
  else if ( exif_data[0] == 0x4D && exif_data[1] == 0x4D )
    is_motorola = 1;
  else {
    fclose ( myfile );
    return 0;
  }
  /* Check Tag Mark */
  if ( is_motorola ) {
    if ( exif_data[2] != 0 ) {fclose ( myfile ); return 0;}
    if ( exif_data[3] != 0x2A ) {fclose ( myfile ); return 0;}
  } else {
    if ( exif_data[3] != 0 ) {fclose ( myfile ); return 0;}
    if ( exif_data[2] != 0x2A ) {fclose ( myfile ); return 0;}
  }

  /* Get first IFD offset (offset to IFD0) */
  if ( is_motorola ) {
    if ( exif_data[4] != 0 ) {fclose ( myfile ); return 0;}
    if ( exif_data[5] != 0 ) {fclose ( myfile ); return 0;}
    offset = exif_data[6];
    offset <<= 8;
    offset += exif_data[7];
  } else {
    if ( exif_data[7] != 0 ) {fclose ( myfile ); return 0;}
    if ( exif_data[6] != 0 ) {fclose ( myfile ); return 0;}
    offset = exif_data[5];
    offset <<= 8;
    offset += exif_data[4];
  }
  if ( offset > length - 2 ) {fclose ( myfile ); return 0;}
  ; /* check end of data segment */

  /* Get the number of directory entries contained in this IFD */
  if ( is_motorola ) {
    number_of_tags = exif_data[offset];
    number_of_tags <<= 8;
    number_of_tags += exif_data[offset+1];
  } else {
    number_of_tags = exif_data[offset+1];
    number_of_tags <<= 8;
    number_of_tags += exif_data[offset];
  }
  if ( number_of_tags == 0 ) {fclose ( myfile ); return 0;}
  offset += 2;

  /* Search for Orientation Tag in IFD0 */
  for ( ;; ) {
    if ( offset > length - 12 ) {fclose ( myfile ); return 0;} /* check end of data segment */
    /* Get Tag number */
    if ( is_motorola ) {
      tagnum = exif_data[offset];
      tagnum <<= 8;
      tagnum += exif_data[offset+1];
    } else {
      tagnum = exif_data[offset+1];
      tagnum <<= 8;
      tagnum += exif_data[offset];
    }
    if ( tagnum == 0x0112 ) break; /* found Orientation Tag */
    if ( --number_of_tags == 0 ) {fclose ( myfile ); return 0;}
    offset += 12;
  }

  /* Get the Orientation value */
  if ( is_motorola ) {
    if ( exif_data[offset+8] != 0 ) {fclose ( myfile ); return 0;}
    set_flag = exif_data[offset+9];
  } else {
    if ( exif_data[offset+9] != 0 ) {fclose ( myfile ); return 0;}
    set_flag = exif_data[offset+8];
  }
  if ( set_flag > 8 ) {fclose ( myfile ); return 0;}

  fclose ( myfile );
  /* Write out Orientation value */
  return set_flag;
  /* All done. */

}


void  my_error_exit ( j_common_ptr cinfo )
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = ( my_error_ptr ) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  ( *cinfo->err->output_message ) ( cinfo );

  /* Return control to the setjmp point */
  longjmp ( myerr->setjmp_buffer, 1 );
}



int GetJpegImage ( const char* strFilename,unsigned char *&ImageColor,int& width,int& height,int& colornum,int &orient )
{
//double *pImage;
  jpeg_decompress_struct cinfo;
  my_error_mgr jerr;
  FILE* pFile;
  JSAMPARRAY buffer;
  int row_stride;
  orient=get_exif_orient ( strFilename );

  if ( ( pFile = fopen ( strFilename, "rb" ) ) == NULL ) {
    fprintf ( stderr, "Unable to open input file '%s'\n",
              strFilename );
    return -1;
  }


// Setup error handling:
  cinfo.err = jpeg_std_error ( &jerr.pub );
  jerr.pub.error_exit = my_error_exit;
  if ( setjmp ( jerr.setjmp_buffer ) ) {
    jpeg_destroy_decompress ( &cinfo );
    fclose ( pFile );
    return -1;
  }
// Create JPEG decompression object:
  jpeg_create_decompress ( &cinfo );

// Specify data source:
  jpeg_stdio_src ( &cinfo, pFile );

// Read header:
  jpeg_read_header ( &cinfo, true );

// Start decompressor:
  jpeg_start_decompress ( &cinfo );
  width=cinfo.output_width;
  height=cinfo.output_height;

// Allocate local uncompressed version of image:

  row_stride = cinfo.output_width * cinfo.output_components;
//printf( "%d\n",cinfo.output_components);
  colornum=cinfo.output_components;
// Make a one-row-high sample array:
  buffer = ( *cinfo.mem->alloc_sarray )
           ( ( j_common_ptr ) &cinfo, JPOOL_IMAGE, row_stride, 1 );

  ImageColor = new unsigned char[height * row_stride];
// Read scan lines:
  while ( cinfo.output_scanline < cinfo.output_height ) {
    jpeg_read_scanlines ( &cinfo, buffer, 1 );
    int k = cinfo.output_components* ( cinfo.output_scanline-1 ) *cinfo.output_width;
    for ( int i=0; i<row_stride; i+=cinfo.output_components ) {
/////////////////asbaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
      for ( int j=0;j<cinfo.output_components;j++ )
        ImageColor[k+i+j] = buffer[0][i+j];
    }

  }


  jpeg_destroy_decompress ( &cinfo );


//   if ( orient !=0 && orient !=1 ) {
//     if ( orient==8 ) {
//       printf ( "Orientation %d detected in the jpeg header of image %s : image will be rotated to normal\n",orient,strFilename );
//       unsigned char *tmpbuff = new unsigned char[height *width* colornum];
//       memcpy ( tmpbuff,ImageColor,height * row_stride );
//
//       height=cinfo.output_width;
//       width=cinfo.output_height;
//       int k=0;
//       for ( int i=0;i<width;i++ )
//         for ( int j=0;j<height;j++ ) {
//           for ( int c=0;c<cinfo.output_components;c++ )
//             ImageColor[colornum*((height-j-1)*width+i)+c] = tmpbuff[colornum*(i*height+j)+c];
//         }
//
//       delete tmpbuff;
//     } else {
//       printf ( "Orientation detected in the jpeg header of image %s : %d rotation not handled for this orientation\n",strFilename,orient );
//       fclose ( pFile );
//       return -1;
//     }
//
//   }

  fclose ( pFile );
  return 0;
//return 0;
}
