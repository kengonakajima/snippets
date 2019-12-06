using System;
using UnityEngine;
using System.Net;
using System.Net.Sockets;

public class Mic : MonoBehaviour {
    AudioClip clip;

	Socket sk;

    void Start() {
		sk=new Socket(AddressFamily.InterNetwork,SocketType.Dgram,ProtocolType.Udp);
		sk.Connect(IPAddress.Parse("127.0.0.1"),22222);
		Debug.Log(sk);
        clip = Microphone.Start(null, true, 1, 4800); // 0.1sec
        while (Microphone.GetPosition(null) < 0) { }
    }

	void sendToServer(float[]samples, int si, int ei) {
		if(sk==null)return;
		if(si==ei)return;
		int bytelen = (ei-si)*sizeof(float);
		var byteArray = new byte[bytelen];
		Buffer.BlockCopy(samples,0,byteArray,0,bytelen);
		Debug.Log("sendtoserver:"+si+","+ei+","+bytelen);
		sk.BeginSend(byteArray,0,bytelen,SocketFlags.None,(ar)=>
		{
//			State so = (State)ar.AsyncState;
            int bytes = sk.EndSend(ar);
        	Debug.Log("SEND:"+bytes);
		},null);
	}
	int prevposition;
    void Update() {
        int position = Microphone.GetPosition(null);
		Debug.Log(""+prevposition+","+position);
	    float[] samples = new float[clip.samples*clip.channels];
        clip.GetData(samples, 0);
		if(prevposition>position) {
			//looped        [---*-------------*----]
			//             position      prevposition
			sendToServer(samples,prevposition,4800);
			sendToServer(samples,0,position);
		} else {
			// not looped   [---*-------------*----]
			//                  prevposition  position    
			sendToServer(samples,prevposition,position);
		}
		prevposition=position;

    }

}
