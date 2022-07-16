

#if !defined (LWS_PLUGIN_STATIC)
#define LWS_DLL
#define LWS_INTERNAL
#include <libwebsockets.h>
#endif


#include <string.h>
#include <assert.h>

#include "ws.h"
#include "cumino.h"


static uint32_t g_max_ccu=0;

WebsocketSession **g_wses_list;
static uint32_t g_wses_list_used=0;

uint64_t WebsocketSession::id_gen=1;

static WebsocketEstablishCallback g_on_establish=0;
static WebsocketCloseCallback g_on_close=0;
static WebsocketReceiveCallback g_on_receive=0;

void ws_set_establish_callback( WebsocketEstablishCallback cb ) {
    g_on_establish=cb;
}
void ws_set_close_callback( WebsocketCloseCallback cb ) {
    g_on_close=cb;    
}
void ws_set_receive_callback( WebsocketReceiveCallback cb ) {
    g_on_receive=cb;
}
void ws_set_app_data_ptr(uint64_t ws_id, void *ptr) {
    
}





struct per_session_data__minimal_server_echo {
    uint64_t ws_id;
    //	struct lws_ring *ring;
    //	uint32_t msglen;
    //	uint32_t tail;
	uint8_t completed:1;
    //	uint8_t flow_controlled:1;
    //	uint8_t write_consume_pending:1;    
};

struct vhd_minimal_server_echo {
	struct lws_context *context;
	struct lws_vhost *vhost;

	int *interrupted;
	int *options;
};


static int interrupted, options;

static const struct lws_protocol_vhost_options pvo_options = {
	NULL,
	NULL,
	"options",		/* pvo name */
	(const char *)&options	/* pvo value */
};

static const struct lws_protocol_vhost_options pvo_interrupted = {
	&pvo_options,
	NULL,
	"interrupted",		/* pvo name */
	(const char *)&interrupted	/* pvo value */
};

static const struct lws_protocol_vhost_options pvo = {
	NULL,				/* "next" pvo linked-list */
	&pvo_interrupted,		/* "child" pvo linked-list */
	"lws-minimal-server-echo",	/* protocol name we belong to on this vhost */
	""				/* ignored */
};
static const struct lws_extension extensions[] = {
	{
		"permessage-deflate",
		lws_extension_callback_pm_deflate,
		"permessage-deflate"
		 "; client_no_context_takeover"
		 "; client_max_window_bits"
	},
	{ NULL, NULL, NULL /* terminator */ }
};





static WebsocketSession *ws_find(uint64_t id) {
    for(int i=0;i<g_wses_list_used;i++) {
        WebsocketSession *ws=g_wses_list[i];
        if(ws&&ws->id==id)return ws;
    }
    return 0;
}
static WebsocketSession *ws_allocate(void*wsi,void *per_session_data) {
    assert(g_wses_list_used<=g_max_ccu);
    print("ws allocating new session");    
    for(int i=0;i<g_wses_list_used;i++) {
        WebsocketSession *ws=g_wses_list[i];
        if(!ws) {
            g_wses_list[i]=new WebsocketSession(wsi,per_session_data);
            return g_wses_list[i];
        }
    }

    if(g_wses_list_used==g_max_ccu) {
        print("ws full. maxccu:%d",g_max_ccu);
        return 0;   
    }
    WebsocketSession* ws=new WebsocketSession(wsi,per_session_data);
    g_wses_list[g_wses_list_used++]=ws;
    return ws;
}
static void ws_clean(WebsocketSession *ws) {
    for(int i=0;i<g_wses_list_used;i++) {
        if(g_wses_list[i]==ws) {
            print("ws cleaning at %d id:%lld",i,ws->id);
            delete ws;
            g_wses_list[i]=0;
            break;
        }
    }
}
   

void WebsocketSession::receiveData(const char *in, uint32_t l, bool first, bool fin) {
    print("first:%d fin:%d l:%d",first,fin,l);        
    if(first) receiving->clear();
    receiving->append(in,l);
    if(fin) {
        print("receiveData: fin!");
        if(g_on_receive) g_on_receive(id,receiving->data,receiving->used);
        receiving->clear();
    }
}

bool ws_send(uint64_t ws_id,const char *data,uint32_t l) {
    WebsocketSession *ws=ws_find(ws_id);
    if(!ws) {
        print("ws not found:%llx",ws_id);
        return false;
    }
    bool binary=true;
    int flags = lws_write_ws_flags(binary ? LWS_WRITE_BINARY : LWS_WRITE_TEXT,1,1);
    char *buf=(char*)malloc(LWS_PRE+l); // TODO: avoid allocation
    memcpy(buf+LWS_PRE,data,l);
    
    int m = lws_write((struct lws*)ws->wsi, (unsigned char*)(buf+LWS_PRE),l,(enum lws_write_protocol)flags);
    free(buf);
    if (m < l) {
        print("ERROR %d writing to ws socket\n", m);
        return false;
    }
    return true;
}
    


static int callback_minimal_server_echo(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	struct per_session_data__minimal_server_echo *pss =
			(struct per_session_data__minimal_server_echo *)user;
	struct vhd_minimal_server_echo *vhd = (struct vhd_minimal_server_echo *)
			lws_protocol_vh_priv_get(lws_get_vhost(wsi),
				lws_get_protocol(wsi));
    //	const struct msg *pmsg;
    //	struct msg amsg;
	int m, n, flags;

	switch (reason) {

	case LWS_CALLBACK_PROTOCOL_INIT:
        lwsl_user("LWS_CALLBACK_PROTOCOL_INIT");
		vhd = (struct vhd_minimal_server_echo*) lws_protocol_vh_priv_zalloc(lws_get_vhost(wsi),
				lws_get_protocol(wsi),
				sizeof(struct vhd_minimal_server_echo));
		if (!vhd)
			return -1;

		vhd->context = lws_get_context(wsi);
		vhd->vhost = lws_get_vhost(wsi);

		/* get the pointers we were passed in pvo */

		vhd->interrupted = (int *)lws_pvo_search(
			(const struct lws_protocol_vhost_options *)in,
			"interrupted")->value;
		vhd->options = (int *)lws_pvo_search(
			(const struct lws_protocol_vhost_options *)in,
			"options")->value;
		break;

	case LWS_CALLBACK_ESTABLISHED:
		/* generate a block of output before travis times us out */
		lwsl_warn("LWS_CALLBACK_ESTABLISHED\n");
        {
            WebsocketSession *wses = ws_allocate(wsi,pss);
            if(g_on_establish) g_on_establish(wses->id);
            pss->ws_id=wses->id;            
        }
		break;

	case LWS_CALLBACK_SERVER_WRITEABLE:

		lwsl_user("LWS_CALLBACK_SERVER_WRITEABLE\n");
		break;

	case LWS_CALLBACK_RECEIVE:

		lwsl_user("LWS_CALLBACK_RECEIVE: %4d (rpp %5d, first %d, "
                  "last %d, bin %d, len:%d\n",
                  (int)len, (int)lws_remaining_packet_payload(wsi),
                  lws_is_first_fragment(wsi),
                  lws_is_final_fragment(wsi),
                  lws_frame_is_binary(wsi), (int)len );


		if (len) {
			print("receive len:%d",len);
			//puts((const char *)in);
			//lwsl_hexdump_notice(in, len);
            bool is_first = lws_is_first_fragment(wsi);
            bool is_final = lws_is_final_fragment(wsi);
            WebsocketSession *ws=ws_find(pss->ws_id);
            if(ws) {
                ws->receiveData((const char*)in,len,is_first,is_final);
            } else {
                print("ws not found for pss->ws_id:%llx",pss->ws_id);
            }
		}
		break;

	case LWS_CALLBACK_CLOSED:
		lwsl_user("LWS_CALLBACK_CLOSED\n");
        //		lws_ring_destroy(pss->ring);

		if (*vhd->options & 1) {
			if (!*vhd->interrupted)
				*vhd->interrupted = 1 + pss->completed;
			lws_cancel_service(lws_get_context(wsi));
		}
        {
            WebsocketSession *ws = ws_find(pss->ws_id);
            if(ws) {
                if(g_on_close) g_on_close(ws->id);
                ws_clean(ws);
            }
        }
		break;

	default:
		break;
	}

	return 0;
}


#define LWS_PLUGIN_PROTOCOL_MINIMAL_SERVER_ECHO \
	{ \
		"lws-minimal-server-echo", \
		callback_minimal_server_echo, \
		sizeof(struct per_session_data__minimal_server_echo), \
		1024, \
		0, NULL, 0 \
	}



struct lws_context *g_context;

static struct lws_protocols protocols[] = {
	LWS_PLUGIN_PROTOCOL_MINIMAL_SERVER_ECHO,
	LWS_PROTOCOL_LIST_TERM
};


static lws_sorted_usec_list_t sul_stagger;

// to unblock ws_service()
static void stagger_cb(lws_sorted_usec_list_t *sul) {
    lws_sul_schedule(g_context,0,&sul_stagger, stagger_cb, 1 * LWS_US_PER_MS );    
}

void ws_start(uint16_t port) {
	int logs = LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE;
	lws_set_log_level(logs, NULL);

	struct lws_context_creation_info info;    
	memset(&info, 0, sizeof info); /* otherwise uninitialized garbage */
	info.port = port;
	info.protocols = protocols;
	info.pvo = &pvo;
    //	if (!lws_cmdline_option(argc, argv, "-n"))
    info.extensions = extensions;
	info.pt_serv_buf_size = 32 * 1024;
	info.options = LWS_SERVER_OPTION_VALIDATE_UTF8 |
		LWS_SERVER_OPTION_HTTP_HEADERS_SECURITY_BEST_PRACTICES_ENFORCE;

	g_context = lws_create_context(&info);
    assert(g_context);
    lws_sul_schedule(g_context,0,&sul_stagger, stagger_cb, 10 * LWS_US_PER_MS );
}

bool ws_service() {
    int n=lws_service(g_context,1);
    return n>=0;
}
void ws_end() {
	lws_context_destroy(g_context);        
}
void ws_init(uint32_t max_ccu) {
    g_max_ccu=max_ccu;
    size_t sz=sizeof(WebsocketSession*)*max_ccu;
    g_wses_list=(WebsocketSession**)malloc(sz);
    memset(g_wses_list,0,sz);
    g_wses_list_used=0;
}




