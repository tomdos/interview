#!/bin/bash

function pattern_test()
{
	pat="$1"
	in="$2"

	echo -n "'$pat' '$in' - "
	./match.pl "$pat" "$in"
}

echo "General input:"
pattern_test '^foo (.*) is a (.*)$' 'foo bla is a very big boat'
pattern_test '^foo (.*) is a (.*)$' 'foo bla is bar'
pattern_test '^foo (.*) is a (.*)$' 'foo blah'
pattern_test '^foo (.*) is a (.*)$' 'foo is'

echo
echo "Whitespaces:"
# no whitespace
pattern_test '^foo (.*) is a (\S*)$' 'foo bla is a very big boat'
# minimum is one space
pattern_test '^foo (.*) is a ((\S*\s\S*){1})' 'foo bla is a big bar blob'

# from pdf
pattern_test '^foo (.*) is a (\w)$' 'foo blah is a bar'
pattern_test '^foo (.*) is a (\S*)$' 'foo blah is a very big boat'
pattern_test '^foo (.*) is a (\S*)$' 'foo blah is a bar'
pattern_test '^foo (.*) is a (\S*)$' 'foo blah'
pattern_test '^foo (.*) is a (\S*)$' 'foo blah is'
