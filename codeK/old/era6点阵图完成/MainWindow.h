#include <gtk/gtk.h>


//char * titlebmp="Data/title.bmp";	



#define buttomNum  4
#define textNum  4
GtkWidget * table;//主界面的table
//GtkWidget * texts[textNum];
GtkWidget * buttoms[buttomNum];
GtkWidget * srollWindows[textNum];
GdkWindow *paintwindow;
GtkWidget *darea;
GtkWidget *fixed;
cairo_surface_t *image;
void hello( GtkWidget *widget, gpointer data );
void drow1( GtkWidget *widget, gpointer data );
void drow2( GtkWidget *widget, gpointer data );
//进行一些初始化
void ProInitialization();
void myMainWindow(GtkWidget *window);
void paintInSurfaceRGB24(cairo_surface_t *image,int LineWeight,int Line,unsigned char * bmpdata);//传入图片的像素大小


 inline int compRGB(unsigned char *color1,unsigned char *color2)//比较俩种颜色的相似度，color1和color2是数组而不是字符串，所以请自己保证数组没有越界
{
	g_return_if_fail(color1 != NULL);
	g_return_if_fail(color2 != NULL);
	int r=(int )color1[0]-(int )color2[0];
	int g=(int )color1[1]-(int )color2[1];	
	int b=(int )color1[2]-(int )color2[2];

	//相似度的定义是需要仔细考虑的
	r= (r<0)? (-1)*r:r;
	g= (g<0)? (-1)*g:g;
	b= (b<0)? (-1)*b:b;	

	return r;
}














