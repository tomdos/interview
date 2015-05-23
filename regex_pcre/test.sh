#!/bin/bash

function pattern_test()
{
	pat="$1"
	in="$2"

	echo -n "'$pat' '$in' - "
	./match.pl "$pat" "$in"
}

echo "General input:"
pattern_test 'foo (.*) is a (.*)' 'foo bla is a very big boat'
pattern_test 'foo (.*) is a (.*)' 'foo bla is bar'
pattern_test 'foo (.*) is a (.*)' 'foo blah'
pattern_test 'foo (.*) is a (.*)' 'foo is'

