#!/usr/bin/env python3
import matplotlib.pyplot as plt
import numpy as np
import pandas

performancetest = ["test_and_set", "test_and_test_and_set"]

for i in range(len(performancetest)):
    measures = pandas.read_csv("./Part2/csv/mesures_"+performancetest[i]+".csv")

    threads = measures["thread"].unique()
    mean = measures.groupby(["thread"]).mean()["time"]
    std = measures.groupby(["thread"]).std()["time"]

    figure = plt.figure()

    plt.plot(threads, mean, color="blue", linewidth=1.0)
    plt.plot(threads, std, color="red", linewidth=1.0)
    plt.errorbar(threads, mean, yerr=std, fmt="-o")

    plt.title("Execution time for programm " + performancetest[i])
    plt.xlabel("Number of Threads")
    plt.ylabel("Time in s")
    plt.ylim(bottom = 0)
    plt.grid()
    plt.savefig("./Part2/png/Graph_"+performancetest[i]+".png")
    plt.close()
