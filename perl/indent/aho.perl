#
# indent
#
# ���̤�ߤĤ���ȡ�
#
# 1 ����0����Ϥ���롣
# 2 ���ιԤ��ߤ��ࡣ
# 3 ���ιԤ���Ƭ��[ \t]+ ������������롣
# 4 ���ߤΥ�����ʬ�Υ��֤��������ƽ��ϡ�
# 4 ���ιԤ�1ʸ���ŤĤߤ롣 { ���դ��ޤ�Ƥ����顢1�ͥ��ȡ�}��ߤĤ�����-1��
# 5 2��
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
