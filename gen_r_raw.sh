#!/bin/bash

cat gups/gups50/1/profiling_* | sort -nk2 > gups/gups50/1/r
awk '{print $1}' gups/gups50/1/r > gups/gups50/1/raw

cat gups/gups50/2/profiling_* | sort -nk2 > gups/gups50/2/r
awk '{print $1}' gups/gups50/2/r > gups/gups50/2/raw

cat gups/gups50/3/profiling_* | sort -nk2 > gups/gups50/3/r
awk '{print $1}' gups/gups50/3/r > gups/gups50/3/raw

cat gups/gups50/4/profiling_* | sort -nk2 > gups/gups50/4/r
awk '{print $1}' gups/gups50/4/r > gups/gups50/4/raw

cat gups/gups50/5/profiling_* | sort -nk2 > gups/gups50/5/r
awk '{print $1}' gups/gups50/5/r > gups/gups50/5/raw



cat gups/gups30/1/profiling_* | sort -nk2 > gups/gups30/1/r
awk '{print $1}' gups/gups30/1/r > gups/gups30/1/raw

cat gups/gups30/2/profiling_* | sort -nk2 > gups/gups30/2/r
awk '{print $1}' gups/gups30/2/r > gups/gups30/2/raw

cat gups/gups30/3/profiling_* | sort -nk2 > gups/gups30/3/r
awk '{print $1}' gups/gups30/3/r > gups/gups30/3/raw

cat gups/gups30/4/profiling_* | sort -nk2 > gups/gups30/4/r
awk '{print $1}' gups/gups30/4/r > gups/gups30/4/raw

cat gups/gups30/5/profiling_* | sort -nk2 > gups/gups30/5/r
awk '{print $1}' gups/gups30/5/r > gups/gups30/5/raw



cat gups/gups15/1/profiling_* | sort -nk2 > gups/gups15/1/r
awk '{print $1}' gups/gups15/1/r > gups/gups15/1/raw

cat gups/gups15/2/profiling_* | sort -nk2 > gups/gups15/2/r
awk '{print $1}' gups/gups15/2/r > gups/gups15/2/raw

cat gups/gups15/3/profiling_* | sort -nk2 > gups/gups15/3/r
awk '{print $1}' gups/gups15/3/r > gups/gups15/3/raw

cat gups/gups15/4/profiling_* | sort -nk2 > gups/gups15/4/r
awk '{print $1}' gups/gups15/4/r > gups/gups15/4/raw

cat gups/gups15/5/profiling_* | sort -nk2 > gups/gups15/5/r
awk '{print $1}' gups/gups15/5/r > gups/gups15/5/raw

