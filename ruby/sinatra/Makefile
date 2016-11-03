CURL=curl -X POST http://localhost:8888 -H "Accept: application/json" -H "Content-Type: application/json"

test:
	$(CURL) -d '{ "user_id" : 12345, "receiver" : 123123, "sender" : 232323, "message" : "hogehogehogehogehogehogehogehogehogehogehogehogehogehoge", "action" : "hoge" }'
repeat:
	watch make test


