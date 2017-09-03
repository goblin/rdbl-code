#! /usr/bin/env perl

use strict;
use warnings;

use Data::Dumper;
use List::Util qw/sum/;
use Algorithm::Huffman;

my %hash;

read_words();
calculate_freqs();

huffmanify_hash();

dump_hash();

sub read_words {
	while(<>) {
		chomp;
		process_word($_."_");
	}
}

sub process_word {
	my $word = shift;

	my $prefix = '__';

	while(length($word) > 0) {
		$word =~ s/^(.)//;
		my $let = $1;

		if(exists($hash{$prefix}->{$let})) {
			$hash{$prefix}->{$let}++;
		} else {
			$hash{$prefix}->{$let} = 1;
		}

		$prefix =~ s/^.//;
		$prefix .= $let;
	}
}

sub calculate_freqs {
	foreach my $prefix (sort keys %hash) {
		my $letters = $hash{$prefix};
		my @order = sort { 
			$letters->{$b} <=> $letters->{$a}
		} (keys %$letters);
		my $endable = exists($letters->{_}) ? 1 : 0;
		$hash{$prefix} = { 
			letters => \@order, 
			freqs => [ map { $letters->{$_} } @order ], 
			endable => $endable
		};
	}
}

sub getword {
	my $prefix = '__';
	my $word = '';

	while($prefix !~ /[A-Z]_/) {
		my $data = $hash{$prefix};

		my $total = sum(@{$data->{freqs}});
		my $num = int(rand($total));

		my $let;

		for(my $i = 0; $i < scalar(@{$data->{freqs}}); $i++) {
			my $cur = $data->{freqs}->[$i];
			$num -= $cur;
			if($num < 0) {
				$let = $data->{letters}->[$i];
				$word .= $let;
				last;
			}
		}

		die "??" unless defined($let);

		last if(length($word) > 7 && $data->{endable});

		$prefix =~ s/^.//;
		$prefix .= $let;
	}

	$word =~ s/_$//;

	return $word;
}

sub dump_hash {
	foreach my $prefix (sort keys %hash) {
		my $data = $hash{$prefix};
		my $huffhash = $data->{encoding};
		my $elems = join(', ', map {
					my $bits = join(',', split(//, $data->{encoding}->{$_}));
					my $c = lc $_;
					"{'$c',{$bits}}"
				} keys %$huffhash);
		my $lcp = lc $prefix;
		if(!$elems) {
			# we'll get it empty when there's just one forced element
			my $forced_letter = lc $data->{letters}->[0];
			$elems = "'$forced_letter'";
		}
		print "{ \"$lcp\", { $elems } },\n";
	}
}

sub huffmanify_hash {
	foreach my $prefix (keys %hash) {
		($hash{$prefix}->{encoding}, $hash{$prefix}->{decoding}) = 
			get_huff_hash($hash{$prefix}->{letters}, $hash{$prefix}->{freqs});
	}
}

sub get_huff_hash {
	my ($letters, $freqs) = @_;
	
	return undef if($#$letters == 0);

	my %map = map { $letters->[$_] => $freqs->[$_] } (0 .. $#$letters);
	my $huff = Algorithm::Huffman->new(\%map);
	return ($huff->encode_hash, $huff->decode_hash);
}
