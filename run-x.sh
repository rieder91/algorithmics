#! /bin/bash
mkdir -p `pwd`/output/$1
for f in `cat files.txt`; do
    qsub -N `basename $f` -r y -e `pwd`/output/$1/`basename $f`.err -o `pwd`/output/$1/`basename $f`.out test_something.sh $1 $f;
done

