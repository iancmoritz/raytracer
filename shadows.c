// Ian Moritz, PD 5, 2/10/17
//
//
#include <stdio.h>
#include <math.h>
//
#define W 640
#define H 480
#define fileSpheres 0 //
#define codeSpheres 4 // 
#define frameCount 120
//
//
typedef struct
{
   double x ;
   double y ;
   double z ;
   //
} Vector ;

typedef struct
{
   Vector start;
   Vector end;
   //
} Ray ;
//
typedef struct
{
   int r;
   int g;
   int b;
   //
} Color ;
//
typedef struct
{
   Vector c;
   double r;
   Color h;
   //
} Sphere ;
//
Vector g = { 0.00 , 1.25 , -0.50 } ; // the light
Vector e = {0.5, 0.5, -1.00}; //the eye
//
double dotp( Vector t , Vector u )
{
   return t.x * u.x + t.y * u.y + t.z * u.z ;
}
//
void diff( Vector* t , Vector u , Vector v ) // t = u - v
{
   t->x = u.x - v.x ;
   t->y = u.y - v.y ;
   t->z = u.z - v.z ;
}
void unitVector( Vector* t )
{
   double mag = sqrt( (t->x)*(t->x) + (t->y)*(t->y) + (t->z)*(t->z) );
   //
   t->x = (t->x)/mag;
   t->y = (t->y)/mag;
   t->z = (t->z)/mag;
}
double getMagnitude( Vector t )
{
   return sqrt( (t.x)*(t.x) + (t.y)*(t.y) + (t.z)*(t.z) );
}
//
Sphere items[codeSpheres + fileSpheres]; // Number of items onscreen
int rgb[H][W][3] ; //Colors at each pixel
//
double hitSphere(Ray p, Sphere s)
{
   Vector d ; //Distace vector
   diff(&d, p.start, s.c) ;
   //
   double a = dotp(p.end, p.end); //dotp(p.end, p.end) ;
   double b = 2.0 * dotp(p.end, d) ;
   double c = dotp(d, d) - (s.r)*(s.r) ;
   double discrim = b*b - (4*a*c) ;
   //
   double t1, t2;
   //
   if (discrim >= 0)
   {
      t1 = ((-1)*b + sqrt(discrim))/(2*a) ;
      t2 = ((-1)*b - sqrt(discrim))/(2*a) ;
      //
      if(t1 > 0 && t1 < t2)
      {
         return t1;
      }
      else if (t2 > 0)
      {
         return t2;
      }
   }
   return 0;
   //
}
void colorPixel(Color* h, Ray p, int f, double t)  //costheta = n * s
{
   Vector q;
   double m = getMagnitude( p.end );
   Sphere sph = items[f] ;
   //
   q.x = p.start.x + t*(p.end.x/m) ;
   q.y = p.start.y + t*(p.end.y/m) ;
   q.z = p.start.z + t*(p.end.z/m) ;
   //
   Vector n;
   Vector s;
   //
   n.x = (q.x + 0.001*(q.x-sph.c.x)/(sph.r)) - sph.c.x ;
   n.y = (q.y + 0.001*(q.y-sph.c.y)/(sph.r)) - sph.c.y ;
   n.z = (q.z + 0.001*(q.z-sph.c.z)/(sph.r)) - sph.c.z ;
   //
   s.x = g.x - (q.x + 0.001*(q.x-sph.c.x)/(sph.r)) ;
   s.y = g.y - (q.y + 0.001*(q.y-sph.c.y)/(sph.r)) ;
   s.z = g.z - (q.z + 0.001*(q.z-sph.c.z)/(sph.r)) ;
   //
   unitVector( &n ) ;
   unitVector( &s ) ;
   //
   //BINARY Shadow
   Ray shadowTest;
   int inShadow = 0;
   shadowTest.start = q;
   shadowTest.end = s;
   int i;
   for(i = 0; i < codeSpheres + fileSpheres; i++)
         {
            t = hitSphere(shadowTest, items[i]);
            if(t > 0)
            {
               inShadow = 1;
            }
         }
   //
   //
   //
   double cosTheta;
   if( inShadow == 0 )
   {
      cosTheta = dotp(n, s);
   }
   
   else //Binary Shadow
   {
      cosTheta = 0.0 ;
   }
   
   int red = sph.h.r ;
   int green = sph.h.g ;
   int blue = sph.h.b ;
   //

/// x and z value, multiply by 10
//cast to int
/*
   sum the ints
   take mod 2 of the value
*/
   int floorBool = ((int)(10*(q.x+1000)) + (int)(10*(q.z+1000)))%2;
   if(f == 0 && floorBool == 0)
   {
      red = 120;
      green = 81;
      blue = 169;
   }
   //
   h->r = abs((int)(0.5*red*(1+cosTheta)));
   h->g = abs((int)(0.5*green*(1+cosTheta)));
   h->b = abs((int)(0.5*blue*(1+cosTheta)));
   /*
   h->r = abs((int)(sph.h.r * cosTheta));
   h->g = abs((int)(sph.h.g * cosTheta));
   h->b = abs((int)(sph.h.b * cosTheta));
   */
}
//
void itrItem(int k) //k value for determining frame adjustment
{
   //
   int x, y;
   //
   Color h;
   //
   for( y = 0 ; y < H ; y++ )
   {
      #pragma omp parallel for private(x,h)
      for( x = 0 ; x < W ; x++)
      {
         Ray p;
         //For an animation:
         //
         e.x = 0.5 ;
         e.y = 0.5 ;
         e.z = -1.0 + 0.005*(k-1);
         //
         p.start = e;
         //Correction
         double px = ((x * 1.0 +.05) *( 4.0/3 ))/W -(1.0/6) ;
         double py = 1 -((y * 1.0 +.05) *( 1.0/H )) ;
         double pz = 0.0;
         //
         //Default set to sky blue
         rgb[y][x][0] = 178;
         rgb[y][x][1] = 255;
         rgb[y][x][2] = 255;
         //
         p.end.x = px - p.start.x ;
         p.end.y = py - p.start.y ;
         p.end.z = pz - p.start.z ;
         //
         unitVector(&p.end);
         //
         double minT = 100000.0; //BIGNUMBER
         //
         int i;
         //
         for(i = 0; i < codeSpheres + fileSpheres; i++)
         {
            double t = hitSphere(p, items[i]);
            //
            if(t > 0 && t < minT)  //If it actually hits
            {
               Color h;
               colorPixel(&h, p, i, t); //Get the pixel color w/ shadow
               //
               rgb[y][x][0] = h.r;
               rgb[y][x][1] = h.g;
               rgb[y][x][2] = h.b;
               //
               minT = t;
            }
         }
         
      }
   }
   //
}
void makePPM()
{
   int x, y;
   FILE* fout ;
   //
   fout = fopen( "tmp.ppm" , "w") ;
   fprintf( fout , "P3\n" ) ;
   fprintf( fout , "%d %d\n" , W , H ) ;
   fprintf( fout , "255\n" ) ;
   //
   for( y = 0 ; y < H ; y++ )
   {
        for( x = 0 ; x < W ; x++)
        {
            fprintf( fout , "%d %d %d\n" , rgb[y][x][0] , rgb[y][x][1] , rgb[y][x][2] ) ;
        }
   }
   //
   fclose( fout ) ;
}
//
void addSpheresFromFile(char* name, Color i)
{
	FILE*  fin;
	double x,y,z,r;
	int    j,n=5000;
	//         
	fin=fopen("helix.txt","r");
	//
   for(j=0;j<n;j++)
	{
		fscanf( fin , "%lf %lf %lf %lf" , &x,&y,&z,&r ) ;
		//
		items[j+codeSpheres].c.x = x;
		items[j+codeSpheres].c.y = y;
		items[j+codeSpheres].c.z = z;
		items[j+codeSpheres].r = r;
		items[j+codeSpheres].h.r = i.r;
      items[j+codeSpheres].h.g = i.g;
      items[j+codeSpheres].h.b = i.b;
		//
	}
	//
	fclose(fin);
}
void init()
{
   //
   items[0].c.x =      0.50 ;
   items[0].c.y = -20000.00 ; // the floor
   items[0].c.z =      0.50 ;
   items[0].r   =  20000.25 ;
   items[0].h.r =    205    ; // color is Peru
   items[0].h.g =    133    ;
   items[0].h.b =     63    ;
   //
   items[1].c.x =      0.50 ;
   items[1].c.y =      0.50 ;
   items[1].c.z =      0.50 ;
   items[1].r   =      0.25 ;
   items[1].h.r =      0    ; // color is Blue
   items[1].h.g =      0    ;
   items[1].h.b =    255    ;
   //
   items[2].c.x =      1.00 ;
   items[2].c.y =      0.50 ;
   items[2].c.z =      1.00 ;
   items[2].r   =      0.25 ;
   items[2].h.r =      0    ; // color is Green
   items[2].h.g =    255    ;
   items[2].h.b =      0    ;
   //
   items[3].c.x =      0.00 ;
   items[3].c.y =      0.75 ;
   items[3].c.z =      1.25 ;
   items[3].r   =      0.50 ;
   items[3].h.r =    255    ; // color is Red
   items[3].h.g =      0    ;
   items[3].h.b =      0    ; 
   //
   /*
   char* name = "helix.txt" ;
   Color i;
   i.r = 0;
   i.g = 0;
   i.b = 150;
   addSpheresFromFile(name, i); */
}
//
void makePNG(int k)
{
   char filename[50];
   sprintf( filename , "convert tmp.ppm f%07d.png" , k );
   system( filename ) ;
   remove("tmp.ppm");
}
//
int main(void)
{
   init() ;
   //
   int k;
   for(k = 1; k <= frameCount; k++)
   {
       itrItem(k);
       makePPM( );
       makePNG(k);
   }
   //
   return 0 ;
   //
}

//end of file