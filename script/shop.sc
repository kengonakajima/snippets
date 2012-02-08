class shop
{
	func start( who, option, mode)
    {
		me.hp = 100 + who.hp
		me.state = "idle"
		me.item[-334].id = 445
    }
	func talked(who,what,mode,col)
    {
		syssys(100)
    }
}







/*  ˆÈ‰º‚Ì‚æ‚¤‚É‚È‚é‚Ì‚©‚È  */
void script_shop_start( value who, value option, value mode )
{
	SET_INT( INT(((character*)me)->hp), ADD_INT( INT(100), INT(((character*)who)->hp) ));
	SET_STRING( (character*)me)->state, "idle" );

    SET_INT( INT(((item*)((character*)me->item))[INDEX(INT(-334))]),INT(445));
}
int script_shop_talked( value who, value what, value mode, value col )
{
	CALL_FUNC( FUNC(syssys), INT(100));
}
