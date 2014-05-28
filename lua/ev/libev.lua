-- LuaJIT binding to libev (http://libev.schmorp.de/)
--
-- uses almost-identical API to lua-ev (https://github.com/brimworks/lua-ev)
--
-- Author:  Evan Wies <evan@neomantra.net>
--

local ffi = require('ffi')

local bit = require("bit")
local band, bor = bit.band, bit.bor

local libev = ffi.load('ev')

-- extracted from preprocessing <ev.h>
ffi.cdef[[

/* eventmask, revents, events... */
enum {
  EV_UNDEF    = 0xFFFFFFFF, /* guaranteed to be invalid */
  EV_NONE     =       0x00, /* no events */
  EV_READ     =       0x01, /* ev_io detected read will not block */
  EV_WRITE    =       0x02, /* ev_io detected write will not block */
  EV__IOFDSET =       0x80, /* internal use only */
  EV_IO       =    EV_READ, /* alias for type-detection */
  EV_TIMER    = 0x00000100, /* timer timed out */
  EV_PERIODIC = 0x00000200, /* periodic timer timed out */
  EV_SIGNAL   = 0x00000400, /* signal was received */
  EV_CHILD    = 0x00000800, /* child/pid had status change */
  EV_STAT     = 0x00001000, /* stat data changed */
  EV_IDLE     = 0x00002000, /* event loop is idling */
  EV_PREPARE  = 0x00004000, /* event loop about to poll */
  EV_CHECK    = 0x00008000, /* event loop finished poll */
  EV_EMBED    = 0x00010000, /* embedded event loop needs sweep */
  EV_FORK     = 0x00020000, /* event loop resumed in child */
  EV_CLEANUP  = 0x00040000, /* event loop resumed in child */
  EV_ASYNC    = 0x00080000, /* async intra-loop signal */
  EV_CUSTOM   = 0x01000000, /* for use by user code */
  EV_ERROR    = 0x80000000  /* sent when an error occurs */
};

/* flag bits for ev_default_loop and ev_loop_new */
enum {
  /* the default */
  EVFLAG_AUTO      = 0x00000000U, /* not quite a mask */
  /* flag bits */
  EVFLAG_NOENV     = 0x01000000U, /* do NOT consult environment */
  EVFLAG_FORKCHECK = 0x02000000U, /* check for a fork in each iteration */
  /* debugging/feature disable */
  EVFLAG_NOINOTIFY = 0x00100000U, /* do not attempt to use inotify */
  EVFLAG_SIGNALFD  = 0x00200000U, /* attempt to use signalfd */
  EVFLAG_NOSIGMASK = 0x00400000U  /* avoid modifying the signal mask */
};

/* method bits to be ored together */
enum {
  EVBACKEND_SELECT  = 0x00000001U, /* about anywhere */
  EVBACKEND_POLL    = 0x00000002U, /* !win */
  EVBACKEND_EPOLL   = 0x00000004U, /* linux */
  EVBACKEND_KQUEUE  = 0x00000008U, /* bsd */
  EVBACKEND_DEVPOLL = 0x00000010U, /* solaris 8 */ /* NYI */
  EVBACKEND_PORT    = 0x00000020U, /* solaris 10 */
  EVBACKEND_ALL     = 0x0000003FU, /* all known backends */
  EVBACKEND_MASK    = 0x0000FFFFU  /* all future backends */
};

typedef double ev_tstamp;

ev_tstamp ev_time (void);
void ev_sleep (ev_tstamp delay); /* sleep for a while */

/* ev_run flags values */
enum {
  EVRUN_NOWAIT = 1, /* do not block/wait */
  EVRUN_ONCE   = 2  /* block *once* only */
};

/* ev_break how values */
enum {
  EVBREAK_CANCEL = 0, /* undo unloop */
  EVBREAK_ONE    = 1, /* unloop once */
  EVBREAK_ALL    = 2  /* unloop all loops */
};

typedef struct ev_loop ev_loop;

typedef struct ev_watcher
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_watcher *w, int revents);
} ev_watcher;

typedef struct ev_watcher_list
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_watcher_list *w, int revents); struct ev_watcher_list *next;
} ev_watcher_list;

typedef struct ev_watcher_time
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_watcher_time *w, int revents); ev_tstamp at;
} ev_watcher_time;

typedef struct ev_io
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_io *w, int revents); struct ev_watcher_list *next;
  int fd;
  int events;
} ev_io;

typedef struct ev_timer
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_timer *w, int revents); ev_tstamp at;
  ev_tstamp repeat_;
} ev_timer;

typedef struct ev_periodic
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_periodic *w, int revents); ev_tstamp at;
  ev_tstamp offset;
  ev_tstamp interval;
  ev_tstamp (*reschedule_cb)(struct ev_periodic *w, ev_tstamp now);
} ev_periodic;

typedef struct ev_signal
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_signal *w, int revents); struct ev_watcher_list *next;
  int signum;
} ev_signal;

typedef struct ev_child
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_child *w, int revents); struct ev_watcher_list *next;
  int flags;
  int pid;
  int rpid;
  int rstatus;
} ev_child;

typedef struct ev_idle
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_idle *w, int revents);
} ev_idle;

typedef struct ev_prepare
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_prepare *w, int revents);
} ev_prepare;

typedef struct ev_check
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_check *w, int revents);
} ev_check;

typedef struct ev_fork
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_fork *w, int revents);
} ev_fork;

typedef struct ev_cleanup
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_cleanup *w, int revents);
} ev_cleanup;

typedef struct ev_embed
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_embed *w, int revents);
  struct ev_loop *other;
  ev_io io;
  ev_prepare prepare;
  ev_check check;
  ev_timer timer;
  ev_periodic periodic;
  ev_idle idle;
  ev_fork fork;
  ev_cleanup cleanup;
} ev_embed;

typedef int sig_atomic_t;
typedef struct ev_async
{
  int active; int pending; int priority; void *data; void (*cb)(struct ev_loop *loop, struct ev_async *w, int revents);
  sig_atomic_t volatile sent;
} ev_async;

void ev_signal_start (struct ev_loop *loop, ev_signal *w);
void ev_signal_stop (struct ev_loop *loop, ev_signal *w);

struct ev_loop *ev_default_loop (unsigned int flags );
struct ev_loop *ev_loop_new (unsigned int flags );
ev_tstamp ev_now (struct ev_loop *loop);
void ev_loop_destroy (struct ev_loop *loop);
unsigned int ev_iteration (struct ev_loop *loop);
unsigned int ev_depth (struct ev_loop *loop);

void ev_io_start (struct ev_loop *loop, ev_io *w);
void ev_io_stop (struct ev_loop *loop, ev_io *w);
//void ev_io_init (struct ev_io *, void (*callback)(), int fd, int events );

void ev_run (struct ev_loop *loop, int flags );
void ev_break (struct ev_loop *loop, int how );
void ev_suspend (struct ev_loop *loop);
void ev_resume (struct ev_loop *loop);
int ev_clear_pending (struct ev_loop *loop, void *w);

void ev_timer_start (struct ev_loop *loop, ev_timer *w);
void ev_timer_stop (struct ev_loop *loop, ev_timer *w);
void ev_timer_again (struct ev_loop *loop, ev_timer *w);
ev_tstamp ev_timer_remaining (struct ev_loop *loop, ev_timer *w);

void ev_idle_start (struct ev_loop *loop, ev_idle *w);
void ev_idle_stop (struct ev_loop *loop, ev_idle *w);

]]

-- local ev_loop_t = ffi.typeof('ev_loop')
-- ffi.metatype( ev_loop_t, {
--     __index = {
--         -- loop:run( flags )
--         run = function( ev_loop, flags )
--                 flags = flags or 0
--                 libev.ev_run(ev_loop, flags )
--             end,
--         -- loop:halt( how )
--         halt = function( ev_loop, how )
--                 how = how or libev.EVBREAK_ALL
--                 libev.ev_break(ev_loop, how)
--             end,
--         -- loop:suspend()
--         suspend = function( ev_loop )
--                 libev.ev_suspend(ev_loop)
--             end,
--         -- loop:resume()
--         resume = function( ev_loop )
--                 libev.ev_resume(ev_loop)
--             end,
--         -- bool = loop:is_default()
--         is_default = function( ev_loop )
--                 return libev.is_default_loop(ev_loop) ~= 0
--             end,
--         -- num = loop:iteration()
--         iteration = function( ev_loop )
--                 return libev.ev_iteration(ev_loop)
--             end,
--         -- num = loop:depth() [libev >= 3.7]
--         depth = function( ev_loop )
--                 return libev.ev_depth(ev_loop)
--             end,
--         -- epochs = loop:now()
--         now = function( ev_loop )
--                 return libev.ev_now(ev_loop)
--             end,
--         -- epochs = loop:update_now()
--         update_now = function( ev_loop )
--                 libev.ev_now_update(ev_loop)
--                 return libev.ev_now(ev_loop)
--             end,
--         -- backend_id = loop:backend()
--         backend = function( ev_loop )
--                 return libev.ev_backend( ev_loop )
--             end,
--         -- loop:loop()
--         loop = function(ev_loop) ev_loop:run() end,
--         -- loop:unloop()
--         unloop = function( ev_loop ) ev_loop:halt() end,
--     },
--     __gc = libev.ev_loop_destroy,
-- })

-- local ev_timer_t = ffi.typeof('ev_timer')
-- ffi.metatype( ev_timer_t, {
--     __index = {
--         -- timer:start(loop [, is_daemon])
--         start = function( ev_timer, ev_loop, is_daemon )
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_timer_start(ev_loop, ev_timer)
--                 --TODO loop_start_watcher(L, 2, 1, is_daemon);
--             end,
--         -- timer:stop(loop)
--         stop = function( ev_timer, ev_loop )
--                 -- TODO loop_stop_watcher(L, 2, 1);
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_timer_stop(ev_loop, ev_timer)
--             end,
--         -- timer:again(loop [, seconds])
--         again = function( ev_timer, ev_loop, repeat_seconds )
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 repeat_seconds = repeat_seconds or 0
--                 if repeat_seconds then
--                     assert( repeat_seconds >= 0, "repeat_seconds must be >= 0" )
--                     timer.repeat_ = repeat_seconds
--                 end
--                 if timer.repeat_ ~= 0 then
--                     libev.ev_timer_again(ev_loop, ev_timer)
--                     --TODO loop_start_watcher(L, 2, 1, -1);
--                 else
--                     -- Just calling stop instead of again in case the symantics change in libev
--                     --TODO loop_stop_watcher(L, 2, 1);
--                     libev.ev_timer_stop(ev_loop, ev_timer)
--                 end
--             end,
--         clear_pending = function( ev_timer, ev_loop )
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 local revents = libev.ev_clear_pending(ev_loop, ev_timer)
--                 if timer.repeat_ ~= 0 and band(revents, libev.EV_TIMEOUT) ~= 0 then
--                     --TODO loop_stop_watcher(L, 2, 1)
--                 end
--                 return revents
--             end,
--         remaining = function( ev_timer, ev_loop )
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 return libev.ev_timer_remaining(ev_loop, ev_timer)
--             end,
--     },
-- })

-- local ev_signal_t = ffi.typeof('ev_signal')
-- ffi.metatype( ev_signal_t, {
--     __index = {
--         -- signal:start(loop [, is_daemon])
--         start = function( ev_signal, ev_loop, is_daemon )
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_signal_start(ev_loop, ev_signal)
--                 --TODO loop_start_watcher(L, 2, 1, is_daemon);
--             end,
--         -- signal:stop(loop)
--         stop = function( ev_signal, ev_loop )
--                 -- TODO loop_stop_watcher(L, 2, 1);
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_signal_stop(ev_loop, ev_signal)
--             end,
--     },
-- })

-- local ev_io_t = ffi.typeof('ev_io')
-- ffi.metatype( ev_io_t, {
--     __index = {
--         -- io:start(loop [, is_daemon])
--         start = function( ev_io, ev_loop, is_daemon )
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_io_start(ev_loop, ev_io)
--                 --TODO loop_start_watcher(L, 2, 1, is_daemon);
--             end,
--         -- io:stop(loop)
--         stop = function( ev_io, ev_loop )
--                 -- TODO loop_stop_watcher(L, 2, 1);
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_io_stop(ev_loop, ev_io)
--             end,
--         -- fd = io:getfd()
--         getfd = function( ev_io )
--                 return io.fd
--             end,
--     },
-- })

-- local ev_idle_t = ffi.typeof('ev_idle')
-- ffi.metatype( ev_idle_t, {
--     __index = {
--         -- idle:start(loop [, is_daemon])
--         start = function( ev_idle, ev_loop, is_daemon )
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_idle_start(ev_loop, ev_idle)
--                 --TODO loop_start_watcher(L, 2, 1, is_daemon);
--             end,
--         -- idle:stop(loop)
--         stop = function( ev_idle, ev_loop )
--                 -- TODO loop_stop_watcher(L, 2, 1);
--                 assert( ffi.istype(ev_loop_t, ev_loop), "loop is not an ev_loop" )
--                 libev.ev_idle_stop(ev_loop, ev_idle)
--             end,
--     },
-- })


-- -- Public API
-- -- This is the table we return to the requirer 
-- local ev = {
--     -- enums
--     UNDEF = libev.EV_UNDEF,
--     NONE = libev.EV_NONE,
--     READ = libev.EV_READ,
--     WRITE = libev.EV_WRITE,
--     IOFDSET = libev.EV__IOFDSET,
--     wIO = libev.EV_IO,
--     wTIMER = libev.EV_TIMER,
--     wPERIODIC = libev.EV_PERIODIC,
--     wSIGNAL = libev.EV_SIGNAL,
--     wCHILD = libev.EV_CHILD,
--     wSTAT = libev.EV_STAT,
--     wIDLE = libev.EV_IDLE,
--     wPREPARE = libev.EV_PREPARE,
--     wCHECK = libev.EV_CHECK,
--     wEMBED = libev.EV_EMBED,
--     wFORK = libev.EV_FORK,
--     wCLEANUP = libev.EV_CLEANUP,
--     wASYNC = libev.EV_ASYNC,
--     wCUSTOM = libev.EV_CUSTOM,
--     ERROR =libev.EV_ERROR,
--     FLAG_AUTO = libev.EVFLAG_AUTO,
--     FLAG_NOENV = libev.EVFLAG_NOENV,
--     FLAG_FORKCHECK = libev.EVFLAG_FORKCHECK,
--     FLAG_NOINOTIFY = libev.EVFLAG_NOINOTIFY,
--     FLAG_SIGNALFD = libev.EVFLAG_SIGNALFD,
--     FLAG_NOSIGMASK = libev.EVFLAG_NOSIGMASK,
--     BACKEND_SELECT = libev.EVBACKEND_SELECT,
--     BACKEND_POLL = libev.EVBACKEND_POLL,
--     BACKEND_EPOLL = libev.EVBACKEND_EPOLL,
--     BACKEND_KQUEUE = libev.EVBACKEND_KQUEUE,
--     BACKEND_DEVPOLL = libev.EVBACKEND_DEVPOLL,
--     BACKEND_PORT = libev.EVBACKEND_PORT,
--     BACKEND_ALL = libev.EVBACKEND_ALL,
--     BACKEND_MASK = libev.EVBACKEND_MASK,
-- }

-- --- major, minor = ev.version()
-- function ev.version()
--     return libev.ev_version_major(), libev.ev_version_minor()
-- end

-- --- 
-- function ev.time()
--     return libev.ev_time()
-- end
-- function ev.sleep( interval )
--     libev.ev_sleep( interval )
-- end

-- -- loop = ev.Loop()
-- function ev.Loop( flags )
--     flags = flags or libev.EVFLAG_AUTO
--     return libev.ev_loop_new( flags )
-- end

-- ---timer = ev.Timer.new(on_timeout, after_seconds [, repeat_seconds])
-- function ev.Timer( on_timeout_fn, after_seconds, repeat_seconds )
--     assert( on_timeout_fn, "on_timeout_fn cannot be nil" )
--     repeat_seconds = repeat_seconds or 0
--     assert( after_seconds > 0, "after_seconds must be > 0" )
--     assert( repeat_seconds >= 0, "repeat_seconds must be >= 0" )

--     local ev_timer = ev_timer_t()
--     ev_timer.active = 0
--     ev_timer.pending = 0
--     ev_timer.priority = 0
--     ev_timer.cb = on_timeout_fn
--     ev_timer.at = after_seconds
--     ev_timer.repeat_ = repeat_seconds
--     return ev_timer
-- end

-- --sig = ev.Signal.new(on_signal, signal_number)
-- function ev.Signal(on_signal_fn, signal_number)
--     assert( on_signal_fn, "on_signal_fn cannot be nil" )
--     local ev_signal = ev_signal_t()
--     ev_signal.active = 0
--     ev_signal.pending = 0
--     ev_signal.priority = 0
--     ev_signal.cb = on_signal_fn
--     ev_signal.signum = signal_number
--     return ev_signal
-- end

-- --- io = ev.IO(on_io, file_descriptor, revents)
-- function ev.IO(on_io_fn, file_descriptor, revents)
--     assert( on_io_fn, "on_io_fn cannot be nil" )
--     local ev_io = ev_io_t()
--     ev_io.active = 0
--     ev_io.pending = 0
--     ev_io.priority = 0
--     ev_io.cb = on_io_fn
--     ev_io.fd = file_descriptor
--     revents = revents or 0 
--     ev_io.events = bor( revents, ffi.C.EV__IOFDSET )
--     return ev_io
-- end

-- --- idle = ev.Idle.new(on_idle)
-- function ev.Idle(on_idle_fn)
--     assert( on_idle_fn, "on_idle_fn cannot be nil" )
--     local ev_idle = ev_idle_t()
--     ev_idle.active = 0
--     ev_idle.pending = 0
--     ev_idle.priority = 0
--     ev_idle.cb = on_idle_fn
--     return ev_idle
-- end

-- --TODO Child, Stat Periodic, Prepare, Check, Embed, Async, Clenaup, Fork


-- -- Return the Public API
-- return ev

return libev
