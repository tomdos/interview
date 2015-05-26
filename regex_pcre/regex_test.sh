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
  match=$1
  pattern=$2
  input=$3
  
  #echo "$input" | ./regex "$pattern"
  
  ret=`echo "$input" | ./regex "$pattern"` 
  
  if ([ "$match" == "0" ] && [ "$ret" == "" ]) || ([ "$match" == "1" ] && [ "$ret" == "$input" ]); then
    echo -n "O  "
  else
    echo -n "F  "
    FAIL_COUNTER=$(($FAIL_COUNTER + 1))
  fi

  if [ "$ret" == "$input" ]; then
    echo -n "==  "
  else
    echo -n "!!  "
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
echo "My tests: "






print_result
