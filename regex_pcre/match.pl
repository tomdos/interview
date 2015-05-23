#!/usr/bin/perl

#use strict;
#use warnings;

if ($#ARGV + 1 != 2) {
	print "Usage: match.pl pattern input\n";
	exit;
}

$pattern = $ARGV[0];
$input = $ARGV[1];
#$pattern = 'foo (\w+) is a (\w+)';

if ($input =~ /$pattern/) {
	print "found: $1 .. $2\n";
} else {
	print "not found\n";
}
