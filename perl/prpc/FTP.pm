
package perlRPCP;

require 5.001;

#use strict;

use Carp;

use Socket 1.3;
use IO::Socket;
use Time::Local;
use Net::Cmd;
use Net::Config;


@ISA     = qw(Exporter Net::Cmd IO::Socket::INET);



sub new
{
 my $pkg  = shift;
 my $peer = shift;
 my %arg  = @_; 

 my $host = $peer;
 my $fire = undef;

 # Should I use Net::Ping here ?? --GMB
 if(exists($arg{Firewall}) || !defined(inet_aton($peer)))
  {
   $fire = $arg{Firewall}
	|| $ENV{FTP_FIREWALL}
	|| $NetConfig{ftp_firewall}
	|| undef;

   if(defined $fire)
    {
     $peer = $fire;
     delete $arg{Port};
    }
  }

 my $ftp = $pkg->SUPER::new(PeerAddr => $peer, 
			    PeerPort => $arg{Port} || 'ftp(21)',
			    Proto    => 'tcp',
			    Timeout  => defined $arg{Timeout}
						? $arg{Timeout}
						: 120
			   ) or return undef;

 ${*$ftp}{'net_ftp_host'}     = $host;		# Remote hostname
 ${*$ftp}{'net_ftp_type'}     = 'A';		# ASCII/binary/etc mode

 ${*$ftp}{'net_ftp_firewall'} = $fire
	if(defined $fire);

 ${*$ftp}{'net_ftp_passive'} = int
	exists $arg{Passive}
	    ? $arg{Passive}
	    : exists $ENV{FTP_PASSIVE}
		? $ENV{FTP_PASSIVE}
		: defined $fire
		    ? $NetConfig{ftp_ext_passive}
		    : $NetConfig{ftp_int_passive};	# Whew! :-)

 $ftp->autoflush(1);

 $ftp->debug(exists $arg{Debug} ? $arg{Debug} : undef);

 unless ($ftp->response() == CMD_OK)
  {
   $ftp->close();
   $@ = $ftp->message;
   undef $ftp;
  }

 $ftp;
}

##
## User interface methods
##

sub quit
{
 my $ftp = shift;

 $ftp->_QUIT;
 $ftp->close;
}

sub DESTROY
{
 my $ftp = shift;
 defined(fileno($ftp)) && $ftp->quit
}

sub ascii  { shift->type('A',@_); }
sub binary { shift->type('I',@_); }

sub ebcdic
{
 carp "TYPE E is unsupported, shall default to I";
 shift->type('E',@_);
}

sub byte
{
 carp "TYPE L is unsupported, shall default to I";
 shift->type('L',@_);
}

# Allow the user to send a command directly, BE CAREFUL !!

sub quot
{ 
 my $ftp = shift;
 my $cmd = shift;

 $ftp->command( uc $cmd, @_);
 $ftp->response();
}

sub mdtm
{
 my $ftp  = shift;
 my $file = shift;

 $ftp->_MDTM($file) && $ftp->message =~ /(\d{4})(\d\d)(\d\d)(\d\d)(\d\d)(\d\d)/
    ? timegm($6,$5,$4,$3,$2-1,$1 - 1900)
    : undef;
}

sub size
{
 my $ftp  = shift;
 my $file = shift;

 $ftp->_SIZE($file)
	? ($ftp->message =~ /(\d+)/)[0]
	: undef;
}

sub login
{
 my($ftp,$user,$pass,$acct) = @_;
 my($ok,$ruser);

 unless (defined $user)
  {
   require Net::Netrc;

   my $rc = Net::Netrc->lookup(${*$ftp}{'net_ftp_host'});

   ($user,$pass,$acct) = $rc->lpa()
	if ($rc);
  }

 $user ||= "anonymous";
 $ruser = $user;

 if(defined ${*$ftp}{'net_ftp_firewall'})
  {
   $user .= "@" . ${*$ftp}{'net_ftp_host'};
  }

 $ok = $ftp->_USER($user);

 # Some dumb firewalls don't prefix the connection messages
 $ok = $ftp->response()
	if($ok == CMD_OK && $ftp->code == 220 && $user =~ /\@/);

 if ($ok == CMD_MORE)
  {
   unless(defined $pass)
    {
     require Net::Netrc;

     my $rc = Net::Netrc->lookup(${*$ftp}{'net_ftp_host'}, $ruser);

     ($ruser,$pass,$acct) = $rc->lpa()
	if ($rc);

     $pass = eval { "-" . (getpwuid($>))[0] . "@" }
        if (!defined $pass && (!defined($ruser) || $ruser =~ /^anonymous/o));
    }

   $ok = $ftp->_PASS($pass || "");
  }

 $ok = $ftp->_ACCT($acct || "")
	if ($ok == CMD_MORE);

 $ftp->authorize()
    if($ok == CMD_OK && defined ${*$ftp}{'net_ftp_firewall'});

 $ok == CMD_OK;
}

sub account
{
 @_ == 2 or croak 'usage: $ftp->account( ACCT )';
 my $ftp = shift;
 my $acct = shift;
 $ftp->_ACCT($acct) == CMD_OK;
}

sub authorize
{
 @_ >= 1 || @_ <= 3 or croak 'usage: $ftp->authorize( [AUTH [, RESP]])';

 my($ftp,$auth,$resp) = @_;

 unless(defined $resp)
  {
   require Net::Netrc;

   $auth ||= eval { (getpwuid($>))[0] };

   my $rc = Net::Netrc->lookup(${*$ftp}{'net_ftp_firewall'}, $auth)
        || Net::Netrc->lookup(${*$ftp}{'net_ftp_firewall'});

   ($auth,$resp) = $rc->lpa()
     if($rc);
  }

 my $ok = $ftp->_AUTH($auth || "");

 $ok = $ftp->_RESP($resp || "")
	if ($ok == CMD_MORE);

 $ok == CMD_OK;
}

sub rename
{
 @_ == 3 or croak 'usage: $ftp->rename(FROM, TO)';

 my($ftp,$from,$to) = @_;

 $ftp->_RNFR($from)
    && $ftp->_RNTO($to);
}

sub type
{
 my $ftp = shift;
 my $type = shift;
 my $oldval = ${*$ftp}{'net_ftp_type'};

 return $oldval
	unless (defined $type);

 return undef
	unless ($ftp->_TYPE($type,@_));

 ${*$ftp}{'net_ftp_type'} = join(" ",$type,@_);

 $oldval;
}

sub abort
{
 my $ftp = shift;

 send($ftp,pack("CC",$TELNET_IAC,$TELNET_IP),0);
 send($ftp,pack("CC", $TELNET_IAC, $TELNET_DM),MSG_OOB);

 $ftp->command("ABOR");

# defined ${*$ftp}{'net_ftp_dataconn'}
#    ? ${*$ftp}{'net_ftp_dataconn'}->close()
#    : $ftp->response();
 
 ${*$ftp}{'net_ftp_dataconn'}->close()
    if defined ${*$ftp}{'net_ftp_dataconn'};

 $ftp->response();
#    if $ftp->status == CMD_REJECT;

 $ftp->status == CMD_OK;
}

sub get
{
 my($ftp,$remote,$local,$where) = @_;

 my($loc,$len,$buf,$resp,$localfd,$data);
 local *FD;

 $localfd = ref($local) ? fileno($local)
			: undef;

 ($local = $remote) =~ s#^.*/##
	unless(defined $local);

 ${*$ftp}{'net_ftp_rest'} = $where
	if ($where);

 delete ${*$ftp}{'net_ftp_port'};
 delete ${*$ftp}{'net_ftp_pasv'};

 $data = $ftp->retr($remote) or
	return undef;

 if(defined $localfd)
  {
   $loc = $local;
  }
 else
  {
   $loc = \*FD;

   unless(($where) ? open($loc,">>$local") : open($loc,">$local"))
    {
     carp "Cannot open Local file $local: $!\n";
     $data->abort;
     return undef;
    }
  }

 if($ftp->type eq 'I' && !binmode($loc))
  {
   carp "Cannot binmode Local file $local: $!\n";
   $data->abort;
   return undef;
  }

 $buf = '';

 do
  {
   $len = $data->read($buf,1024);
  }
 while($len > 0 && syswrite($loc,$buf,$len) == $len);

 close($loc)
	unless defined $localfd;
 
 $data->close(); # implied $ftp->response

 return $local;
}

sub cwd
{
 @_ == 2 || @_ == 3 or croak 'usage: $ftp->cwd( [ DIR ] )';

 my($ftp,$dir) = @_;

 $dir ||= "/";

 $dir eq ".."
    ? $ftp->_CDUP()
    : $ftp->_CWD($dir);
}

sub cdup
{
 @_ == 1 or croak 'usage: $ftp->cdup()';
 $_[0]->_CDUP;
}

sub pwd
{
 @_ == 1 || croak 'usage: $ftp->pwd()';
 my $ftp = shift;

 $ftp->_PWD();
 $ftp->_extract_path;
}

sub rmdir
{
 @_ == 2 || croak 'usage: $ftp->rmdir( DIR )';

 $_[0]->_RMD($_[1]);
}

sub mkdir
{
 @_ == 2 || @_ == 3 or croak 'usage: $ftp->mkdir( DIR [, RECURSE ] )';

 my($ftp,$dir,$recurse) = @_;

 $ftp->_MKD($dir) || $recurse or
    return undef;

 my $path = $dir;

 unless($ftp->ok)
  {
   my @path = split(m#(?=/+)#, $dir);

   $path = "";

   while(@path)
    {
     $path .= shift @path;

     $ftp->_MKD($path);
     $path = $ftp->_extract_path($path);

     # 521 means directory already exists
     last
        unless $ftp->ok || $ftp->code == 521 || $ftp->code == 550;
    }
  }

 $ftp->_extract_path($path);
}

sub delete
{
 @_ == 2 || croak 'usage: $ftp->delete( FILENAME )';

 $_[0]->_DELE($_[1]);
}

sub put        { shift->_store_cmd("stor",@_) }
sub put_unique { shift->_store_cmd("stou",@_) }
sub append     { shift->_store_cmd("appe",@_) }

sub nlst { shift->_data_cmd("NLST",@_) }
sub list { shift->_data_cmd("LIST",@_) }
sub retr { shift->_data_cmd("RETR",@_) }
sub stor { shift->_data_cmd("STOR",@_) }
sub stou { shift->_data_cmd("STOU",@_) }
sub appe { shift->_data_cmd("APPE",@_) }

sub _store_cmd 
{
 my($ftp,$cmd,$local,$remote) = @_;
 my($loc,$sock,$len,$buf,$localfd);
 local *FD;

 $localfd = ref($local) ? fileno($local)
			: undef;

 unless(defined $remote)
  {
   croak 'Must specify remote filename with stream input'
	if defined $localfd;

   ($remote = $local) =~ s%.*/%%;
  }

 if(defined $localfd)
  {
   $loc = $local;
  }
 else
  {
   $loc = \*FD;

   unless(open($loc,"<$local"))
    {
     carp "Cannot open Local file $local: $!\n";
     return undef;
    }
  }

 if($ftp->type eq 'I' && !binmode($loc))
  {
   carp "Cannot binmode Local file $local: $!\n";
   return undef;
  }

 delete ${*$ftp}{'net_ftp_port'};
 delete ${*$ftp}{'net_ftp_pasv'};

 $sock = $ftp->_data_cmd($cmd, $remote) or 
	return undef;

 while(1)
  {
   last unless $len = sysread($loc,$buf="",1024);

   unless($sock->write($buf,$len) == $len)
    {
     $sock->abort;
     close($loc)
	unless defined $localfd;
     return undef;
    }
  }

 $sock->close();

 close($loc)
	unless defined $localfd;

 ($remote) = $ftp->message =~ /unique file name:\s*(\S*)\s*\)/
	if ('STOU' eq uc $cmd);

 return $remote;
}

sub port
{
 @_ == 1 || @_ == 2 or croak 'usage: $ftp->port([PORT])';

 my($ftp,$port) = @_;
 my $ok;

 delete ${*$ftp}{'net_ftp_intern_port'};

 unless(defined $port)
  {
   # create a Listen socket at same address as the command socket

   ${*$ftp}{'net_ftp_listen'} ||= IO::Socket::INET->new(Listen    => 5,
				    	    	        Proto     => 'tcp',
				    	    	        LocalAddr => $ftp->sockhost, 
				    	    	       );
  
   my $listen = ${*$ftp}{'net_ftp_listen'};

   my($myport, @myaddr) = ($listen->sockport, split(/\./,$listen->sockhost));

   $port = join(',', @myaddr, $myport >> 8, $myport & 0xff);

   ${*$ftp}{'net_ftp_intern_port'} = 1;
  }

 $ok = $ftp->_PORT($port);

 ${*$ftp}{'net_ftp_port'} = $port;

 $ok;
}

sub ls  { shift->_list_cmd("NLST",@_); }
sub dir { shift->_list_cmd("LIST",@_); }

sub pasv
{
 @_ == 1 or croak 'usage: $ftp->pasv()';

 my $ftp = shift;

 delete ${*$ftp}{'net_ftp_intern_port'};

 $ftp->_PASV && $ftp->message =~ /(\d+(,\d+)+)/
    ? ${*$ftp}{'net_ftp_pasv'} = $1
    : undef;    
}

sub unique_name
{
 my $ftp = shift;
 ${*$ftp}{'net_ftp_unique'} || undef;
}

sub supported {
    @_ == 2 or croak 'usage: $ftp->supported( CMD )';
    my $ftp = shift;
    my $cmd = uc shift;
    my $hash = ${*$ftp}{'net_ftp_supported'} ||= {};

    return $hash->{$cmd}
        if exists $hash->{$cmd};

    my $ok = $ftp->_HELP($cmd) &&
        $ftp->message !~ /unimplemented/i;

    $hash->{$cmd} = $ok;
}

##
## Depreciated methods
##

sub lsl
{
 carp "Use of Net::FTP::lsl depreciated, use 'dir'"
    if $^W;
 goto &dir;
}

sub authorise
{
 carp "Use of Net::FTP::authorise depreciated, use 'authorize'"
    if $^W;
 goto &authorize;
}


##
## Private methods
##

sub _extract_path
{
 my($ftp, $path) = @_;

 # This tries to work both with and without the quote doubling
 # convention (RFC 959 requires it, but the first 3 servers I checked
 # didn't implement it).  It will fail on a server which uses a quote in
 # the message which isn't a part of or surrounding the path.
 $ftp->ok &&
    $ftp->message =~ /(?:^|\s)\"(.*)\"(?:$|\s)/ &&
    ($path = $1) =~ s/\"\"/\"/g;

 $path;
}

##
## Communication methods
##

sub _dataconn
{
 my $ftp = shift;
 my $data = undef;
 my $pkg = "Net::FTP::" . $ftp->type;

 eval "require " . $pkg;

 $pkg =~ s/ /_/g;

 delete ${*$ftp}{'net_ftp_dataconn'};

 if(defined ${*$ftp}{'net_ftp_pasv'})
  {
   my @port = split(/,/,${*$ftp}{'net_ftp_pasv'});

   $data = $pkg->new(PeerAddr => join(".",@port[0..3]),
    	    	     PeerPort => $port[4] * 256 + $port[5],
    	    	     Proto    => 'tcp'
    	    	    );
  }
 elsif(defined ${*$ftp}{'net_ftp_listen'})
  {
   $data = ${*$ftp}{'net_ftp_listen'}->accept($pkg);
   close(delete ${*$ftp}{'net_ftp_listen'});
  }

 if($data)
  {
   ${*$data} = "";
   $data->timeout($ftp->timeout);
   ${*$ftp}{'net_ftp_dataconn'} = $data;
   ${*$data}{'net_ftp_cmd'} = $ftp;
  }

 $data;
}

sub _list_cmd
{
 my $ftp = shift;
 my $cmd = uc shift;

 delete ${*$ftp}{'net_ftp_port'};
 delete ${*$ftp}{'net_ftp_pasv'};

 my $data = $ftp->_data_cmd($cmd,@_);

 return undef
	unless(defined $data);

 require Net::FTP::A;
 bless $data, "Net::FTP::A"; # Force ASCII mode

 my $databuf = '';
 my $buf = '';

 while($data->read($databuf,1024))
  {
   $buf .= $databuf;
  }

 my $list = [ split(/\n/,$buf) ];

 $data->close();

 wantarray ? @{$list}
           : $list;
}

sub _data_cmd
{
 my $ftp = shift;
 my $cmd = uc shift;
 my $ok = 1;
 my $where = delete ${*$ftp}{'net_ftp_rest'} || 0;

 if(${*$ftp}{'net_ftp_passive'} &&
     !defined ${*$ftp}{'net_ftp_pasv'} &&
     !defined ${*$ftp}{'net_ftp_port'})
  {
   my $data = undef;

   $ok = defined $ftp->pasv;
   $ok = $ftp->_REST($where)
	if $ok && $where;

   if($ok)
    {
     $ftp->command($cmd,@_);
     $data = $ftp->_dataconn();
     $ok = CMD_INFO == $ftp->response();
     if($ok) 
      {
       $data->reading
         if $data && $cmd =~ /RETR|LIST|NLST/;
       return $data
      }
     $data->_close
	if $data;
    }
   return undef;
  }

 $ok = $ftp->port
    unless (defined ${*$ftp}{'net_ftp_port'} ||
            defined ${*$ftp}{'net_ftp_pasv'});

 $ok = $ftp->_REST($where)
    if $ok && $where;

 return undef
    unless $ok;

 $ftp->command($cmd,@_);

 return 1
    if(defined ${*$ftp}{'net_ftp_pasv'});

 $ok = CMD_INFO == $ftp->response();

 return $ok 
    unless exists ${*$ftp}{'net_ftp_intern_port'};

 return $ftp->_dataconn()
	if $ok;

 close(delete ${*$ftp}{'net_ftp_listen'});
 
 return undef;
}

##
## Over-ride methods (Net::Cmd)
##

sub debug_text { $_[2] =~ /^(pass|resp)/i ? "$1 ....\n" : $_[2]; }

sub command
{
 my $ftp = shift;

 delete ${*$ftp}{'net_ftp_port'};
 $ftp->SUPER::command(@_);
}

sub response
{
 my $ftp = shift;
 my $code = $ftp->SUPER::response();

 delete ${*$ftp}{'net_ftp_pasv'}
    if ($code != CMD_MORE && $code != CMD_INFO);

 $code;
}

sub parse_response
{
 return ($1, $2 eq "-")
    if $_[1] =~ s/^(\d\d\d)(.?)//o;

 my $ftp = shift;

 # Darn MS FTP server is a load of CRAP !!!!
 return ()
	unless ${*$ftp}{'net_cmd_code'} + 0;

 (${*$ftp}{'net_cmd_code'},1);
}

##
## Allow 2 servers to talk directly
##

sub pasv_xfer {
    my($sftp,$sfile,$dftp,$dfile,$unique) = @_;

    ($dfile = $sfile) =~ s#.*/##
	unless(defined $dfile);

    my $port = $sftp->pasv or
	return undef;

    $dftp->port($port) or
	return undef;

    return undef
	unless($unique ? $dftp->stou($dfile) : $dftp->stor($dfile));

    unless($sftp->retr($sfile) && $sftp->response == CMD_INFO) {
	$sftp->retr($sfile);
	$dftp->abort;
	$dftp->response();
	return undef;
    }

    $dftp->pasv_wait($sftp);
}

sub pasv_wait
{
 @_ == 2 or croak 'usage: $ftp->pasv_wait(NON_PASV_FTP)';

 my($ftp, $non_pasv) = @_;
 my($file,$rin,$rout);

 vec($rin,fileno($ftp),1) = 1;
 select($rout=$rin, undef, undef, undef);

 $ftp->response();
 $non_pasv->response();

 return undef
	unless $ftp->ok() && $non_pasv->ok();

 return $1
	if $ftp->message =~ /unique file name:\s*(\S*)\s*\)/;

 return $1
	if $non_pasv->message =~ /unique file name:\s*(\S*)\s*\)/;

 return 1;
}


