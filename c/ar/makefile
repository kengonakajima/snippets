OBJS=setup_client.o setup_sever.o

snet.a ${OBJS}
		rm -f snet.a
		ac rc snet.a setup_client.o setup_sever.o
		ranlib snet.a
		rm setup_client.o setup_sever.o
		
