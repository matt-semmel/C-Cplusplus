@echo off
title Project 3 Batch. AKA: Matt does not want to keep typing this over and over again!
javac vmsim.java

echo Running traces for OPT algo, 1:1 memSplit........
echo:
java vmsim -a opt -n 16 -p 4 -s 1:1 2-gzip-mcf.trace
echo:
echo:
java vmsim -a opt -n 1024 -p 4 -s 1:1 2-gzip-mcf.trace
echo:
echo:
java vmsim -a opt -n 16 -p 4000 -s 1:1 2-gzip-mcf.trace
echo:
echo:
java vmsim -a opt -n 1024 -p 4000 -s 1:1 2-gzip-mcf.trace
echo:
pause
echo: 
echo Running traces for LRU algo, 1:1 memSplit........
echo:
java vmsim -a lru -n 16 -p 4 -s 1:1 2-gzip-mcf.trace
echo:
echo:
java vmsim -a lru -n 1024 -p 4 -s 1:1 2-gzip-mcf.trace
echo:
echo:
java vmsim -a lru -n 16 -p 4000 -s 1:1 2-gzip-mcf.trace
echo:
echo:
java vmsim -a lru -n 1024 -p 4000 -s 1:1 2-gzip-mcf.trace
echo: