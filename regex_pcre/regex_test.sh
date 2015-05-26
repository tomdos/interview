#!/bin/bash

# FIXME
#EXEC=regex
#if [ ! -x "$EXEC" ]; then
#  echo "Unable to find executable file."
#  exit 1
#fi

FAIL_COUNTER=0;

function test_regex()
{
  match=$1    # expectation - 1-matched, 0-not
  pattern=$2
  input=$3
  
  ret=`echo "$input" | ./regex "$pattern"` 
  
  # Output of test - OK/FAIL
  if ([ "$match" == "0" ] && [ "$ret" == "" ]) || ([ "$match" == "1" ] && [ "$ret" == "$input" ]); then
    echo -n "O  "
  else
    echo -n "F  "
    FAIL_COUNTER=$(($FAIL_COUNTER + 1))
  fi

  # Output of the program - True (pattern and input matched) or False
  if [ "$ret" == "$input" ]; then
    echo -n "T  "
  else
    echo -n "F  "
  fi

  echo "'$input' =~ '$pattern'"
}

function print_result()
{
  echo "========================="
  if [ "$FAIL_COUNTER" == "0" ]; then
    echo "TEST SUCCEEDED"
  else
    echo "TES FAILED - errors: $FAIL_COUNTER"
  fi
  echo "========================="
}


###### Examples from assignment 
# General pattern
test_regex 1 'foo %{0} is a %{1}' "foo blah is a bar"
test_regex 1 'foo %{0} is a %{1}' "foo blah is a very big boat"
test_regex 0 'foo %{0} is a %{1}' "foo blah is bar"
test_regex 0 'foo %{0} is a %{1}' "foo blah"
test_regex 0 'foo %{0} is a %{1}' "foo blah is"

# Space limitation modifier
test_regex 1 'foo %{0} is a %{1S0}' "foo blah is a bar"
test_regex 0 'foo %{0} is a %{1S0}' "foo blah is a very big boat"
test_regex 0 'foo %{0} is a %{1S0}' "foo blah is bar"
test_regex 0 'foo %{0} is a %{1S0}' "foo blah"
test_regex 0 'foo %{0} is a %{1S0}' "foo blah is"
test_regex 1 'the %{0S1} %{1} ran away' "the big brown fox ran away"

# Greedy
test_regex 1 'bar %{0G} foo %{1}' "bar foo bar foo bar foo bar foo"


###### My tests
echo "My tests - general: "
test_regex 1 'AX %{0} EX %{1}' "AX BX CX DX EX FX"
test_regex 1 'AX %{0} %{1}' "AX BX CX DX EX FX"
test_regex 1 'AX %{0}%{1}%{2}' "AX BX CX"
test_regex 1 '%{0}%{1}%{2}' "AXBXCX" # XXX

echo "My tests - spaces: "
test_regex 0 '%{1S0}' " AX "
test_regex 0 '%{1S1}' " AX "
test_regex 1 '%{1S2}' " AX "
test_regex 0 '%{1S3}' " AX "
test_regex 0 '%{1S0}' "AX BX CX"
test_regex 0 '%{1S1}' "AX BX CX"
test_regex 1 '%{1S2}' "AX BX CX"
test_regex 0 '%{1S3}' "AX BX CX"

echo "My tests - greedy: "
test_regex 1 '%{0G}' "AXBXCX"
test_regex 1 'A%{0G}X' "AXBXCX"
test_regex 0 'A%{0G} X' "AXBXCX"
test_regex 0 'AXBXCX%{0G}' "AXBXCX" # XXX should probably fail

echo "My tests - mix: "
test_regex 0 'AXBXC%{0G}%{1}' "AXBXCX" # XXX should probably fail
test_regex 1 '%{0G}%{1S5}%{2}' "AX BX CX DX EX FX"
test_regex 1 '%{0G}%{1S0}%{2}' "AX BX CX DX EX FX"
test_regex 0 '%{0G}%{1S6}%{2}' "AX BX CX DX EX FX"


print_result
