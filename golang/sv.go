package main
import (
    "rpc";
    "http";
    "log";
    "net";
    "os";
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
    arith := new(Arith);
    rpc.Register(arith);
    rpc.HandleHTTP();
    l, e := net.Listen("tcp", ":1234");
    if e != nil {
        log.Exit("listen error: ", e);
    }
    http.Serve(l, nil);
}
