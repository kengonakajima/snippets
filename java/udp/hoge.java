
class CommandQueue {
	List<String> q;
	CommandQueue() {
		q=new ArrayList<String>();
	}
	synchronized public void append(String s) {		
		q.add(s);
	}
	synchronized public String fetch() {
		if(q.isEmpty()) return null;
		return q.remove(0);
	}
};


class MyThread extends Thread { 
	DatagramSocket sv;
	DatagramPacket packet;
	CommandQueue comq;
	MyThread(CommandQueue _comq){	
		comq=_comq;
		try {
			sv = new DatagramSocket(33333);
		} catch (SocketException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return;
		}
		byte[] b=new byte[1024];
		packet=new DatagramPacket(b,1024);

	}
	public void run() {
		while(true) {
			try {
				sv.receive(packet);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			byte[] data=packet.getData();
			String text = new String(data, 0, packet.getLength());
			comq.append(text);
			System.out.println(packet.getAddress().getHostAddress() + ":" + packet.getPort() + " received: '" + text + "'");
		}
	}
};
