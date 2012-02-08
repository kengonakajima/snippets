% comment


-module(sample).
-export([square/1,sum/1,make_rect/4,judge/1,reverse/1,ckrect/1,apply_all/2,head_of/1,tail_of/1,is_zero/1,ans2/3,hoger/1,send_and_receive/2,fib/1,calc_fibs/1,fib_send/2]).

square(X) -> X * X.

-record( rectangle, {x,y,w,h} ).

make_rect(X,Y,W,H) -> #rectangle {x=X,y=Y,w=W,h=H}.


sum(X) ->
 if
	X==0 ->  0;
	X/=0 -> X + sum(X-1)
 end.


judge(X) ->
	case X of
	[] -> "empty list";
	[A|_] -> { "nonempty list of head",A };
	{A} -> { "single element tuple",A };
	{A,B} ->  { "double element tuple",A,B };
	{A,B,C} ->  { "triple element tuple",A,B,C };
    _  -> "otherwise"
	end.


reverse(List) ->
	case List of
		[] -> [];
		[Head|Tail] -> reverse(Tail) ++ [Head]
	end.

ckrect(Rect) ->
	case Rect of
		{A,A} -> io:put_chars( "square\n" );
		{_,_} -> io:put_chars( "oblong\n" )
	end.


apply_all(List,Func) ->
	case List of
		[] -> [];
		[Head|Tail] -> [Func(Head) | apply_all(Tail,Func)]
	end.

% 以上は、以下のふたつのセットとおなじ

% apply_all( [], F ) -> [];   % セミコロン重要
% apply_all( [Head|Tail], F ) -> [F(Head)|apply_all(Tail,F) ].

	
head_of([H|_]) -> H.
tail_of(List) -> head_of(reverse(List)).



is_zero(0) -> true;     % セミコロン重要
is_zero(_) -> false.


% 二次方程式の解をもとめる

ans2(0,0,0) -> [0];
ans2(0,0,C) -> [];
ans2(0,B,C) -> [-C/B];

ans2(A,B,C) when B*B-4*A*C > 0 ->
	Rd = math:sqrt( B*B-4*A*C ),
	[(-B+Rd)/(2*A),(-B-Rd)/(2*A)];

ans2(A,B,C) when B*B-4*A*C == 0 ->
	[(-B)/(2*A)];

ans2(A,B,C) -> [].



hoger(P) ->
	receive X -> P ! { hoge, X } end,
	hoger(P).

send_and_receive(X,P) ->
	P ! X,
	receive R -> R end.

fib(0) -> 1;
fib(1) -> 1;
fib(N) -> io:format("Fn=(~w)", [N]), fib(N-1) + fib(N-2).


fib_send(P,N) -> P ! { N, fib(N) }. % 計算結果をプロセスPに送る版

calc_fibs_list([]) -> ok;
calc_fibs_list([N|List]) ->
	spawn( sample, fib_send, [self(),N] ),
	calc_fibs_list(List).

receive_results(0) -> complete;
receive_results(N) ->
	receive
		{X,Fx}->
			io:format("fib(~w) = ~w~n", [X,Fx] ),
			receive_results(N-1)
	after 1000->
		timeout
	end.


calc_fibs( List ) ->
	calc_fibs_list(List),
	receive_results( length(List) ).
