extends Node

var capture : AudioEffectCapture
var udp := PacketPeerUDP.new()

func _ready():
	print("INPUT DEVICES:", AudioServer.get_input_device_list())
	print("MIX RATE:", AudioServer.get_mix_rate())
	udp.connect_to_host("127.0.0.1", 22222)
	AudioServer.add_bus()
	var idx := AudioServer.bus_count - 1
	AudioServer.set_bus_name(idx, "Record")
	capture = AudioEffectCapture.new()
	AudioServer.add_bus_effect(idx, capture)
	AudioServer.set_bus_mute(idx, true)
	var p := AudioStreamPlayer.new()
	p.stream = AudioStreamMicrophone.new()
	p.bus = "Record"
	add_child(p)
	p.play()

func _process(_dt):
	if capture == null:
		return
	var n := capture.get_frames_available()
	if n > 0:
		var frames := capture.get_buffer(n)
		var buf := PackedFloat32Array()
		buf.resize(frames.size())
		for i in frames.size():
			buf[i] = frames[i].x
		udp.put_packet(buf.to_byte_array())
