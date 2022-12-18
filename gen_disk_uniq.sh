#!/bin/bash


sort -n gups/gups50/1/raw | uniq -c | sort -nk1 > gups/gups50/1/uniq
awk '{print $1}' gups/gups50/1/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups50/1/dist

sort -n gups/gups50/2/raw | uniq -c | sort -nk1 > gups/gups50/2/uniq
awk '{print $1}' gups/gups50/2/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups50/2/dist

sort -n gups/gups50/3/raw | uniq -c | sort -nk1 > gups/gups50/3/uniq
awk '{print $1}' gups/gups50/3/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups50/3/dist

sort -n gups/gups50/4/raw | uniq -c | sort -nk1 > gups/gups50/4/uniq
awk '{print $1}' gups/gups50/4/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups50/4/dist

sort -n gups/gups50/5/raw | uniq -c | sort -nk1 > gups/gups50/5/uniq
awk '{print $1}' gups/gups50/5/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups50/5/dist


sort -n gups/gups30/1/raw | uniq -c | sort -nk1 > gups/gups30/1/uniq
awk '{print $1}' gups/gups30/1/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups30/1/dist

sort -n gups/gups30/2/raw | uniq -c | sort -nk1 > gups/gups30/2/uniq
awk '{print $1}' gups/gups30/2/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups30/2/dist

sort -n gups/gups30/3/raw | uniq -c | sort -nk1 > gups/gups30/3/uniq
awk '{print $1}' gups/gups30/3/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups30/3/dist

sort -n gups/gups30/4/raw | uniq -c | sort -nk1 > gups/gups30/4/uniq
awk '{print $1}' gups/gups30/4/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups30/4/dist

sort -n gups/gups30/5/raw | uniq -c | sort -nk1 > gups/gups30/5/uniq
awk '{print $1}' gups/gups30/5/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups30/5/dist


sort -n gups/gups15/1/raw | uniq -c | sort -nk1 > gups/gups15/1/uniq
awk '{print $1}' gups/gups15/1/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups15/1/dist

sort -n gups/gups15/2/raw | uniq -c | sort -nk1 > gups/gups15/2/uniq
awk '{print $1}' gups/gups15/2/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups15/2/dist

sort -n gups/gups15/3/raw | uniq -c | sort -nk1 > gups/gups15/3/uniq
awk '{print $1}' gups/gups15/3/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups15/3/dist

sort -n gups/gups15/4/raw | uniq -c | sort -nk1 > gups/gups15/4/uniq
awk '{print $1}' gups/gups15/4/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups15/4/dist

sort -n gups/gups15/5/raw | uniq -c | sort -nk1 > gups/gups15/5/uniq
awk '{print $1}' gups/gups15/5/uniq | uniq -c | awk '{print $2"\t"$1}' > gups/gups15/5/dist


