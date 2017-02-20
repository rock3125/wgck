#!/usr/bin/perl

# Tell the client what type of content you're going to send
print "Content-type: text/html\n\n";

# get environment string from shop
$key_gen_string = $ENV{'QUERY_STRING'};

# find &name= field in string
$name = "";
$email = "";
@list = split(/&/, $key_gen_string);

foreach $elt (@list) 
{
	@items = split(/=/,$elt);
	if ($items[0] eq "name") 
	{
	    $name = join('',split(/%20/,$items[1]));
	}
	if ($items[0] eq "email") 
	{
	    $email = join('',split(/%20/,$items[1]));
	}
}

# to uppercase
$email = uc $email;
$name = uc $name;

# check length
$code1Len = length($email);
if ( $code1Len > 35 )
{
	$email = substr( $email, 0, 35 );
}

# was $name . $email!!
$storeData = $email;
$total = 0;
$len = length($storeData);
$code = "";
for ( $i=0; $i < $len; $i++ )
{
	$letter = substr($storeData,$i,1);
	$ascii = ord $letter;
	$valid = 0;
	if ( $ascii >= 65 )
	{
		if ( $ascii <= 91 )
		{
			$valid = 1;
		}
	}
	if ( $ascii >= 48 )
	{
		if ( $ascii <= 58 )
		{
			$valid = 1;
		}
	}
	if ( $ascii == 46 )
	{
		$valid = 1;
	}
	if ( $ascii == 64 )
	{
		$valid = 1;
	}
	if ( $valid == 1 )
	{
		$total = $total + $ascii;
		$code = $code . chr($ascii);
	}
}

# encode the id string - this is the actual coding part
# encode 0..9 A..Z string into "secret code"
# . = 0x2e = 46
# @ = 0x40 = 64

$abc = "9BHJ0E-N45X2I76CVQ1TP+DKZOAG3WMSYFR8LU";
$code1Len = length($code);
$perm = 3;
$code2 = "";
for ( $i=0; $i < $code1Len; $i++ )
{
	$letter = substr($code,$i,1);
	$ascii = ord $letter;
	if ( $ascii >= 65 )
	{
		$ascii = $ascii - 65;
	}
	if ( $ascii >= 48 )
	{
		if ( $ascii <= 58 )
		{
			$ascii = ($ascii - 48) + 26;
		}
	}
	if ( $ascii == 46 )
	{
		$ascii = ($ascii - 46) + 36;
	}
	if ( $ascii == 64 )
	{
		$ascii = ($ascii - 64) + 37;
	}
	$ascii = $ascii + $perm;
	if ( $perm == 3 )
	{
		$perm = -3;
	}
	else
	{
		$perm = 3;
	}
	if ( $ascii < 0 )
	{
		$ascii = $ascii + 38;
	}
	if ( $ascii > 37 )
	{
		$ascii = $ascii - 38;
	}
	$code2 = $code2 . substr( $abc, $ascii, 1 );
}

# prefix code with hex length
$prefix = sprintf( "%02X", $code1Len );

#postfix code is hex checksum
$postfix = sprintf( "%04X", $total );

# finale code is all together
$code = $prefix . $code2 . $postfix;

# output html from keycode generator as required
print "<html><body><softshop>$code</softshop></body></html>"; 
