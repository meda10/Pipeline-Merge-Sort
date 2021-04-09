import matplotlib.pyplot as plt
import numpy as np
import random

def do_something(name, list):
    for i in range(0, 3):
        list.remove(max(list))
        list.remove(min(list))

    length = len(list)
    avg = round(sum(list) / len(list))
    min_l = min(list)
    max_l = max(list)
    print("N: {} | Len: {} | Min: {} Max: {} | Avg: {}".format(name, length, min_l, max_l, avg))
    return avg


if __name__ == '__main__':
    avgs = []
    avgs.append(do_something("16", [150739, 41307, 156282, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]))
    avgs.append(do_something("32", [0, 105276, 0, 0, 0, 428693, 0, 312518, 0, 0, 528707, 0, 21091, 385859, 367706, 0, 400059, 0, 0, 91663]))
    avgs.append(do_something("64", [0, 0, 181401, 91570, 408046, 423689, 2225208, 0, 0, 186697, 159590, 455750, 48181, 0, 0, 121654, 458720, 91979, 0, 132767]))
    avgs.append(do_something("128", [182440, 0, 173187, 247677, 212561, 526515, 0, 0, 148908, 575217, 0, 0, 619105, 0, 0, 26090, 41257, 0, 0, 511714]))
    avgs.append(do_something("256", [538584, 275186, 314867, 0, 271626, 0, 718716, 171358, 594847, 0, 602973, 178864, 0, 644985, 730840, 369853, 128889, 0, 609575, 136322]))
    avgs.append(do_something("512", [273939, 787515, 469236, 370134, 377477, 262169, 934924, 396530, 332031, 735387, 210507, 514546, 86989, 630136, 560178, 824272, 450181, 564507, 807110, 106387]))
    avgs.append(do_something("1024", [1716049, 261312, 383563, 1155175, 1461854, 784280, 999278, 1154955, 1098626, 837544, 240075, 1288879, 407909, 442554, 691785, 1207270, 1174724, 1151246, 277531, 835859]))
    avgs.append(do_something("2048", [1885039, 1373490, 2601151, 1386385, 1715375, 2065157, 2152515, 1787028, 1960908, 1462529, 2094678, 1922830, 1933779, 1458043, 2150679, 2460268, 1873696, 2484444, 1274136, 1672049]))

    a = np.arange(8)
    x = [16, 32, 64, 128, 256, 512, 1024, 2048]

    #plt.plot(a, avgs)
    #plt.xticks(a, x)
    plt.plot(x, avgs)
    plt.xlabel('Velikost vstupu')
    plt.ylabel('Čas [ns]')
    plt.title("")
    plt.savefig('graph.png')



