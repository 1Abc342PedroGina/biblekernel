#ifndef _BIBLE_SYSCALLS_H
#define _BIBLE_SYSCALLS_H

#include <bible/ktypes.h>
#include <bible/regsister.h>

/* --- Process Management & Lifecycle --- */
#define BIBLE_READ              10000
#define BIBLE_WRITE             10001
#define BIBLE_OPEN              10002
#define BIBLE_CLOSE             10003
#define BIBLE_STAT              10004
#define BIBLE_FSTAT             10005
#define BIBLE_LSTAT             10006
#define BIBLE_POLL              10007
#define BIBLE_LSEEK             10008
#define BIBLE_MMAP              10009
#define BIBLE_MPROTECT          10010
#define BIBLE_MUNMAP            10011
#define BIBLE_BRK               10012
#define BIBLE_RT_SIGACTION      10013
#define BIBLE_RT_SIGPROCMASK    10014
#define BIBLE_RT_SIGRETURN      10015
#define BIBLE_IOCTL             10016
#define BIBLE_PREAD64           10017
#define BIBLE_PWRITE64          10018
#define BIBLE_READV             10019
#define BIBLE_WRITEV            10020
#define BIBLE_ACCESS            10021
#define BIBLE_PIPE              10022
#define BIBLE_SELECT            10023
#define BIBLE_SCHED_YIELD       10024
#define BIBLE_MREMAP            10025
#define BIBLE_MSYNC             10026
#define BIBLE_MINCORE           10027
#define BIBLE_MADVISE           10028
#define BIBLE_SHMGET            10029
#define BIBLE_SHMAT             10030
#define BIBLE_SHMCTL            10031
#define BIBLE_DUP               10032
#define BIBLE_DUP2              10033
#define BIBLE_PAUSE             10034
#define BIBLE_NANOSLEEP         10035
#define BIBLE_GETITIMER         10036
#define BIBLE_ALARM             10037
#define BIBLE_SETITIMER         10038
#define BIBLE_GETPID            10039
#define BIBLE_SENDFILE          10040
#define BIBLE_SOCKET            10041
#define BIBLE_CONNECT           10042
#define BIBLE_ACCEPT            10043
#define BIBLE_SENDTO            10044
#define BIBLE_RECVFROM          10045
#define BIBLE_SENDMSG           10046
#define BIBLE_RECVMSG           10047
#define BIBLE_SHUTDOWN          10048
#define BIBLE_BIND              10049
#define BIBLE_LISTEN            10050
#define BIBLE_GETSOCKNAME       10051
#define BIBLE_GETPEERNAME       10052
#define BIBLE_SOCKETPAIR        10053
#define BIBLE_SETSOCKOPT        10054
#define BIBLE_GETSOCKOPT        10055
#define BIBLE_CLONE             10056
#define BIBLE_FPROC             10057
/* Cria um Processo Filho do Pai
 * Tambem cria um Processo Irmão do Pai, Irmão do Filho
 * Tambem cria um Processo Filho, que é o Própio Pai
 * Tambem cria um Processo Pai, que é o Filho, dele mesmo
 * Cria Processo Tio, do Pai, Irmão do Filho
 * tambem cria processo Neto
 * Tambem cria Processo Irmão do Pai
 * Tambem cria um Processo Neto do Filho
 * E Processos consubstanciais, ou seja, que tem a mesma substancias, 
 * Mas recursos e/ou pessoas diferente
 */
#define BIBLE_RPROC           10058
/* Semelhante ao FPROC, mas  mas com a diferença crítica de que o filho compartilha o espaço de
 * endereçamento (memória) do 'familiar 1' e o 'familiar 1' fica bloqueado até que o 'familiar 2'
 * chame as syscall de executar e/ou de saida
 * É mais rápido que o FPROC, Mas exige cautela
 */
#define BIBLE_EXECVE            10059
#define BIBLE_EXIT              10060
#define BIBLE_WAIT4             10061
#define BIBLE_KILL              10062
#define BIBLE_UNAME             10063
#define BIBLE_SEMGET            10064
#define BIBLE_SEMOP             10065
#define BIBLE_SEMCTL            10066
#define BIBLE_SHMDT             10067
#define BIBLE_MSGGET            10068
#define BIBLE_MSGSND            10069
#define BIBLE_MSGRCV            10070
#define BIBLE_MSGCTL            10071
#define BIBLE_FCNTL             10072
#define BIBLE_FLOCK             10073
#define BIBLE_FSYNC             10074
#define BIBLE_FDATASYNC         10075
#define BIBLE_TRUNCATE          10076
#define BIBLE_FTRUNCATE         10077
#define BIBLE_GETDENTS          10078
#define BIBLE_GETCWD            10079
#define BIBLE_CHDIR             10080
#define BIBLE_FCHDIR            10081
#define BIBLE_RENAME            10082
#define BIBLE_MKAD              10083
/* MKAD significa Make a DIR */
#define BIBLE_RMDIR             10084
#define BIBLE_CREAT             10085
#define BIBLE_LINK              10086
#define BIBLE_UNLINK            10087
#define BIBLE_SYMLINK           10088
#define BIBLE_READLINK          10089
#define BIBLE_CHMOD             10090
#define BIBLE_FCHMOD            10091
#define BIBLE_CHOWN             10092
#define BIBLE_FCHOWN            10093
#define BIBLE_LCHOWN            10094
#define BIBLE_UMASK             10095
#define BIBLE_GETTIMEOFDAY      10096
#define BIBLE_GETRLIMIT         10097
#define BIBLE_GETRUSAGE         10098
#define BIBLE_SYSINFO           10099
#define BIBLE_TIMES             10100
#define BIBLE_PTRACE            10101
#define BIBLE_GETUID            10102
#define BIBLE_SYSLOG            10103
#define BIBLE_GETGID            10104
#define BIBLE_SETUID            10105
#define BIBLE_SETGID            10106
#define BIBLE_GETEUID           10107
#define BIBLE_GETEGID           10108
#define BIBLE_SETPGID           10109
#define BIBLE_GETPPID           10110
#define BIBLE_GETPGRP           10111
#define BIBLE_SETSID            10112
#define BIBLE_SETREUID          10113
#define BIBLE_SETREGID          10114
#define BIBLE_GETGROUPS         10115
#define BIBLE_SETGROUPS         10116
#define BIBLE_SETRESUID         10117
#define BIBLE_GETRESUID         10118
#define BIBLE_SETRESGID         10119
#define BIBLE_GETRESGID         10120
#define BIBLE_GETPGID           10121
#define BIBLE_SETFSUID          10122
#define BIBLE_SETFSGID          10123
#define BIBLE_GETSID            10124
#define BIBLE_CAPGET            10125
#define BIBLE_CAPSET            10126
#define BIBLE_RT_SIGPENDING     10127
#define BIBLE_RT_SIGTIMEDWAIT   10128
#define BIBLE_RT_SIGQUEUEINFO   10129
#define BIBLE_RT_SIGSUSPEND     10130
#define BIBLE_SIGALTSTACK       10131
#define BIBLE_UTIME             10132
#define BIBLE_MKNOD             10133
#define BIBLE_USELIB            10134
#define BIBLE_PERSONALITY       10135
#define BIBLE_USTAT             10136
#define BIBLE_STATFS            10137
#define BIBLE_FSTATFS           10138
#define BIBLE_SYSFS             10139
#define BIBLE_GETPRIORITY       10140
#define BIBLE_SETPRIORITY       10141
#define BIBLE_SCHED_SETPARAM    10142
#define BIBLE_SCHED_GETPARAM    10143
#define BIBLE_SCHED_SETSCHEDULER 10144
#define BIBLE_SCHED_GETSCHEDULER 10145
#define BIBLE_SCHED_GET_PRIORITY_MAX 10146
#define BIBLE_SCHED_GET_PRIORITY_MIN 10147
#define BIBLE_SCHED_RR_GET_INTERVAL  10148
#define BIBLE_MLOCK             10149
#define BIBLE_MUNLOCK           10150
#define BIBLE_MLOCKALL          10151
#define BIBLE_MUNLOCKALL        10152
#define BIBLE_VHANGUP           10153
#define BIBLE_MODIFY_LDT        10154
#define BIBLE_PIVOT_ROOT        10155
#define BIBLE_SYSCTL            10156
#define BIBLE_PRCTL             10157
#define BIBLE_ARCH_PRCTL        10158
#define BIBLE_ADJTIMEX          10159
#define BIBLE_SETRLIMIT         10160
#define BIBLE_CHROOT            10161
#define BIBLE_SYNC              10162
#define BIBLE_ACCT              10163
#define BIBLE_SETTIMEOFDAY      10164
#define BIBLE_MOUNT             10165
#define BIBLE_UMOUNT2           10166
#define BIBLE_SWAPON            10167
#define BIBLE_SWAPOFF           10168
#define BIBLE_REBOOT            10169
#define BIBLE_SETHOSTNAME       10170
#define BIBLE_SETDOMAINNAME     10171
#define BIBLE_IOPL              10172
#define BIBLE_IOPERM            10173
#define BIBLE_CREATE_MODULE     10174
#define BIBLE_INIT_MODULE       10175
#define BIBLE_DELETE_MODULE     10176
#define BIBLE_GET_KERNEL_SYMS   10177
#define BIBLE_QUERY_MODULE      10178
#define BIBLE_QUOTACTL          10179
#define BIBLE_NFSSERVCTL        10180
#define BIBLE_GETPMSG           10181
#define BIBLE_PUTPMSG           10182
#define BIBLE_AFS_SYSCALL       10183
#define BIBLE_TUXCALL           10184
#define BIBLE_SECURITY          10185
#define BIBLE_GETTID            10186
#define BIBLE_READAHEAD         10187
#define BIBLE_SETXATTR          10188
#define BIBLE_LSETXATTR         10189
#define BIBLE_FSETXATTR         10190
#define BIBLE_GETXATTR          10191
#define BIBLE_LGETXATTR         10192
#define BIBLE_FGETXATTR         10193
#define BIBLE_LISTXATTR         10194
#define BIBLE_LLISTXATTR        10195
#define BIBLE_FLISTXATTR        10196
#define BIBLE_REMOVEXATTR       10197
#define BIBLE_LREMOVEXATTR      10198
#define BIBLE_FREMOVEXATTR      10199
#define BIBLE_TKILL             10200
#define BIBLE_TIME              10201
#define BIBLE_FUTEX             10202
#define BIBLE_SCHED_SETAFFINITY 10203
#define BIBLE_SCHED_GETAFFINITY 10204
#define BIBLE_SET_THREAD_AREA   10205
#define BIBLE_IO_SETUP          10206
#define BIBLE_IO_DESTROY        10207
#define BIBLE_IO_GETEVENTS      10208
#define BIBLE_IO_SUBMIT         10209
#define BIBLE_IO_CANCEL         10210
#define BIBLE_GET_THREAD_AREA   10211
#define BIBLE_LOOKUP_DCOOKIE    10212
#define BIBLE_EPOLL_CREATE      10213
#define BIBLE_EPOLL_CTL_OLD     10214
#define BIBLE_EPOLL_WAIT_OLD    10215
#define BIBLE_REMAP_FILE_PAGES  10216
#define BIBLE_GETDENTS64        10217
#define BIBLE_SET_TID_ADDRESS   10218
#define BIBLE_RESTART_SYSCALL   10219
#define BIBLE_SEMTIMEDOP        10220
#define BIBLE_FADVISE64         10221
#define BIBLE_TIMER_CREATE      10222
#define BIBLE_TIMER_SETTIME     10223
#define BIBLE_TIMER_GETTIME     10224
#define BIBLE_TIMER_GETOVERRUN  10225
#define BIBLE_TIMER_DELETE      10226
#define BIBLE_CLOCK_SETTIME     10227
#define BIBLE_CLOCK_GETTIME     10228
#define BIBLE_CLOCK_GETRES      10229
#define BIBLE_CLOCK_NANOSLEEP   10230
#define BIBLE_EXIT_GROUP        10231
#define BIBLE_EPOLL_WAIT        10232
#define BIBLE_EPOLL_CTL         10233
#define BIBLE_TGKILL            10234
#define BIBLE_UTIMES            10235
#define BIBLE_VSERVER           10236
#define BIBLE_MBIND             10237
#define BIBLE_SET_MEMPOLICY     10238
#define BIBLE_GET_MEMPOLICY     10239
#define BIBLE_MQ_OPEN           10240
#define BIBLE_MQ_UNLINK         10241
#define BIBLE_MQ_TIMEDSEND      10242
#define BIBLE_MQ_TIMEDRECEIVE   10243
#define BIBLE_MQ_NOTIFY         10244
#define BIBLE_MQ_GETSETATTR     10245
#define BIBLE_KEXEC_LOAD        10246
#define BIBLE_WAITID            10247
#define BIBLE_ADD_KEY           10248
#define BIBLE_REQUEST_KEY       10249
#define BIBLE_KEYCTL            10250
#define BIBLE_IOPRIO_SET        10251
#define BIBLE_IOPRIO_GET        10252
#define BIBLE_INOTIFY_INIT      10253
#define BIBLE_INOTIFY_ADD_WATCH 10254
#define BIBLE_INOTIFY_RM_WATCH  10255
#define BIBLE_MIGRATE_PAGES     10256
#define BIBLE_OPENAT            10257
#define BIBLE_MKDIRAT           10258
#define BIBLE_MKNODAT           10259
#define BIBLE_FCHOWNAT          10260
#define BIBLE_FUTIMESAT         10261
#define BIBLE_NEWFSTATAT        10262
#define BIBLE_UNLINKAT          10263
#define BIBLE_RENAMEAT          10264
#define BIBLE_LINKAT            10265
#define BIBLE_SYMLINKAT         10266
#define BIBLE_READLINKAT        10267
#define BIBLE_FCHMODAT          10268
#define BIBLE_FACCESSAT         10269
#define BIBLE_PSELECT6          10270
#define BIBLE_PPOLL             10271
#define BIBLE_UNSHARE           10272
#define BIBLE_SET_ROBUST_LIST   10273
#define BIBLE_GET_ROBUST_LIST   10274
#define BIBLE_SPLICE            10275
#define BIBLE_TEE               10276
#define BIBLE_SYNC_FILE_RANGE   10277
#define BIBLE_VMSPLICE          10278
#define BIBLE_MOVE_PAGES        10279
#define BIBLE_UTIMENSAT         10280
#define BIBLE_EPOLL_PWAIT       10281
#define BIBLE_SIGNALFD          10282
#define BIBLE_TIMERFD_CREATE    10283
#define BIBLE_EVENTFD           10284
#define BIBLE_FALLOCATE         10285
#define BIBLE_TIMERFD_SETTIME   10286
#define BIBLE_TIMERFD_GETTIME   10287
#define BIBLE_ACCEPT4           10288
#define BIBLE_SIGNALFD4         10289
#define BIBLE_EVENTFD2          10290
#define BIBLE_EPOLL_CREATE1     10291
#define BIBLE_DUP3              10292
#define BIBLE_PIPE2             10293
#define BIBLE_IONOTIFY_INIT1    10294
#define BIBLE_PREADV            10295
#define BIBLE_PWRITEV           10296
#define BIBLE_RT_TGSIGQUEUEINFO 10297
#define BIBLE_PERF_EVENT_OPEN   10298
#define BIBLE_RECVMMSG          10299
#define BIBLE_FANOTIFY_INIT     10300
#define BIBLE_FANOTIFY_MARK     10301
#define BIBLE_PRLIMIT64         10302
#define BIBLE_NAME_TO_HANDLE_AT 10303
#define BIBLE_OPEN_BY_HANDLE_AT 10304
#define BIBLE_CLOCK_ADJTIME     10305
#define BIBLE_SYNCFS            10306
#define BIBLE_SENDMMSG          10307
#define BIBLE_SETNS             10308
#define BIBLE_GETCPU            10309
#define BIBLE_PROCESS_VM_READV  10310
#define BIBLE_PROCESS_VM_WRITEV 10311
#define BIBLE_KCMP              10312
#define BIBLE_FINIT_MODULE      10313
#define BIBLE_SCHED_SETATTR     10314
#define BIBLE_SCHED_GETATTR     10315
#define BIBLE_RENAMEAT2         10316
#define BIBLE_SECCOMP           10317
#define BIBLE_GETRANDOM         10318
#define BIBLE_MEMFD_CREATE      10319
#define BIBLE_KEXEC_FILE_LOAD   10320
#define BIBLE_BPF               10321
#define BIBLE_EXECVEAT          10322
#define BIBLE_USERFAULTFD       10323
#define BIBLE_MEMBARRIER        10324
#define BIBLE_MLOCK2            10325
#define BIBLE_COPY_FILE_RANGE   10326
#define BIBLE_PREADV2           10327
#define BIBLE_PWRITEV2          10328
#define BIBLE_PKEY_MPROTECT     10329
#define BIBLE_PKEY_ALLOC        10330
#define BIBLE_PKEY_FREE         10331
#define BIBLE_STATX             10332
#define BIBLE_IO_PGETEVENTS     10333
#define BIBLE_RSEQ              10334
#define BIBLE_PIDFD_SEND_SIGNAL 10335
#define BIBLE_IO_URING_SETUP    10336
#define BIBLE_IO_URING_ENTER    10337
#define BIBLE_IO_URING_REGISTER 10338
#define BIBLE_OPEN_TREE         10339
#define BIBLE_MOVE_MOUNT        10340
#define BIBLE_FSOPEN            10341
#define BIBLE_FSCONFIG          10342
#define BIBLE_FSMOUNT           10343
#define BIBLE_FSPICK            10344
#define BIBLE_PIDFD_OPEN        10345
#define BIBLE_CLONE3            10346
#define BIBLE_CLOSE_RANGE       10347
#define BIBLE_OPENAT2           10348
#define BIBLE_PIDFD_GETFD       10349
#define BIBLE_FACCESSAT2        10350
#define BIBLE_PROCESS_MADVISE   10351
#define BIBLE_EPOLL_PWAIT2      10352
#define BIBLE_MOUNT_SETATTR     10353
#define BIBLE_QUOTACTL_FD       10354
#define BIBLE_LANDLOCK_CREATE_RULESET 10355
#define BIBLE_LANDLOCK_ADD_RULE       10356
#define BIBLE_LANDLOCK_RESTRICT_SELF  10357
#define BIBLE_MEMFD_SECRET      10358
#define BIBLE_PROCESS_MRELEASE  10359
#define BIBLE_FUTEX_WAITV       10360
#define BIBLE_SET_MEMPOLICY_HOME_NODE 10361
#define BIBLE_PROCESS_MRELEASE       10362
#define BIBLE_FUTEX_WAITV            10363
#define BIBLE_SET_MEMPOLICY_HOME_NODE 10364
#define BIBLE_CACHESTAT              10365
#define BIBLE_FCHMODAT2              10366
#define BIBLE_MAP_SHADOW_STACK       10367
#define BIBLE_FUTEX_WAKE             10368
#define BIBLE_FUTEX_WAIT             10369
#define BIBLE_FUTEX_REQUEUE          10370
#define BIBLE_STATX_ATTRIBUTES       10371
#define BIBLE_MKNOD_IDMAP            10372
#define BIBLE_MKDIR_IDMAP            10373
#define BIBLE_MOUNT_IDMAP            10374
#define BIBLE_FCHOWN_IDMAP           10375
#define BIBLE_FCHMOD_IDMAP           10376
#define BIBLE_OPEN_TREE_IDMAP        10377
#define BIBLE_FSCONFIG_IDMAP         10378
#define BIBLE_PREADV3                10379
#define BIBLE_PWRITEV3               10380
#define BIBLE_GUMBO_CALL             10381
#define BIBLE_SGETMASK               10382
#define BIBLE_SSETMASK               10383
#define BIBLE_QUERY_MODULE_OLD       10384
#define BIBLE_GET_KERNEL_SYMS_OLD    10385
#define BIBLE_CREATE_MODULE_OLD      10386
#define BIBLE_NFSSERVCTL_OLD         10387
#define BIBLE_GETPMSG_OLD            10388
#define BIBLE_PUTPMSG_OLD            10389
#define BIBLE_AFS_SYSCALL_OLD        10390
#define BIBLE_TUXCALL_OLD            10391
#define BIBLE_SECURITY_OLD           10392
#define BIBLE_BREAK                  10393
#define BIBLE_STIME                  10394
#define BIBLE_GDT_SET                10395
#define BIBLE_GDT_GET                10396
#define BIBLE_LDT_SET                10397
#define BIBLE_LDT_GET                10398
#define BIBLE_IOPERM_SET             10399
#define BIBLE_IOPL_SET               10400
#define BIBLE_FSTAT_OLD              10401
#define BIBLE_STAT_OLD               10402
#define BIBLE_LSTAT_OLD              10403
#define BIBLE_UNAME_OLD              10404
#define BIBLE_GETRLIMIT_OLD          10405
#define BIBLE_SETRLIMIT_OLD          10406
#define BIBLE_GETRUSAGE_OLD          10407
#define BIBLE_SIGPENDING_OLD         10408
#define BIBLE_SIGPROCMASK_OLD        10409
#define BIBLE_SIGACTION_OLD          10410
#define BIBLE_SIGRETURN_OLD          10411
#define BIBLE_IPC_OLD                10412
#define BIBLE_SOCKETCALL             10413
#define BIBLE_WAITPID                10414
#define BIBLE_BDFLUSH                10415
#define BIBLE_SETFSGID_OLD           10416
#define BIBLE_SETFSUID_OLD           10417
#define BIBLE_GETDENTS_OLD           10418
#define BIBLE_SELECT_OLD             10419
#define BIBLE_PIPE_OLD               10420
#define BIBLE_MMAP_OLD               10421
#define BIBLE_PAUSE_OLD              10422
#define BIBLE_DEBUG_CALL             10423
#define BIBLE_KLOG_CTL               10424
#define BIBLE_SYS_TRACE              10425
#define BIBLE_VHANGUP_CALL           10426
#define BIBLE_IDLE_CALL              10427
#define BIBLE_VM86_OLD               10428
#define BIBLE_VM86_NEW               10429
#define BIBLE_SVR4_SYSCALL           10430
#define BIBLE_BSD_SYSCALL            10431
#define BIBLE_OSF_SYSCALL            10432
#define BIBLE_RT_SIGSUSPEND_OLD      10433
#define BIBLE_RT_SIGTIMEDWAIT_OLD    10434
#define BIBLE_RT_SIGQUEUEINFO_OLD    10435
#define BIBLE_RT_SIGPENDING_OLD      10436
#define BIBLE_RT_SIGPROCMASK_NEW     10437
#define BIBLE_RT_SIGACTION_NEW       10438
#define BIBLE_PREAD_OLD              10439
#define BIBLE_PWRITE_OLD             10440
#define BIBLE_CHOWN_OLD              10441
#define BIBLE_GETEUID_OLD            10442
#define BIBLE_GETEGID_OLD            10443
#define BIBLE_SETREUID_OLD           10444
#define BIBLE_SETREGID_OLD           10445
#define BIBLE_GETGROUPS_OLD          10446
#define BIBLE_SETGROUPS_OLD          10447
#define BIBLE_FCHOWN_OLD             10448
#define BIBLE_SETRESUID_OLD          10449
#define BIBLE_GETRESUID_OLD          10450
#define BIBLE_SETRESGID_OLD          10451
#define BIBLE_GETRESGID_OLD          10452
#define BIBLE_CHOWN32                10453
#define BIBLE_FCHOWN32               10454
#define BIBLE_LCHOWN32               10455
#define BIBLE_SETUID32               10456
#define BIBLE_SETGID32               10457
#define BIBLE_SETREUID32             10458
#define BIBLE_SETREGID32             10459
#define BIBLE_SETRESUID32            10460
#define BIBLE_SETRESGID32            10461
#define BIBLE_GETRESUID32            10462
#define BIBLE_GETRESGID32            10463
#define BIBLE_STIME32                10464
#define BIBLE_TIME32                 10465
#define BIBLE_UTIME32                10466
#define BIBLE_STAT64                 10467
#define BIBLE_LSTAT64                10468
#define BIBLE_FSTAT64                10469
#define BIBLE_FSTATAT64              10470
#define BIBLE_FCNTL64                10471
#define BIBLE_MMAP2                  10472
#define BIBLE_TRUNCATE64             10473
#define BIBLE_FTRUNCATE64            10474
#define BIBLE_SEND_SIGNAL            10475
#define BIBLE_REVOKE                 10476
#define BIBLE_FREVOKE                10477
#define BIBLE_GETFH                  10478
#define BIBLE_FHOPEN                 10479
#define BIBLE_FHSTAT                 10480
#define BIBLE_FHSTATFS               10481
#define BIBLE_PATHCONF               10482
#define BIBLE_FPATHCONF              10483
#define BIBLE_GETDIRENTRIES          10484
#define BIBLE_UNDELETE               10485
#define BIBLE_NSTAT                  10486
#define BIBLE_NFSTAT                 10487
#define BIBLE_NLSTAT                 10488
#define BIBLE_GETFSSTAT              10489
#define BIBLE_STATFS_OLD             10490
#define BIBLE_FSTATFS_OLD            10491
#define BIBLE_GETDOMAINNAME_OLD      10492
#define BIBLE_SETDOMAINNAME_OLD      10493
#define BIBLE_ADJTIMEX_OLD           10494
#define BIBLE_QUOTACTL_OLD           10495
#define BIBLE_EXPORTFS               10496
#define BIBLE_GETXATTR_OLD           10497
#define BIBLE_SETXATTR_OLD           10498
#define BIBLE_LISTXATTR_OLD          10499
#define BIBLE_REMOVEXATTR_OLD        10500
#define BIBLE_FSYNC_RANGE            10501
#define BIBLE_AIOCB_SETUP            10502
#define BIBLE_AIOCB_SUBMIT           10503
#define BIBLE_AIOCB_WAIT             10504
#define BIBLE_AIOCB_CANCEL           10505
#define BIBLE_LIO_LISTIO             10506
#define BIBLE_THRIFT_CALL            10507
#define BIBLE_KDB_ENTER              10508
#define BIBLE_KDB_EXIT               10509
#define BIBLE_VM86_ENTER             10510
#define BIBLE_VM86_EXIT              10511
#define BIBLE_SYSLOG_READ            10512
#define BIBLE_SYSLOG_WRITE           10513
#define BIBLE_PROFILE_START          10514
#define BIBLE_PROFILE_STOP           10515
#define BIBLE_CHMOD_RELATIVE         10516
#define BIBLE_CHOWN_RELATIVE         10517
#define BIBLE_LINK_RELATIVE          10518
#define BIBLE_SYMLINK_RELATIVE       10519
#define BIBLE_RENAME_RELATIVE        10520
#define BIBLE_UNLINK_RELATIVE        10521
#define BIBLE_MKDIR_RELATIVE         10522
#define BIBLE_RMDIR_RELATIVE         10523
#define BIBLE_OPEN_RELATIVE          10524
#define BIBLE_UTIMES_RELATIVE        10525
#define BIBLE_FCHMOD_RELATIVE        10526
#define BIBLE_FCHOWN_RELATIVE        10527
#define BIBLE_FUTIMES_RELATIVE       10528
#define BIBLE_FSTAT_RELATIVE         10529
#define BIBLE_LSTAT_RELATIVE         10530
#define BIBLE_ACCESS_RELATIVE        10531
#define BIBLE_READLINK_RELATIVE      10532
#define BIBLE_MKNOD_RELATIVE         10533
#define BIBLE_CHROOT_RELATIVE        10534
#define BIBLE_GETDENTS_RELATIVE      10535
#define BIBLE_MOUNT_RELATIVE         10536
#define BIBLE_UMOUNT_RELATIVE        10537
#define BIBLE_TRUNCATE_RELATIVE      10538
#define BIBLE_SWAPON_RELATIVE        10539
#define BIBLE_SWAPOFF_RELATIVE       10540
#define BIBLE_REBOOT_RELATIVE        10541
#define BIBLE_SETTIME_RELATIVE       10542
#define BIBLE_GETTIME_RELATIVE       10543
#define BIBLE_ADJTIME_RELATIVE       10544
#define BIBLE_SYS_FSTYP              10545
#define BIBLE_SYS_FREEMEM            10546
#define BIBLE_SYS_FPSIG              10547
#define BIBLE_SYS_FPHW               10548
#define BIBLE_SYS_FPUINFO            10549
#define BIBLE_SYS_FPSTATE            10550
#define BIBLE_SYS_FPEXCEPTION        10551
#define BIBLE_SYS_FPRESTORE          10552
#define BIBLE_SYS_FPSAVE             10553
#define BIBLE_SYS_FPSET              10554
#define BIBLE_SYS_FPGET              10555
#define BIBLE_SYS_FPCLEAR            10556
#define BIBLE_SYS_FPCONTROL          10557
#define BIBLE_SYS_FPSTATUS           10558
#define BIBLE_SYS_FPWAIT             10559
#define BIBLE_SYS_FPCHECK            10560
#define BIBLE_SYS_FPSIGNAL           10561
#define BIBLE_SYS_FPSTACK            10562
#define BIBLE_SYS_FPREG              10563
#define BIBLE_SYS_FPDATA             10564
#define BIBLE_SYS_FPINST             10565
#define BIBLE_SYS_FPADDR             10566
#define BIBLE_SYS_FPMASK             10567
#define BIBLE_SYS_FPCAUSE            10568
#define BIBLE_SYS_FPERR              10569
#define BIBLE_SYS_FPVALID            10570
#define BIBLE_SYS_FPUSER             10571
#define BIBLE_SYS_FPKERN             10572
#define BIBLE_SYS_FPPHYS             10573
#define BIBLE_SYS_FPVIRT             10574
#define BIBLE_SYS_FPBUS              10575
#define BIBLE_SYS_FPIRQ              10576
#define BIBLE_SYS_FPTRAP             10577
#define BIBLE_SYS_FPDUMP             10578
#define BIBLE_SYS_FPLOAD             10579
#define BIBLE_SYS_FPSTORE            10580
#define BIBLE_SYS_FPMOVE             10581
#define BIBLE_SYS_FPCONV             10582
#define BIBLE_SYS_FPADD              10583
#define BIBLE_SYS_FPSUB              10584
#define BIBLE_SYS_FPMUL              10585
#define BIBLE_SYS_FPDIV              10586
#define BIBLE_SYS_FPCMP              10587
#define BIBLE_SYS_FPSQRT             10588
#define BIBLE_SYS_FPABS              10589
#define BIBLE_SYS_FPNNEG             10590
#define BIBLE_SYS_FPIEEE             10591
#define BIBLE_SYS_FPNONIEEE          10592
#define BIBLE_SYS_FPRND              10593
#define BIBLE_SYS_FPEXC              10594
#define BIBLE_SYS_FPDIS              10595
#define BIBLE_SYS_FPENB              10596
#define BIBLE_SYS_FPSETREGS          10597
#define BIBLE_SYS_FPGETREGS          10598
#define BIBLE_SYS_FPSETENV           10599
#define BIBLE_SYS_FPGETENV           10600
#define BIBLE_SYS_FPSETMASK          10601
#define BIBLE_SYS_FPGETMASK          10602
#define BIBLE_SYS_FPSETSTATUS        10603
#define BIBLE_SYS_FPGETSTATUS        10604
#define BIBLE_SYS_FPSETCAUSE         10605
#define BIBLE_SYS_FPGETCAUSE         10606
#define BIBLE_SYS_FPSETERR           10607
#define BIBLE_SYS_FPGETERR           10608
#define BIBLE_SYS_FPSETVALID         10609
#define BIBLE_SYS_FPGETVALID         10610
#define BIBLE_SYS_FPSETUSER          10611
#define BIBLE_SYS_FPGETUSER          10612
#define BIBLE_SYS_FPSETKERN          10613
#define BIBLE_SYS_FPGETKERN          10614
#define BIBLE_SYS_FPSETPHYS          10615
#define BIBLE_SYS_FPGETPHYS          10616
#define BIBLE_SYS_FPSETVIRT          10617
#define BIBLE_SYS_FPGETVIRT          10618
#define BIBLE_SYS_FPSETBUS           10619
#define BIBLE_SYS_FPGETBUS           10620
#define BIBLE_SYS_FPSETIRQ           10621
#define BIBLE_SYS_FPGETIRQ           10622
#define BIBLE_SYS_FPSETTRAP          10623
#define BIBLE_SYS_FPGETTRAP          10624
#define BIBLE_SYS_FPSETDUMP          10625
#define BIBLE_SYS_FPGETDUMP          10626
#define BIBLE_SYS_FPSETLOAD          10627
#define BIBLE_SYS_FPGETLOAD          10628
#define BIBLE_SYS_FPSETSTORE         10629
#define BIBLE_SYS_FPGETSTORE         10630
#define BIBLE_SYS_FPSETMOVE          10631
#define BIBLE_SYS_FPGETMOVE          10632
#define BIBLE_SYS_FPSETCONV          10633
#define BIBLE_SYS_FPGETCONV          10634
#define BIBLE_SYS_FPSETADD           10635
#define BIBLE_SYS_FPGETADD           10636
#define BIBLE_SYS_FPSETSUB           10637
#define BIBLE_SYS_FPGETSUB           10638
#define BIBLE_SYS_FPSETMUL           10639
#define BIBLE_SYS_FPGETMUL           10640
#define BIBLE_SYS_FPSETDIV           10641
#define BIBLE_SYS_FPGETDIV           10642
#define BIBLE_SYS_FPSETCMP           10643
#define BIBLE_SYS_FPGETCMP           10644
#define BIBLE_SYS_FPSETSQRT          10645
#define BIBLE_SYS_FPGETSQRT          10646
#define BIBLE_SYS_FPSETABS           10647
#define BIBLE_SYS_FPGETABS           10648
#define BIBLE_SYS_FPSETNEG           10649
#define BIBLE_SYS_FPGETNEG           10650
#define BIBLE_SYS_FPSETIEEE          10651
#define BIBLE_SYS_FPGETIEEE          10652
#define BIBLE_SYS_FPSETNONIEEE       10653
#define BIBLE_SYS_FPGETNONIEEE       10654
#define BIBLE_SYS_FPSETRND           10655
#define BIBLE_SYS_FPGETRND           10656
#define BIBLE_SYS_FPSETEXC           10657
#define BIBLE_SYS_FPGETEXC           10658
#define BIBLE_SYS_FPSETDIS           10659
#define BIBLE_SYS_FPGETDIS           10660
#define BIBLE_SYS_FPSETENB           10661
#define BIBLE_SYS_FPGETENB           10662
#define BIBLE_SYS_MEM_INIT           10663
#define BIBLE_SYS_MEM_FREE           10664
#define BIBLE_SYS_MEM_ALLOC          10665
#define BIBLE_SYS_MEM_REALLOC        10666
#define BIBLE_SYS_MEM_MAP            10667
#define BIBLE_SYS_MEM_UNMAP          10668
#define BIBLE_SYS_MEM_PROTECT        10669
#define BIBLE_SYS_MEM_LOCK           10670
#define BIBLE_SYS_MEM_UNLOCK         10671
#define BIBLE_SYS_MEM_SYNC           10672
#define BIBLE_SYS_MEM_ADVISE         10673
#define BIBLE_SYS_MEM_MINCORE        10674
#define BIBLE_SYS_MEM_MOVE           10675
#define BIBLE_SYS_MEM_COPY           10676
#define BIBLE_SYS_MEM_FILL           10677
#define BIBLE_SYS_MEM_ZERO           10678
#define BIBLE_SYS_MEM_READ           10679
#define BIBLE_SYS_MEM_WRITE          10680
#define BIBLE_SYS_MEM_SEARCH         10681
#define BIBLE_SYS_MEM_CMP            10682
#define BIBLE_SYS_MEM_DUMP           10683
#define BIBLE_SYS_MEM_LOAD           10684
#define BIBLE_SYS_MEM_STORE          10685
#define BIBLE_SYS_MEM_FLUSH          10686
#define BIBLE_SYS_MEM_INVALIDATE     10687
#define BIBLE_SYS_MEM_PURGE          10688
#define BIBLE_SYS_MEM_COMPRESS       10689
#define BIBLE_SYS_MEM_DECOMPRESS     10690
#define BIBLE_SYS_MEM_ENCRYPT        10691
#define BIBLE_SYS_MEM_DECRYPT        10692
#define BIBLE_SYS_MEM_HASH           10693
#define BIBLE_SYS_MEM_VERIFY         10694
#define BIBLE_SYS_MEM_CHECK          10695
#define BIBLE_SYS_MEM_STATS          10696
#define BIBLE_SYS_MEM_TRACE          10697
#define BIBLE_SYS_MEM_DEBUG          10698
#define BIBLE_SYS_MEM_LOG            10699
#define BIBLE_LAST_CALL              10700
#define __biblead2 __attribute__((__noreturn__))
#define BK_SYSCALL_ERROR_BIT (1ULL << 63)

__BK_BEGIN_DECLS

BK_QUAD bk_syscall(BK_REGISTER number, ...);



#define _BK_SYSCALL_ARG(arg) ((BK_REGISTER)(arg))

#define BIBLE_SYSCALL0(n) \
    bk_syscall(_BK_SYSCALL_ARG(n))

#define BIBLE_SYSCALL1(n, a1) \
    bk_syscall(_BK_SYSCALL_ARG(n), _BK_SYSCALL_ARG(a1))

#define BIBLE_SYSCALL2(n, a1, a2) \
    bk_syscall(_BK_SYSCALL_ARG(n), _BK_SYSCALL_ARG(a1), _BK_SYSCALL_ARG(a2))

#define BIBLE_SYSCALL3(n, a1, a2, a3) \
    bk_syscall(_BK_SYSCALL_ARG(n), _BK_SYSCALL_ARG(a1), _BK_SYSCALL_ARG(a2), \
               _BK_SYSCALL_ARG(a3))

#define BIBLE_SYSCALL4(n, a1, a2, a3, a4) \
    bk_syscall(_BK_SYSCALL_ARG(n), _BK_SYSCALL_ARG(a1), _BK_SYSCALL_ARG(a2), \
               _BK_SYSCALL_ARG(a3), _BK_SYSCALL_ARG(a4))

#define BIBLE_SYSCALL5(n, a1, a2, a3, a4, a5) \
    bk_syscall(_BK_SYSCALL_ARG(n), _BK_SYSCALL_ARG(a1), _BK_SYSCALL_ARG(a2), \
               _BK_SYSCALL_ARG(a3), _BK_SYSCALL_ARG(a4), _BK_SYSCALL_ARG(a5))

#define BIBLE_SYSCALL6(n, a1, a2, a3, a4, a5, a6) \
    bk_syscall(_BK_SYSCALL_ARG(n), _BK_SYSCALL_ARG(a1), _BK_SYSCALL_ARG(a2), \
               _BK_SYSCALL_ARG(a3), _BK_SYSCALL_ARG(a4), _BK_SYSCALL_ARG(a5), \
               _BK_SYSCALL_ARG(a6))


/* Gerenciamento de Processos e Fluxo */
__biblead2 void bk_exit(BK_I32 status);
BK_PID       bk_fproc(void);
BK_PID       bk_getpid(void);
BK_PID       bk_getppid(void);
BK_I32       bk_execve(const char *path, char *const argv[], char *const envp[]);
BK_PID       bk_wait4(BK_PID pid, BK_I32 *status, BK_I32 options, void *rusage);
BK_I32       bk_kill(BK_PID pid, BK_I32 sig);
BK_I32       bk_getpriority(BK_I32 which, BK_ID who);
BK_I32       bk_setpriority(BK_I32 which, BK_ID who, BK_I32 prio);

/* Sistema de Arquivos e I/O */
BK_I32       bk_open(const char *path, BK_I32 flags, BK_MODE mode);
BK_I32       bk_close(BK_I32 fd);
BK_SSIZE     bk_read(BK_I32 fd, void *buf, BK_SIZE nbyte);
BK_SSIZE     bk_write(BK_I32 fd, const void *buf, BK_SIZE nbyte);
BK_SSIZE     bk_pread(BK_I32 fd, void *buf, BK_SIZE nbyte, BK_OFF offset);
BK_SSIZE     bk_pwrite(BK_I32 fd, const void *buf, BK_SIZE nbyte, BK_OFF offset);
BK_OFF       bk_lseek(BK_I32 fd, BK_OFF offset, BK_I32 whence);
BK_I32       bk_stat(const char *path, struct bk_stat *sb);
BK_I32       bk_fstat(BK_I32 fd, struct bk_stat *sb);
BK_I32       bk_lstat(const char *path, struct bk_stat *sb);
BK_I32       bk_unlink(const char *path);
BK_I32       bk_chdir(const char *path);
BK_I32       bk_fchdir(BK_I32 fd);
BK_I32       bk_mkdir(const char *path, BK_MODE mode);
BK_I32       bk_rmdir(const char *path);
BK_I32       bk_dup(BK_I32 oldfd);
BK_I32       bk_dup2(BK_I32 oldfd, BK_I32 newfd);
BK_I32       bk_pipe(BK_I32 fildes[2]);
BK_I32       bk_access(const char *path, BK_I32 amode);
BK_I32       bk_sync(void);

/* Gerenciamento de Memória */
void * bk_mmap(void *addr, BK_SIZE len, BK_I32 prot, BK_I32 flags, BK_I32 fd, BK_OFF offset);
BK_I32       bk_munmap(void *addr, BK_SIZE len);
BK_I32       bk_mprotect(void *addr, BK_SIZE len, BK_I32 prot);
BK_I32       bk_minherit(void *addr, BK_SIZE len, BK_I32 inherit);
BK_I32       bk_madvise(void *addr, BK_SIZE len, BK_I32 behav);
BK_I32       bk_msync(void *addr, BK_SIZE len, BK_I32 flags);

/* Atributos de Usuário e Grupos */
BK_UID       bk_getuid(void);
BK_UID       bk_geteuid(void);
BK_GID       bk_getgid(void);
BK_GID       bk_getegid(void);
BK_I32       bk_setuid(BK_UID uid);
BK_I32       bk_setgid(BK_GID gid);

/* Tempo e Sincronização */
BK_I32       bk_nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
BK_I32       bk_gettimeofday(struct timeval *tp, struct timezone *tzp);
BK_I32       bk_clock_gettime(BK_CLOCKID clock_id, struct timespec *tp);

/* Networking e Sockets */
BK_I32       bk_socket(BK_I32 domain, BK_I32 type, BK_I32 protocol);
BK_I32       bk_bind(BK_I32 s, const struct bk_sockaddr *name, __SOCKLEN_KT namelen);
BK_I32       bk_listen(BK_I32 s, BK_I32 backlog);
BK_I32       bk_accept(BK_I32 s, struct bk_sockaddr *addr, __SOCKLEN_KT *addrlen);
BK_I32       bk_connect(BK_I32 s, const struct bk_sockaddr *name, __SOCKLEN_KT namelen);

__BK_END_DECLS
#endif /* _BIBLE_SYSCALLS_H */
