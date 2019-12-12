#!/usr/bin/perl

$notes = "usage: $0 inc comma-sep-list
adds inc to each element of list and prints to stdout
";

$inc = shift || die $notes;
@l = split ',', shift;
print join( ',', map { $_ + $inc } @l ) , "\n";
