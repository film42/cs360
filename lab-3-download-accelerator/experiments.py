import os
import sys
import time

# Run a set of download experiments

# List of URLs to download (Census Bureau)

urls = [        ("small","http://film42.s3.amazonaws.com/tl_2013_10_tract.zip"),
                ("medium","http://film42.s3.amazonaws.com/tl_2013_35005_edges.zip"),
                ("large","http://film42.s3.amazonaws.com/tlgdb_2013_a_39_oh.gdb.zip")]

# Number of threads to use
threads = [1,2,3,5,10,50,100]

# Number of times to repeat each experiment
times = 10

# Run experiment for each URL with a given number of threads
for (name,url) in urls:
    for thread in threads:
        sys.stdout.write("Running experiment for %s with %s threads" % (url,thread))
        output = "data-%s.txt" % (name)
        for i in range(0,times):
            sys.stdout.write(".")
            os.system("python downloadAccelerator.py -n %d %s >> %s" % (thread,url,output))
            sys.stdout.flush()
        print
