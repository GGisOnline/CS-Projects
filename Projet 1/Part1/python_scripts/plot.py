#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import pandas

performancetests=["philosopher", "ReaderWriter", "producer_consumer"]

for i in range(len(performancetests)):
    measures = pandas.read_csv("./Part1/csv/mesures_"+performancetests[i]+".csv")

    threads = measures["thread"].unique()
    mean = measures.groupby(["thread"]).mean()["time"]
    std = measures.groupby(["thread"]).std()["time"]

    figure = plt.figure()

    plt.plot(threads, mean, color="blue", linewidth=1.0)
    plt.plot(threads, std, color="red", linewidth=1.0)
    plt.errorbar(threads, mean, yerr=std, fmt="-o")

    plt.title("Execution time for programm " + performancetests[i])
    plt.xlabel("Number of Threads")
    plt.ylabel("Time in s")
    plt.ylim(bottom = 0)
    plt.grid()
    plt.savefig("./Part1/png/Graph_"+performancetests[i]+".png")
    plt.close()

