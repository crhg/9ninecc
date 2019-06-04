#!/usr/bin/perl
#
# test.shをテスト用Cコードに変換する
# 1回しか使わないのである程度手作業前提で雑にやる
# 
# * 変数名や関数名の衝突は手で直す
#
use strict;

my @tests;
my $seq = 0;
while (<>) {
    chomp;

    next unless /^try\w* /;

    my($type, $expect, $code) = split(/\s+/, $_, 3);

    my $quot = substr($code, 0, 1);

    $code =~ /$quot(.*)$quot/ or die;
    $code = $1;

    # 対象コードを整形
    # * '{', ';', '}'の後に改行を追加する
    # * インデントはあとでclang-formatかけるから気にしない
    $code =~ s/[\{\;\}]/$&\n/g;

    $code =~ s/printf/try_printf/g;

    if ($type eq 'try') {
        printf("// \@try_ret test%d %s\n", $seq, $expect);
        printf("int test%d() {\n", $seq);
        print $code;
        printf("}\n");
    } elsif ($type eq 'try_output') {
        printf("// \@try_out test%d %s\n", $seq, $expect);
        printf("int test%d() {\n", $seq);
        print $code;
        printf("}\n");
    } elsif ($type eq 'try_output_raw') {
        printf("// \@try_out test%d %s\n", $seq, $expect);
        $code =~ s/main\(\)/test$seq()/;
        print $code;
    }
    printf("// \@end\n");
    printf("\n");

    push @tests, {type=>$type, expect=>$expect, code=>$code};
    $seq++;
}
