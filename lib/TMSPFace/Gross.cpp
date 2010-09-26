#include "TMSP_image.h"

namespace TMSP_Face_Space
{
// Calculate the Minimum of a vector
  float min ( float *tab,int taille )
  {
    float min=tab[0];
    for ( int i=1;i<taille;++i ) {
      if ( tab[i]<min ) {
        min=tab[i];
      }
    }
    return min;
  }


// Calculate the Maximum of a vector
  float max ( float *tab,int taille )
  {
    float max=tab[0];
    for ( int i=1;i<taille;++i ) {
      if ( tab[i]>max ) {
        max=tab[i];
      }
    }
    return max;
  }


  bool IsEven ( int i,int j,int width )
  {
    if ( ( i*width+j ) %2 == 0 )
      return true;
    else
      return false;
  }

  int ASNorm ( Pfc_Image &InputImage,int steps, float lambda, Pfc_Image &ReflectImage, Pfc_Image &LightImage )
  {

    float *TmpImage,*LImage,minval,maxval;
    int width=InputImage.GetImWidth();
    int height=InputImage.GetImHeight();
    int size=width*height;


    TmpImage = new float[size];
    LImage = new float[size];


    for ( int i=0;i<size;i++ )
      TmpImage[i]= ( float ) InputImage.GetData ( i );


    memcpy ( LImage,TmpImage, size*sizeof ( float ) );


    for ( int i=0; i<steps; i++ ) {
      gaussSeidel ( LImage, TmpImage, height, width, lambda );
      memcpy ( TmpImage,LImage, size*sizeof ( float ) );
    }


    minval=min ( TmpImage,size );
    maxval=max ( TmpImage,size );


    for ( int i=0;i<size;i++ )
      LightImage.SetData ( i, ( uint8_t ) ( 255* ( TmpImage[i]-minval ) / ( maxval-minval ) ) );

    for ( int i = 0; i < size; i++ )
      if ( Between ( TmpImage[i], 0.0, 1 ) )
        LImage[i] = 0;
      else
        LImage[i]= ( float ) ( InputImage.GetData ( i ) /TmpImage[i] );

    minval=min ( LImage,size );
    maxval=max ( LImage,size );

    for ( int i = 0; i < size; i++ )
      ReflectImage.SetData ( i, ( uint8_t ) ( 255* ( LImage[i]-minval ) / ( maxval-minval ) ) );


    //Free memory

    delete []TmpImage;
    delete []LImage;

    return 0;
  }


  int ASNorm ( Pfc_Image &InputImage,int steps, float lambda, Pfc_Image &ReflectImage, Pfc_Image &LightImage, Mask ROI, int meanref,float stdref )
  {

    float *TmpImage,*LImage,minval,maxval;
    int width=InputImage.GetImWidth();
    int height=InputImage.GetImHeight();
    int size=width*height;


    TmpImage = new float[size];
    LImage = new float[size];


    for ( int i=0;i<size;i++ )
      TmpImage[i]= ( float ) InputImage.GetData ( i );


    memcpy ( LImage,TmpImage, size*sizeof ( float ) );


    for ( int i=0; i<steps; i++ ) {
      gaussSeidel ( LImage, TmpImage, height, width, lambda );
      memcpy ( TmpImage,LImage, size*sizeof ( float ) );
    }


    minval=min ( TmpImage,size );
    maxval=max ( TmpImage,size );


    for ( int i=0;i<size;i++ )
      LightImage.SetData ( i, ( uint8_t ) ( 255* ( TmpImage[i]-minval ) / ( maxval-minval ) ) );

    for ( int i = 0; i < size; i++ )
      if ( Between ( TmpImage[i], 0.0, 1 ) )
        LImage[i] = 0;
      else
        LImage[i]= ( float ) ( InputImage.GetData ( i ) /TmpImage[i] );

    /*minval=min ( LImage,size );
    maxval=max ( LImage,size );*/

    float mean=0;
    float std=0;
    int count=0;
    for ( int i=0; i < width; ++i )
      for ( int j=0;j<height;j++ ) {
        if ( ROI.Ismask ( j+1,i-2 ) ) {
          mean+=LImage[count];
          std+=LImage[count]*LImage[count];
        }
        count++;
      }

    mean=mean/count;
    std=sqrt ( ( std/count )-mean*mean );

    count=0;
    for ( int i=0; i < width; ++i )
      for ( int j=0;j<height;j++ ) {
        if ( ROI.Ismask ( j+1,i-2 ) ) {
          float val= ( meanref + stdref* ( LImage[count]-mean ) /std );
          if ( val <0 )
            ReflectImage.SetData ( count,0 );
          else
            if ( val >255 )
              ReflectImage.SetData ( count,255 );
            else
              ReflectImage.SetData ( count, ( uint8_t ) val );

        } else
          ReflectImage.SetData ( count,0 );
        count++;
      }


    //Free memory

    delete []TmpImage;
    delete []LImage;

    return 0;
  }


  void gaussSeidel ( float *Result, float *Source, int InputHeight,int InputWidth, float lambda )
  {
    int indic, indicup, indicdown, indicleft, indicright;
    float rho[5];
    float up, down, left, right, center;
    int width=InputWidth;
    int height=InputHeight;

    for ( int i = 1; i <= height; i++ )
      for ( int j = 1; j <= width; j++ ) {
//compute odds points
        indic= ( j-1 ) + ( i-1 ) *width;
        indicup= ( j-1 ) + ( i-2 ) *width;
        indicdown= ( j-1 ) +i*width;
        indicleft= ( j-2 ) + ( i-1 ) *width;
        indicright=j+ ( i-1 ) *width;
        if ( i>1 && i<height && j>1 && j<width ) {
          if ( IsEven ( i,j,width ) ) {
            ComputeRho ( rho, Source, height, width, i, j );
            up = -lambda*rho[UP]*Result[indicup];
            down = -lambda*rho[DOWN]*Result[indicdown];
            left = -lambda*rho[LEFT]*Result[indicleft];
            right = -lambda*rho[RIGHT]*Result[indicright];
            center = 1 + lambda*rho[UP] + lambda*rho[DOWN] + lambda*rho[LEFT] + lambda*rho[RIGHT];
            Result[indic] = Source[indic]/center - ( up + down + left + right ) /center;
          }
        } else
          Result[indic]=0.0;
      }

//compute even points
    for ( int i = 1; i <= height; i++ )
      for ( int j = 1; j <= width; j++ ) {
        indic= ( j-1 ) + ( i-1 ) *width;
        indicup= ( j-1 ) + ( i-2 ) *width;
        indicdown= ( j-1 ) +i*width;
        indicleft= ( j-2 ) + ( i-1 ) *width;
        indicright=j+ ( i-1 ) *width;
        if ( i>1 && i<height && j>1 && j<width ) {
          if ( !IsEven ( i,j,width ) ) {
            ComputeRho ( rho, Source, height, width, i, j );
            up = -lambda*rho[UP]*Result[indicup];
            down = -lambda*rho[DOWN]*Result[indicdown];
            left = -lambda*rho[LEFT]*Result[indicleft];
            right = -lambda*rho[RIGHT]*Result[indicright];
            center = 1 + lambda*rho[UP] + lambda*rho[DOWN] + lambda*rho[LEFT] + lambda*rho[RIGHT];
            Result[indic] = ( Source[indic]/center - ( up + down + left + right ) /center );
          }
        } else
          Result[indic]=0.0;
      }

  }


//
  void ComputeRho ( float *rho, float *image, int InputHeight,int InputWidth, int posx, int posy )
  {

// initial contrast values set to zero, updated if we can compute it ...
    bool up = true;
    bool down = true;
    bool left = true;
    bool right = true;

    int height=InputHeight;
    int width=InputWidth;

    if ( posx==1 )
      up=false;

    if ( posx==height )
      down=false;

    if ( posy==1 )
      left=false;

    if ( posy==width )
      right=false;

    for ( int i=0; i<5; i++ )
      rho[i]= 0;

    if ( up )
      weber ( rho, image, height, width, posx, posy, UP );

    if ( down )
      weber ( rho, image, height, width, posx,posy, DOWN );

    if ( left )
      weber ( rho, image, height, width, posx,posy, LEFT );

    if ( right )
      weber ( rho, image, height, width, posx,posy, RIGHT );

    rho[CENTER] = rho[UP] + rho[DOWN] + rho[LEFT] + rho[RIGHT];

  }


//
  void weber ( float *rho, float *image, int InputHeight,int InputWidth, int posx, int posy, int position )
  {

    float center,up,diff,minim,right,left,down;
    int indic = ( posy-1 ) + ( posx-1 ) *InputWidth;
    int indicup = ( posy-1 ) + ( posx-2 ) *InputWidth;
    int indicdown = ( posy-1 ) + ( posx ) *InputWidth;
    int indicleft = ( posy-2 ) + ( posx-1 ) *InputWidth;
    int indicright = ( posy ) + ( posx-1 ) *InputWidth;

    switch ( position ) {
      case UP:
        center = image[indic];
        up = image[indicup];
        diff = fabs ( center - up );
        minim = 0.0;

        if ( up > center )
          minim = center;
        else
          minim = up;

        if ( Between ( diff, 0, 1 ) )
          rho[UP] = minim;
        else
          rho[UP] = minim/diff;

        break;

      case DOWN:
        center = image[indic];
        down = image[indicdown];
        diff = fabs ( center - down );
        minim = 0.0;

        if ( down > center )
          minim = center;
        else
          minim = down;

        if ( Between ( diff, 0, 1 ) )
          rho[DOWN] = minim;
        else
          rho[DOWN] = minim/diff;
        break;

      case LEFT:
        center = image[indic];
        left = image[indicleft];
        diff = fabs ( center - left );
        minim = 0.0;

        if ( left > center )
          minim = center;
        else
          minim = left;

        if ( Between ( diff, 0, 1 ) )
          rho[LEFT] = minim;
        else
          rho[LEFT] = minim/diff;

        break;

      case RIGHT:
        center = image[indic];
        right = image[indicright];
        diff = fabs ( center - right );
        minim = 0.0;

        if ( right > center )
          minim = center;
        else
          minim = right;

        if ( Between ( diff, 0, 1 ) )
          rho[RIGHT] = minim;
        else
          rho[RIGHT] = minim/diff;
        break;
    }
  }
}
