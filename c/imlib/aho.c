#include <gtk/gtk.h>
#include <gdk_imlib.h>


int
main( int argc, char **argv )
{
    GdkImlibImage *gim;
    GdkPixmap *pix;
    
    gtk_set_locale();
    gtk_init( &argc, &argv );

    gdk_imlib_init();
    gtk_widget_push_visual(  (GdkVisual*)gdk_imlib_get_visual() );
    gtk_widget_push_colormap( (GdkColormap*) gdk_imlib_get_colormap() );

    gim = gdk_imlib_load_image( "Boy.pnm" );
    printf( "%p\n", gim );
    gdk_imlib_render( gim, gim->rgb_width, gim->rgb_height );
    pix = gdk_imlib_move_image( gim );

    printf( "%p\n", pix );

    return 0;
}
