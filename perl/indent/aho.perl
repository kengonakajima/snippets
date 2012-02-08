#
# indent
#
# 中括弧をみつけると、
#
# 1 タブ0からはじめる。
# 2 次の行をよみこむ。
# 3 その行の先頭の[ \t]+ を全部削除する。
# 4 現在のタブ値分のタブを挿入して出力。
# 4 その行を1文字づつみる。 { がふくまれていたら、1ネスト。}をみつけたら-1。
# 5 2へ
#

open( IN , "<$ARGV[0]") || die "fuckednasdf\n";

@fuck = <IN>;
$hoge = join( "",@fuck );


print cIndent( 4 , $hoge );

sub cIndent($$)
{
	my $tabdep = shift;
	my $src = shift;
	my $out;
	my $t = 0;
	my $i;

	$src =~ s/\{/\n\{/g;
	my @lines = split( /\n/ , $src );
					
	foreach $i ( @lines ){
		my $tt = " " x ( $t * $tabdep );
		
		$i =~ s/^[ \t]+//;
		next if( $i =~ /^\s*$/ );

		my $hoge;
		$t += ( $i =~ /\{/g );
		$t -= ( $hoge = ( $i =~ /\}/g ) );

		$tt = " " x ( $t * $tabdep ) if( $hoge );
		
		$out .= "$tt$i\n";		
	}
	return $out;
}
