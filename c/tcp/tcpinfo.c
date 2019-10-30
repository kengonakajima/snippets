#include <netinet/tcp.h>

#ifdef __APPLE__
void print_tcp_stat(int fd) {
    struct tcp_connection_info info;
    int length = sizeof(info);
    getsockopt(fd, IPPROTO_TCP, TCP_CONNECTION_INFO, (void *)&info, (socklen_t *)&length);
    fprintf(stderr, "TCP %u %u %u %u %u %u %u %u %u %u\n",
            info.tcpi_rcv_wnd,
            info.tcpi_rcv_wscale,
            info.tcpi_rto,
            info.tcpi_rttcur,
            info.tcpi_rttvar,
            info.tcpi_snd_cwnd,
            info.tcpi_snd_ssthresh,
            info.tcpi_snd_wnd,
            info.tcpi_snd_wscale,
            info.tcpi_srtt);
}
#endif
#ifdef __linux__
void print_tcp_stat(int fd) {
    struct tcp_info info;
    memset(&info,0,sizeof(info));
    int length = sizeof(info);
    getsockopt(fd, IPPROTO_TCP, TCP_INFO, (void *)&info, (socklen_t *)&length);
	fprintf(stderr,"TCP %u %u %u %u %u %u %u %u %u %u %u %u %u\n",
			info.tcpi_last_data_sent,
			info.tcpi_last_data_recv,
			info.tcpi_snd_cwnd,
			info.tcpi_snd_ssthresh,
			info.tcpi_rcv_ssthresh,
			info.tcpi_rtt,
			info.tcpi_rttvar,
			info.tcpi_unacked,
			info.tcpi_sacked,
			info.tcpi_lost,
			info.tcpi_retrans,
			info.tcpi_fackets,
            info.tcpi_reordering
		   );
    //    fprintf(stderr, "TCP sndw:%d rtt:%d rttvar:%d lost:%d retrans:%d unacked:%d\n",
    //            info.tcpi_snd_cwnd, info.tcpi_rtt, info.tcpi_rttvar, info.tcpi_lost, info.tcpi_retrans, info.tcpi_unacked );
}
#endif


