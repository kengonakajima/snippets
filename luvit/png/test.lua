-- read , modify, write a png


require( "../lumino/lumino" )

ffidefstr = readFile( "./png.lua")

ffi = require("ffi")


ffi.cdef(ffidefstr)


--     /* for setjmp.. */
--     typedef int* jmp_buf; /* OSX..  */
--     int setjmp( jmp_buf env );

    
--     typedef uint8_t * png_bytep;
--     typedef uint32_t png_size_t;    
--     int png_sig_cmp (png_bytep sig, png_size_t start, png_size_t num_to_check);

--     void * memcpy(void *restrict s1, const void *restrict s2, size_t n);

--     typedef void * png_structp;
--     typedef const uint8_t * png_const_charp;
--     typedef void * png_voidp;
--     typedef void * png_error_ptr;
--     png_structp png_create_read_struct (png_const_charp user_png_ver, png_voidp error_ptr, png_error_ptr error_fn, png_error_ptr warn_fn);

--     typedef void * png_infop;
--     png_infop png_create_info_struct (png_structp png_ptr);

--     /*    jmp_buf png_jmpbuf( png_structp png_ptr );*/

--     typedef void * png_rw_ptr;
--     void png_set_read_fn (png_structp png_ptr, png_voidp io_ptr, png_rw_ptr read_data_fn);
    

-- ]]

local libpng = ffi.load( "/usr/X11/lib/libpng15.dylib")

print("libpng:", libpng)

sol = readFile( "./sol.png" )
solbuf = ffi.new( "uint8_t[?]", #sol)
ffi.copy( solbuf, sol, #sol )

hbuf = ffi.new( "uint8_t[8]")
ffi.C.memcpy( hbuf, sol, 8 )

print( "hbuf:", hbuf, typeof(hbuf) )

hoge = "asdfa"

hstr = ffi.string( hbuf, 8 )

ary = hoge:byte(1,8)


-- read..

ck = libpng.png_sig_cmp( hbuf, 0, 8 )

print( "sol len:", #sol )

print( "ck:", ck )
if ck ~= 0 then
  error( "png_sig_cmp fails")
end

verstr = "1.5.4" -- osx lion
png_ptr = libpng.png_create_read_struct( verstr, nil, nil, nil )
print("png_ptr:", png_ptr )


info_ptr = libpng.png_create_info_struct(png_ptr)
assert(info_ptr)

print("info_ptr:", info_ptr )

-- jmpbuf = libpng.png_jmpbuf( png_ptr )  これはマクロだった！が、なくても動くようである。
-- print("jmpbuf:", jmpbuf )
-- sjresult = ffi.C.setjmp( jmpbuf )
--print("sjresult:", sjresult )


function readfn(pngptr, outBytesPtr, readSize )
  print("readn")
end

libpng.png_set_read_fn( png_ptr, solbuf, readfn )

print("OK")
  







-- int x, y;


-- int width, height;
-- png_byte color_type;
-- png_byte bit_depth;

-- png_structp png_ptr;
-- png_infop info_ptr;
-- int number_of_passes;
-- png_bytep * row_pointers;

-- void read_png_file(char* file_name)
-- {
--     char header[8];    // 8 is the maximum size that can be checked

--     /* open file and test for it being a png */
--     FILE *fp = fopen(file_name, "rb");
--     if (!fp)
--         abort_("[read_png_file] File %s could not be opened for reading", file_name);
--     fread(header, 1, 8, fp);
--     if (png_sig_cmp( (png_byte*) header, 0, 8))
--         abort_("[read_png_file] File %s is not recognized as a PNG file", file_name);


--     /* initialize stuff */
--     png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

--     if (!png_ptr)
--         abort_("[read_png_file] png_create_read_struct failed");

--     info_ptr = png_create_info_struct(png_ptr);
--     if (!info_ptr)
--         abort_("[read_png_file] png_create_info_struct failed");

--     if (setjmp(png_jmpbuf(png_ptr)))
--         abort_("[read_png_file] Error during init_io");

--     png_init_io(png_ptr, fp);
--     png_set_sig_bytes(png_ptr, 8);

--     png_read_info(png_ptr, info_ptr);

--     width = png_get_image_width(png_ptr, info_ptr);
--     height = png_get_image_height(png_ptr, info_ptr);
--     color_type = png_get_color_type(png_ptr, info_ptr);
--     bit_depth = png_get_bit_depth(png_ptr, info_ptr);

--     number_of_passes = png_set_interlace_handling(png_ptr);
--     png_read_update_info(png_ptr, info_ptr);


--     /* read file */
--     if (setjmp(png_jmpbuf(png_ptr)))
--         abort_("[read_png_file] Error during read_image");

--     row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
--     for (y=0; y<height; y++)
--         row_pointers[y] = (png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));

--     png_read_image(png_ptr, row_pointers);

--     fclose(fp);
-- }


-- void write_png_file(char* file_name)
-- {
--     /* create file */
--     FILE *fp = fopen(file_name, "wb");
--     if (!fp)
--         abort_("[write_png_file] File %s could not be opened for writing", file_name);


--     /* initialize stuff */
--     png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

--     if (!png_ptr)
--         abort_("[write_png_file] png_create_write_struct failed");

--     info_ptr = png_create_info_struct(png_ptr);
--     if (!info_ptr)
--         abort_("[write_png_file] png_create_info_struct failed");

--     if (setjmp(png_jmpbuf(png_ptr)))
--         abort_("[write_png_file] Error during init_io");

--     png_init_io(png_ptr, fp);


--     /* write header */
--     if (setjmp(png_jmpbuf(png_ptr)))
--         abort_("[write_png_file] Error during writing header");

--     png_set_IHDR(png_ptr, info_ptr, width, height,
--                  bit_depth, color_type, PNG_INTERLACE_NONE,
--                  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

--     png_write_info(png_ptr, info_ptr);


--     /* write bytes */
--     if (setjmp(png_jmpbuf(png_ptr)))
--         abort_("[write_png_file] Error during writing bytes");

--     png_write_image(png_ptr, row_pointers);


--     /* end write */
--     if (setjmp(png_jmpbuf(png_ptr)))
--         abort_("[write_png_file] Error during end of write");

--     png_write_end(png_ptr, NULL);

--     /* cleanup heap allocation */
--     for (y=0; y<height; y++)
--         free(row_pointers[y]);
--     free(row_pointers);

--     fclose(fp);
-- }


-- void process_file(int cellsz, int xind, int yind, int alphav )
-- {
--     if (png_get_color_type(png_ptr, info_ptr) == PNG_COLOR_TYPE_RGB)
--         abort_("[process_file] input file is PNG_COLOR_TYPE_RGB but must be PNG_COLOR_TYPE_RGBA "
--                "(lacks the alpha channel)");

--     if (png_get_color_type(png_ptr, info_ptr) != PNG_COLOR_TYPE_RGBA)
--         abort_("[process_file] color_type of input file must be PNG_COLOR_TYPE_RGBA (%d) (is %d)",
--                PNG_COLOR_TYPE_RGBA, png_get_color_type(png_ptr, info_ptr));

--     int leftTopColorR=-1, leftTopColorG=-1, leftTopColorB=-1;
--     for (y=0; y<height; y++) {
--         png_byte* row = row_pointers[y];
--         for (x=0; x<width; x++) {

--             int spriteX = x / cellsz;
--             int spriteY = y / cellsz;

--             int alpha;

--             png_byte* ptr = &(row[x*4]);
--             //            printf("Pixel at position [ %d - %d ] has RGBA values: %d - %d - %d - %d\n",
--             //                   x, y, ptr[0], ptr[1], ptr[2], ptr[3]);

--             /* set red value to 0 and green value to the blue one */

--             if( xind == -1 ){
--                 alpha = spriteX + spriteY * cellsz;
--             } else {
--                 if( spriteX == xind and spriteY == yind ){
--                     alpha = alphav;
--                 } else {
--                     alpha = ptr[3];//0xff;
--                 }
--             }

--             ptr[3] = alpha;

--             if( x == 0 && y == 0 ) {
--                 printf("%d,%d,%d,%d\n", ptr[0], ptr[1], ptr[2], ptr[3]);
--             }
--             if( ptr[0] == 255 && ptr[1]==255 && ptr[2]==255 ){
--                 ptr[3]=0;
--             }
            
--             //            ptr[0] = 0;
--             //            ptr[1] = 0;
--             //            ptr[2] = 0;

--         }
--     }
-- }


-- int main(int argc, char **argv)
-- {
--     if (argc != 7){
--         abort_("Usage: program_name <file_in> <file_out> <CELLPXSIZE> <XIND> <YIND> <ALPHAVAL>");
--     }

--     read_png_file(argv[1]);
--     process_file(atoi(argv[3]),atoi(argv[4]), atoi(argv[5]),atoi(argv[6]));
--     write_png_file(argv[2]);

--     return 0;
-- } 
