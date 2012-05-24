luvit write() causes EFAULT
====

How to reproduce EFAULT
----
 1. luvit sv.lua (uses port 61111)
 2. luvit cl.lua

then get

    write(5, "\1\2\3\4\5\6\7\10\t\n\v\f\r\16\17\20\21\22\23\24\25\26\27\30\31\32\33\34\35\36\37 "..., 160000) = 49152
    write(5, 0x4200a020, 110848)            = -1 EFAULT (Bad address)


luvit doesn't resend the data, so the chunk is lost if EFAULT occurs.

writing 160KB to a socket causes EFAULT, and libuv disposes the data. data is now lost.


in luv_stream.c: luv_write() :

    /* Store the chunk
     * TODO: this is probably unsafe, should investigate
     */
    buf = uv_buf_init((char*)chunk, len);
    uv_write(req, handle, &buf, 1, luv_after_write);
    return 0;

    Why it is considered unsafe?


