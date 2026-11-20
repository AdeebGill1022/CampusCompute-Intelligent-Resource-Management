# CampusCompute

A command-line C prototype demonstrating CPU scheduling, POSIX threads, POSIX shared-memory IPC, semaphores, Banker's safety algorithm, child-process status collection, and process resource limits.

## Prerequisites

- Ubuntu Linux or WSL 2 with Ubuntu
- GCC, GNU Make, and POSIX threads/semaphore/shared-memory support

Install tools:

```bash
sudo apt update
sudo apt install build-essential
```

## Build and run

```bash
make
./campuscompute help
./campuscompute schedule fcfs
./campuscompute schedule rr       # enter a positive time quantum when prompted
./campuscompute schedule priority
./campuscompute schedule custom fcfs
./campuscompute compare
./campuscompute logger 8
./campuscompute ipc
./campuscompute banker both
./campuscompute banker safe
./campuscompute banker unsafe
./campuscompute worker both
./campuscompute worker normal
./campuscompute worker fail
./campuscompute limit
./campuscompute demo
./campuscompute logs
make clean
```

`compare` runs the three scheduling policies against the same sample workload; for RR it prompts for a quantum. The integrated `demo` runs FCFS plus the logger, IPC, Banker's cases, worker cases, and resource-limit demonstration.

## Scheduling rules

The built-in workload is J1=(arrival 0, burst 5, priority 2), J2=(1,3,1), J3=(2,8,3), and J4=(3,6,2). Lower numeric priority is higher priority. FCFS ties are resolved by arrival time then job ID. Non-preemptive priority ties are resolved by priority, arrival time, then job ID. Round Robin appends newly arrived jobs before re-queueing the preempted job at the same time boundary. RR asks for a positive time quantum. Turnaround = completion - arrival; waiting = turnaround - CPU burst.

For custom jobs, arrival must be 0..100000, burst 1..100000, priority 1..100000, and job count 1..20. Invalid input cancels the scheduling run.

## Design and cleanup

- Logger: bounded five-entry queue, one producer and one consumer, mutex plus `not_empty`/`not_full` condition variables. Threads are joined and the log file is closed.
- IPC: unique per-run POSIX shared-memory and named-semaphore names based on the parent PID. The parent unmaps/closes and unlinks only those objects it created.
- Workers: `fork()` plus `waitpid()`. Exit code 42 is an explicitly controlled simulated failure, not an OS crash.
- Resource limit: a child lowers its soft `RLIMIT_NOFILE` to 16 and opens `/dev/null` until `EMFILE` is observed. The parent process limit is not changed.
- Banker: standard Need = Maximum - Allocation and iterative safety check; included safe and unsafe test states.
- The logger appends to `logs/campuscompute.log`; `make clean` removes build products but deliberately preserves logs. Remove only this project's generated log with `rm -f logs/campuscompute.log` if desired.

## Suggested screenshot command map

Run these from the project directory after setting the SRN prompt. The exact assignment checklist controls the final screenshot content; these commands are suggested evidence actions:

- S01: `pwd` and `uname -a`
- S02: `gcc --version` and `make --version`
- S03: `ls -la`
- S04: `make clean && make`
- S05: `./campuscompute help`
- S06: `./campuscompute schedule fcfs`
- S07: `./campuscompute schedule rr` (enter `2`)
- S08: `./campuscompute schedule priority`
- S09: `./campuscompute compare` (enter `2` for RR)
- S10-S11: `./campuscompute logger 8` followed by `./campuscompute logs`
- S12-S13: `./campuscompute ipc`
- S14-S15: `./campuscompute banker both`
- S16-S17: `./campuscompute worker both`
- S18: `./campuscompute limit`
- S19: `./campuscompute unknown-command`
- S20: run `./campuscompute ipc`, then inspect only the program's own `/dev/shm` names if your environment exposes them; the program also prints its cleanup confirmation
- S21: `./campuscompute demo`
- S22: `./campuscompute logs`
- S23: `ls -l` and `file campuscompute`
- S24: from the parent directory, `unzip -l CampusCompute.zip` and `unzip -t CampusCompute.zip`

A single command may support more than one checklist item, but capture separate numbered screenshots where the assignment asks for distinct evidence. Keep the complete SRN visible in every terminal screenshot.

## Evidence screenshots

Capture genuine screenshots in your own Ubuntu/WSL terminal. Set your own complete SRN in the prompt before taking evidence, for example:

```bash
export SRN='YOUR_COMPLETE_SRN'
export PS1='[$SRN] \w\$ '
```

Replace the placeholder with your actual SRN. Capture the applicable S01-S24 commands/results from the assignment checklist, number them without reuse, and place each screenshot followed by one explanatory sentence into a single PDF named exactly `<YOUR_COMPLETE_SRN>.pdf`. Do not fabricate output or include screenshots from another machine/student. This package does not generate or claim to provide those personal terminal screenshots.
