#!/bin/sh
#Script for grading submissions on the provided traces
#Assumes that submission is placed in a folder name, which is provided
#as an argument to the script 

## check we have an argument
if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    exit 1
fi

## team folder provided as argument 
#TEAM=Team1
TEAM=$1

## compile the simulator files within team directory
#gcc -std=gnu99 -o ./$TEAM/cpu ./$TEAM/five_stage.c -I./
#gcc -std=gnu99 -o ./$TEAM/ss ./$TEAM/superscalar.c -I./

## write the five stage simulator output and cycle counts for provided traces
## to a team-named file in the current directory
echo "------FIVE STAGE COUNTS: TEAM $TEAM -----" > ./$TEAM-counts

echo "--------------SAMPLE TRACE-------------------" >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 0 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 1 0 >> ./$TEAM-counts

echo "--------------LONG TRACES-------------------" >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/long_traces/sample_large1.tr 0 0 >> ./$TEAM-counts 
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/long_traces/sample_large1.tr 1 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/long_traces/sample_large2.tr 0 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/long_traces/sample_large2.tr 1 0 >> ./$TEAM-counts

echo "--------------SHORT TRACES-------------------" >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample1.tr 0 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample1.tr 1 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample2.tr 0 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample2.tr 1 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample3.tr 0 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample3.tr 1 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample4.tr 0 0 >> ./$TEAM-counts
./$TEAM/cpu /afs/cs.pitt.edu/courses/1541/short_traces/sample4.tr 1 0 >> ./$TEAM-counts


## repeat for superscalar simulator 
#echo "------SUPERSCALAR COUNTS: TEAM $TEAM -----" > ./$TEAM-counts

#echo "--------------SAMPLE TRACE-------------------" >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 0 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 1 0 >> ./$TEAM-counts

#echo "--------------LONG TRACES-------------------" >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/long_traces/sample_large1.tr 0 0 >> ./$TEAM-counts 
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/long_traces/sample_large1.tr 1 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/long_traces/sample_large2.tr 0 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/long_traces/sample_large2.tr 1 0 >> ./$TEAM-counts

#echo "--------------SHORT TRACES-------------------" >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample1.tr 0 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample1.tr 1 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample2.tr 0 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample2.tr 1 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample3.tr 0 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample3.tr 1 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample4.tr 0 0 >> ./$TEAM-counts
#./$TEAM/ss /afs/cs.pitt.edu/courses/1541/short_traces/sample4.tr 1 0 >> ./$TEAM-counts