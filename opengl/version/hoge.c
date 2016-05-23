#include <GL/gl.h>
#include <stdio.h>

int main() {
    printf( "vender:%s version:%s %x", glGetString(GL_VENDOR), glGetString(GL_VERSION), GL_VERSION );
    return 0;
}
        
