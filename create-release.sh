#!/bin/sh

if [ -z "$1" ]
then
    echo "Usage:"
    echo "$0 <tree-ish>"
    exit 0
fi

git describe "$1" > /dev/null
if [ $? -ne 0 ]
then
    echo "Error: $1 is not a value <tree-ish> specifier."
    exit
fi

git archive --format=tar \
    "--prefix=agentXcpp-$1/" \
    "$1" \
    -o "agentXcpp-$1.tar"
if [ $? -ne 0 ]
then
    echo "Error: git was not able to create an archive."
    exit
fi
 
gzip "agentXcpp-$1.tar"
if [ $? -ne 0 ]
then
    echo "Error: gzip was not able to compress the archive."
    exit
fi

git archive --format=zip \
    "--prefix=agentXcpp-$1/" \
    "$1" \
    -o "agentXcpp-$1.zip"
if [ $? -ne 0 ]
then
    echo "Error."
    exit
fi
