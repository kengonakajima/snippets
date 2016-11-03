CURL=curl -X POST http://localhost:8888 -H "Accept: application/json" -H "Content-Type: application/json"

test:
	$(CURL) -d '{ "command" : "send_message", "auth_uid" : 12345, "receiver" : 123123, "sender" : 232323, "message_id": "7777777778c25df92958c686673feb61", "message" : "hogehogehogehogehogehogehogehogehogehogehogehogehogehoge", "receiver_type" : "User" }'

repeat:
	watch make test


