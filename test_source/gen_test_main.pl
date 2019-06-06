#!/usr/bin/perl

printf("#include \"try.h\"\n");
printf("\n");
printf("void main(int argc, char **argv) {\n");

while (<>) {
    next unless m%^//\s%;
    chomp;

    my($comment, $try, $func, $expected) = split(/\s+/, $_, 4);

    # printf("// try=%s, func=%s, expected=%s\n", $try, $func, $expected);


    if ($try eq '@try_ret') {
        printf("extern int %s();\n", $func);
        printf("try_ret(%s, %s(), \"%s\");\n", $expected, $func, $func);
    } elsif ($try eq '@try_out') {
        unless ($expected =~ /".*"/) {
            $expected = '"'.$expected.'"';
        }

        printf("extern int %s();\n", $func);
        printf("try_out_start();\n");
        printf("%s();\n", $func);
        printf("try_out_check(%s, \"%s\");\n", $expected, $func);
    } else {
        # printf("// skip %s\n", $_);
    }

    printf("\n");
}

printf("printf(\"OK\\n\");\n");
printf("exit(0);\n");
printf("}\n");

