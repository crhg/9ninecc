#!/usr/bin/perl
use strict;
use warnings;

my @puncts;
while (<>) {
    chomp;

    my($punct, $token) = split;
    push @puncts, {punct => $punct, token => $token};
}

foreach (sort {length($a->{punct}) cmp length($b->{punct}) || $a->{'punct'} cmp $b->{'punct'}} @puncts) {
    if (length($_->{punct}) == 1) {
        printf "    TK_%s = '%s',\n", $_->{token}, $_->{punct};
    } else {
        printf "    TK_%s, // %s\n", $_->{token}, $_->{punct};
    }
}
