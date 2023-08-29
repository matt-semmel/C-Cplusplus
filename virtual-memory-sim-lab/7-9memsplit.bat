@echo off
title Project 3 Batch. AKA: Matt does not want to keep typing this over and over again!
javac vmsim.java

echo Running traces for OPT algo, 7:9 memSplit........
echo:
java vmsim -a opt -n 16 -p 4 -s 7:9 2-gzip-mcf.trace
echo:
echo:
java vmsim -a opt -n 1024 -p 4 -s 7:9 2-gzip-mcf.trace
echo:
echo:
java vmsim -a opt -n 16 -p 4000 -s 7:9 2-gzip-mcf.trace
echo:
echo:
java vmsim -a opt -n 1024 -p 4000 -s 7:9 2-gzip-mcf.trace
echo:
pause
echo: 
echo Running traces for LRU algo, 7:9 memSplit........
echo:
java vmsim -a lru -n 16 -p 4 -s 7:9 2-gzip-mcf.trace
echo:
echo:
java vmsim -a lru -n 1024 -p 4 -s 7:9 2-gzip-mcf.trace
echo:
echo:
java vmsim -a lru -n 16 -p 4000 -s 7:9 2-gzip-mcf.trace
echo:
echo:
java vmsim -a lru -n 1024 -p 4000 -s 7:9 2-gzip-mcf.trace
echo: