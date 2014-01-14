/*#ifndef ColourSimilar*/

#include "MainWindow.h"
/*#endif*/
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <gtk/gtk.h>

extern void myMainWindow(GtkWidget *window);
/*示例开始 h e l l o w o r l d h e l l o w o r l d . c */

/*回调函数在本例中忽略了传递给程序的所有参数。下面是回调函数 */
/*extern struct shareData;*/
/*extern	int shmIdWithCandy;*/
/*extern	key_t keyWithCandy;*/
/*extern	shareData *pMapWithCandy;*/
/*extern	char* nameWithCandy ;*/

/*extern void otput();*/
/*extern void ex();*/
/*extern void myMainWindow(GtkWidget *window);*/
/*extern char* SendMsgWithCandy;*/
/*extern shareData *pMapWithCandy;*/
//void hello( GtkWidget *widget, gpointer data )
//{
	//const gchar *msg="Hello World\n";
//	g_print ("%s,is ok \n",msg);
//	gint32 file=open("data",O_RDWR);
//	gchar buffer[128];
//	gint32 haveRead=read(file,buffer,128);
	//gint32 file=open("data",O_RDWR);
	//gint32 haveRead=read(file,buffer,128);
//	buffer[haveRead]='\0';	
//	g_print ("%s,\n",buffer);
//	close(file);

	//otput();
//	ex();
//}


gint delete_event( GtkWidget *widget, GdkEvent *event, gpointer data )
{

/*如果在"delete_ev e n t "处理程序中返回 F A L S E,GTK 将引发一个 " d e s t r o y "
*信号,返回 TRUE意味着你不想关闭窗口。
* 这些在弹出 "你真的要退出 ? "对话框时很有作用 */

		
	g_print ("delete event occurred11111\n");
	gtk_main_quit();

/* 将TRUE改为FALSE,主窗口就会用一个 " d e l e t e _ e v e n t "信号,然后退出 */
	return(TRUE);
}


/* 另一个回调函数 */
void destroy( GtkWidget *widget, gpointer data )
{
	g_print ("destroy \n");
	gtk_main_quit();
}

int main( int argc, char *argv[] )
{
/* GtkWidget
是构件的存储类型 */
GtkWidget *window;
//GtkWidget *button;
/* 在所有的Gtk应用程序中都应该调用。它的作用是解析由命令行传递
* 进来的参数并将它返回给应用程序 */
gtk_init(&argc, &argv);
/* 创建一个主窗口 */
window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
/* 当给窗口一个 "del e t e _ e v e n t "信号时(这个信号是由窗口管
* 理器发出的,通常是在点击窗口标题条右边的 "×"按钮,或
* 者在标题条的快捷菜单上选择 " c l o s e "选项时发出的 ),我们
* 要求调用上面定义的 d e l e t e _ e v e n t ( )函数传递给这个回调函数
* 的数据是NULL,回调函数会忽略这个参数 */

gtk_window_set_title(GTK_WINDOW(window),"Nero  Sys");


/*g_signal_connect (window,"delete-event",G_CALLBACK (delete_event),NULL);*/


/* 这里,我们给 " d e s t o r y "事件连接一个信号处理函数,
* 当我们在窗口上调用 g t k _ w i d g e t _ d e s t r o y ( )函数
* 或者在"dele t e _ e v e n t "事件的回调函数中返回 F A L S E
* 时会发生这个事件 */
g_signal_connect (window, "destroy",G_CALLBACK (destroy), NULL);//window, "destroy", G_CALLBACK (gtk_main_quit), NULL
/* 设置窗口的边框宽度 */
gtk_container_set_border_width (GTK_CONTAINER (window), 10);
//gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
gtk_window_set_default_size (GTK_WINDOW(window), 450, 480);
/* 创建一个标题为" H e l l o W o r l d的按钮 */
gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
myMainWindow(window);

//button = gtk_button_new_with_label ("Hello World");

/* 当按钮接收到 " c l i c k e d "时,它会调用 h e l l o ( )函数,
* 传递的参数为 N U L L。函数h e l l o ( )是在上面定义的 */

//g_signal_connect (GTK_OBJECT (button), "clicked",
//GTK_SIGNAL_FUNC (hello), NULL);

/* 当点击按钮时,通过调用 g t k _ w i d g e t _ d e s t r o y ( w i n d o w )函数销毁窗口。
* 另外,"dest o r y "信号可以从这里发出,也可以来自于窗口管理器 */

//g_signal_connect_object (GTK_OBJECT (button), "clicked",
//			GTK_SIGNAL_FUNC (gtk_widget_destroy),
//			GTK_OBJECT (window));

/* 将按钮组装到窗口中(一个 g t k容器中)*/

//gtk_container_add (GTK_CONTAINER (window), button);

/* 最后一步就是显示新创建的构件 */
//gtk_widget_show (button);


/* 显示窗口 */
gtk_widget_show (window);
/* 所有的GTK应用程序都应该有一个 g t k _ m a i n ( )函数。
* 程序的控制权停在这里并等着事件的发生(比如一次按键或鼠标事件) */
gtk_main ();
return(0);
}
/* 示例结束*/
















































