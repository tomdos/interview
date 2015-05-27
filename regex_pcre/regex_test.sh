#!/bin/bash

# FIXME
#EXEC=regex
#if [ ! -x "$EXEC" ]; then
#  echo "Unable to find executable file."
#  exit 1
#fi

FAIL_COUNTER=0;

# Debug mode - run the program in verbose mode
if [ "X$1" == "X-d" ]; then
  DEBUG="1"
else
  DEBUG="0"
fi


function test_unit_regex()
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


function print_unit_result()
{
  echo "========================="
  if [ "$FAIL_COUNTER" == "0" ]; then
    echo "TEST SUCCEEDED"
  else
    echo "TES FAILED - errors: $FAIL_COUNTER"
  fi
  echo "========================="
}

function test_debug_regex()
{
  pattern=$1
  input=$2
  
  echo "========================================================"
  echo "'$pattern' =~ '$input'"
  
  echo "$input" | ./regex "$pattern"
}

function test_regex()
{
  match=$1
  pattern=$2
  input=$3
  
  if [ "$DEBUG" == "0" ]; then
    test_unit_regex "$match" "$pattern" "$input"
  else
    test_debug_regex "$pattern" "$input"
  fi
}

function print_result()
{
  [ "$DEBUG" == "0" ] && print_unit_result
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
test_regex 1 '%{0}' ""  # XXX ???
test_regex 1 'AX %{0} EX %{1}' "AX BX CX DX EX FX"
test_regex 1 'AX %{0} %{1}' "AX BX CX DX EX FX"
test_regex 1 'AX %{0}%{1}%{2}' "AX BX CX"
test_regex 1 '%{0}%{1}%{2}' "AXBXCX" # XXX

echo "My tests - spaces: "
test_regex 1 '%{1S0}' ""  # XXX ???
test_regex 1 '%{1S1}' " "  # XXX ???
test_regex 0 '%{1S0}' " AX "
test_regex 0 '%{1S1}' " AX "
test_regex 1 '%{1S2}' " AX "
test_regex 0 '%{1S3}' " AX "
test_regex 0 '%{1S0}' "AX BX CX"
test_regex 0 '%{1S1}' "AX BX CX"
test_regex 1 '%{1S2}' "AX BX CX"
test_regex 0 '%{1S3}' "AX BX CX"
test_regex 0 '%{1S2}%{1S2}' "AX BX CX DX"
test_regex 1 '%{1S0}%{1S2}%{1S0}' "AX BX CX"

echo "My tests - greedy: "
test_regex 1 '%{0G}' ""  # XXX ???
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
