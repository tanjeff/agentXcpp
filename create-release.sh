#!/bin/sh

if [ -z "$1" ]
then
    echo "Usage:"
    echo "$0 <tree-ish>"
fi


git archive --format=tar "--prefix=agentXcpp-$1/" "$1" \
    | gzip > "agentXcpp-$1.tar.gz"

if [ $? -ne 0 ]
then
    echo "Error."
    exit
fi

git archive --format=zip "--prefix=agentXcpp-$1/" "$1" \
    -o "agentXcpp-$1.zip"

if [ $? -ne 0 ]
then
    echo "Error."
    exit
fi
