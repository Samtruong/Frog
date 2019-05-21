import os


def perf_test():
    dataset = input("Dataset to run: ")
    num_run = input("Number of trials: ")
    os.system("rm -rf tmp.txt")
    for i in range(int(num_run)):
        os.system("CUDA_VISIBLE_DEVICES=1 ./exp ./data/" + dataset +  '>> tmp.txt')

def find_mean():
    with open('tmp.txt') as f:
        content = f.readlines()
    count = 0.0
    bfs_edge_loop = 0.0
    part_edge_loop = 0.0
    cc_edge_loop = 0.0
    part_cc_edge_loop = 0.0
    sssp_edge_loop = 0.0
    sssp_edge_part_loop = 0.0
    sssp_vertex = 0.0
    sssp_vertex_part = 0.0
    for line in content:

        if ('bfs_edge_loop' in line.split()):
            count +=1
            bfs_edge_loop += float(line[1:4])

        if ('part_edge_loop' in line.split()):
            part_edge_loop += float(line[1:4])

        if ('cc_edge_loop' in line.split()):
            cc_edge_loop += float(line[1:4])

        if ('part_cc_edge_loop' in line.split()):
            part_cc_edge_loop += float(line[1:4])

        if ('sssp_edge_loop' in line.split()):
            sssp_edge_loop += float(line[1:4])

        if ('sssp_edge_part_loop' in line.split()):
            sssp_edge_part_loop += float(line[1:4])

        if ('sssp_vertex' in line.split()):
            sssp_vertex += float(line[1:4])

        if ('sssp_vertex_part' in line.split()):
            sssp_vertex_part += float(line[1:4])

    print("-"*50)
    print("BFS:")
    print("Average bfs_edge_loop: " + str(bfs_edge_loop/count) + ' ms')
    print("Average part_edge_loop: " + str(part_edge_loop/count) + ' ms')

    print("-"*50)
    print("CC:")
    print("Average cc_edge_loop: " + str(cc_edge_loop/count) + ' ms')
    print("Average part_cc_edge_loop: " + str(part_cc_edge_loop/count) + ' ms')

    print("-"*50)
    print("SSSP:")
    print("Average sssp_edge_loop: " + str(sssp_edge_loop/count) + ' ms')
    print("Average sssp_edge_part_loop: " + str(sssp_edge_part_loop/count) + ' ms')
    print("Average sssp_vertex: " + str(sssp_vertex/count) + ' ms')
    print("Average sssp_vertex_part: " + str(sssp_vertex_part/count) + ' ms')


if __name__ == '__main__':
    perf_test()
    find_mean()
