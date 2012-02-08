package main
import (
    "os";
    "rpc";
    "log";
    "fmt";
);

type Args struct {
    A int;
    B int;
};
type Reply struct {
    C int;
};
type Arith int;
func (p*Arith) Add(args *Args, reply *Reply) os.Error {
    reply.C = args.A + args.B;
    return nil;
}

func main() {
    client, err := rpc.DialHTTP("tcp", "localhost:1234");
    if err != nil {
        log.Exit("dialing:", err);
    }
    args := &Args{7,8};
    reply := new(Reply);
    err = client.Call("Arith.Add", args, reply);
    if err != nil {
        log.Exit("arith error:", err);
    }
    fmt.Printf("Arith: %d+%d=%d\n", args.A, args.B, reply.C);
}
