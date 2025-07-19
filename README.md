# nanoMPI
By Quentin Anthony and Nick Sarkauskas

A minimal MPI Implementation loosely based on OpenMPI. nanoMPI has a few usecases:

1. First and foremost, nanoMPI is for educational purposes. The vast majority of most MPI libraries' code is dedicated to performance optimization, which makes them hard to parse as a beginner. nanoMPI allows beginners to the field of distributed computing to quickly see answers to questions like "how is a ring allreduce implemented?"
2. Development of distributed code on local devices is super convenient! It works offline and doesn't require job scheduling. Part of this project is to allow the authors to develop distributed code on a local laptop. 

# Contents

- [Quickstart](#quickstart)
- [Additional Resources](#additional-resources)
- [Things To Be Aware Of](#things-to-be-aware-of)
- [Roadmap](#roadmap)

## Quickstart

Here are some basic setup instructions for running socket-based point-to-point. These instructions assume a linux-based environment like Ubuntu or [WSL](https://learn.microsoft.com/en-us/windows/wsl/install).

### Pre-Requisites

Install GNU `make`
```
sudo apt install make
```

<details>
<summary>What is make? What are Makefiles?</summary>
<br>

`make` is a tool that helps compile software. Central to `make`'s operation is the `Makefile`, a text file located in the project's directory. The `Makefile` contains compilation instructions for `make`, including optimization settings, debugging options, and where to install components such as executables, documentation, and configuration files. `make` has a lot of convenience features such as only recompiling parts of the program that have changed since the last build, system-specific instructions, etc. My favorite resource on all things `Makefile` is at: https://makefiletutorial.com/

Use `make` to compile nanoMPI

</details>

Install the `ssh` server
```
sudo apt update
sudo apt install openssh-server
```

Enable it to start on boot
```
sudo systemctl enable ssh
```

Generate an ssh keypair using the ed25519 protocol. Use the default location under `~/.ssh/` with no passphrase.
```
ssh-keygen -t ed25519
```

You should now have a private key (`~/.ssh/id_ed25519`) and public key (`~/.ssh/id_ed25519.pub`).

Add your key to the `ssh-agent`:

```
ssh-add ~/.ssh/id_ed25519
```



Test your installation by `ssh`-ing to your local machine (**Note**: If using a remote server--like on a cluster--you should copy the key into ~/.ssh/authorized_keys on the remote server first. This will avoid it asking for your password every time):

```
$ ssh localhost
```

<details>
<summary>What is ssh? Why do we need it?</summary>
<br>
ssh (Secure Shell) is a cryptographic network protocol used for secure remote login over an unsecured network. We need SSH for several reasons:

- Secure remote access: It allows users to securely log into and control remote systems.
- File transfer: It enables secure file transfer between local and remote systems.
- Port forwarding: It can securely tunnel other protocols through its encrypted connection.

ssh-agent is a program that holds private keys used for public key authentication in SSH. It's particularly useful because:

- It saves you from typing your passphrase every time you use your SSH key.
- It allows for single sign-on across multiple SSH sessions.

In the context of the provided commands:

- We install the SSH server to allow incoming SSH connections to our machine.
- We generate an ED25519 key pair for secure authentication.
- We add the private key to ssh-agent to manage it securely and conveniently.
- We test the setup by SSH-ing to localhost, which simulates connecting to a remote machine.

</details>

### Repo Setup

Clone and build nanoMPI
```
git clone https://github.com/Quentin-Anthony/nanoMPI
cd nanoMPI
make
```

Add nanoMPI to your linux environment:
```
export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH
```

<details>
<summary>What are these Linux environment variables?</summary>
<br>
Linux environment variables are editable values that affect programs running on a system. They are part of the environment in which a process runs. There are a few important ones on linux-based systems:

- `LD_LIBRARY_PATH`: Tells the system where to look for shared libraries (e.g. `libmpi.so`) when executing programs at run-time.
- `PATH`: Tells the system where to look for program binaries (e.g. `mpirun`)

And if you're compiling with `gcc` like us:

- `LIBRARY_PATH`: Tells `gcc` where to look for linker files or ordinary libraries at compilation-time. Note that `LD_LIBRARY_PATH` is used by your program *after compilation* to find libraries, and `LIBRARY_PATH` is used by `gcc` *before compilation* to find libraries that need linked to the program.
- `CPATH`: Tells `gcc` where to look for include paths (i.e. header files) at compilation-time

You can access the values of environment variables using the `$` character. Try running `echo $LD_LIBRARY_PATH` to see the library paths already exported!

The command `export LD_LIBRARY_PATH=$PWD:$LD_LIBRARY_PATH` does the following:

- It adds the value of the current directory (`$PWD`, which is the `nanoMPI` directory) to the beginning of the existing `LD_LIBRARY_PATH`.
- This allows the system to find and use shared libraries (`libmpi.so`) in the current directory when running programs later on (`mpirun`).
- The `export` command makes this change available to all child processes of the current shell.

</details>

### Running

Fill in the hostfile with the hosts you will run with, one per line:
```
echo -e "localhost\nlocalhost" > hostfile
```


#### Run Basic [Hello World With MPI](tests/test_hello.c):
```
./mpirun ./hostfile ./tests/test_hello
```

Which should output:
```
Hello world from rank 0 out of 2 processors
Hello world from rank 1 out of 2 processors
```

#### Run [MPI_Allreduce Benchmark](benchmarks/benchmark_allreduce.c)

```
./mpirun ./hostfile ./benchmarks/benchmark_allreduce
```

Which should output:

```
Message Size (bytes)      Latency (us)         Bus BW (MB/s)        Validation
8                         47.01                0.1702               PASS
16                        41.87                0.3822               PASS
32                        36.82                0.8692               PASS
64                        39.55                1.6180               PASS
128                       76.70                1.6689               PASS
256                       40.31                6.3505               PASS
512                       119.03               4.3014               PASS
1024                      93.79                10.9182              PASS
2048                      97.00                21.1136              PASS
4096                      49776.58             0.0823               PASS
8192                      149.62               54.7506              PASS
16384                     77.30                211.9644             PASS
32768                     264.47               123.9025             PASS
65536                     139.57               469.5430             PASS
131072                    269.17               486.9470             PASS
262144                    547.08               479.1712             PASS
524288                    1076.73              486.9248             PASS
1048576                   2896.33              362.0366             PASS
```

## Things To Be Aware Of

The MPI standard allows MPI_Recv to partially fill the posted recvbuf if an incoming message matches the source, tag, and comm. The implementation so far:

- Requires each MPI_Send and MPI_Recv to match the buffer size, otherwise there may be a hang.
- Ignores the tag

These are todo items on the Roadmap.

## Additional Resources

- [This page from NCCL](https://github.com/NVIDIA/nccl-tests/blob/master/doc/PERFORMANCE.md) explains how to analyze the bandwidth of collectives
- [MPI Introduction](https://mpitutorial.com/tutorials/mpi-introduction/) - A quick introduction to common MPI concepts
- [Parallel Programming for Science and Engineering Book](https://theartofhpc.com/pcse/) - A thorough coverage of MPI concepts. More of a "textbook for MPI"

## Roadmap

Contributions are welcome! If you have something to add, open a PR!

- [x] Basic launcher
- [x] Socket implementation of point-to-point
- [x] Basic Collectives
- [x] Collective algos
- [x] Implement tag matching
- [x] Allow MPI_Recv to match with a send smaller than the recvbuf size
- [ ] InfiniBand implementation of point-to-point
- [ ] Basic benchmarks
    - [ ] Collectives
        - [ ] Allgather(v)
        - [x] Allreduce
        - [ ] Alltoall(v)
        - [ ] Bcast
        - [ ] Reduce
        - [ ] Reduce_scatter(v)
        - [ ] Scatter
        - [ ] Barrier
    - [ ] Point-to-Point
        - [ ] Latency
        - [ ] Bandwidth
        - [ ] Bidirectional Bandwidth
- [ ] PyTorch MPI Process group (PG) support
    - [ ] Subcommunicator/group functions used by the PG
        - [ ] MPI_Comm_group, MPI_Group_free
        - [ ] MPI_Comm_create
    - [ ] MPI_IN_PLACE
    - [ ] MPI_Isend, MPI_Irecv
    - [ ] Datatypes used by the PG
    - [ ] MPI_Request, MPI_REQUEST_NULL
    - [ ] MPI_Initialized()
    - [ ] MPI_Init_thread, MPI_THREAD_SERIALIZED
