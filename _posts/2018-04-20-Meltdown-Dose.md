---
layout: post
title: Meltdown Dose
categories: Coding
tags: ubuntu git
year: 2018
month: 04
day: 20
permalink: meltdown
---
# Disclaimer
The related information here have as source from the posts and articles from
LWN, Kernel.org and the available papers [[0]](https://meltdownattack.com/)
 and I write about the PowerPc Arch just as far I know and work.

# The beggining
Almost 10 years ago the CPU development got a new way to speed up the processing
and execution of instructions in queue of processors, the branch prediction,
_"Branch predictors try to determine which direction of a branch will be taken
before its condition is actually evaluated."_ [[1]](https://meltdownattack.com/meltdown.pdf)
So, that seems an re-ordering of instructions through aspecing of theirs possible
 execution given its possibilites to execute the most recurrent. Well, that’s
where the problem beggining.

## What the problem prefetching an instruction ?
No problem at most of the time, the problem is when a set of instructions load
a priviledge memory data from memory and let it available in the LD1 cache and
a speculative access to try read them.

The processor just have access to read data from the LD1 where all of other
memory must be loaded and recovery here, and so with an paralel access to LD1
cache and copying it and waiting for the next side channel attack.


## How to exploit it
Simplifying, it’s possible to training the branch prediction in favor of your
exploitation when you force the code claim for a syscall(kernel space) before an
inapropriate access to kernel memory space that it will be avoided.

## Solution
The most common fix by the maintainers of the archás is a preventive cleaning of
cache before the returning from interrupting between kernel space and user
space.

# Commenting the patcheset

## RFI(Return From Interrupt)  Macros
First overall we need to identify which kind of return of the interruption and
where it came if from a userspace or kernel space and/or from a
guest(hypervisor) to choose the type of return it will be used.

[[2]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=50e51c13b3822d14ff6df4279423e4b7b2269bc3) Created eight types which the type depends on of destiny of the returning(to
kernel space, to user space, to guest) and some others depends too of origin if
hypervisor or not.

[[3]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=222f20f140623ef6033491d0103ee0875fe87d35) Here the patch is applied to change each kind of situation needs and use the
RFI Macros previous created

## Converting Macros

[[4]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=b8e90cb7bc04a509e821e82ab6ed7a8ef11ba333)
That patch insert the RFI macro and other treatments to return to user space
using **RFI_TO_USER** macro and its pre-requests.

For situations of a kernel space returning the **RFI_TO_KERNEL** macro to return to
kernel space without needs special preps.

```
+   b   .   /* prevent speculative execution */
```
This ASM line creates a load as a uncondicional branch for the own function
address(b .) but it never will be used because before the **RFI_TO_USER** just
returns, it's a kind of speculative barrier.


Boot message during the rfi-flush patching:
```
[    0.000000] rfi-flush: mttrig type flush available
[    0.000000] rfi-flush: patched 9 locations (mttrig type flush)
```
[[5]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c7305645eb0c1621351cfc104038831ae87c0053)
 In this patch there is a preventing as a backup of status some structs from/and PACA struct, sure if depends on the kind of returning to user or to kernel space.
```
+   RESTORE_CTR(r9, PACA_EXSLB)
+   RESTORE_PPR_PACA(PACA_EXSLB, r9)
+   mr  r3,r12
+   ld  r9,PACA_EXSLB+EX_R9(r13)
+   ld  r10,PACA_EXSLB+EX_R10(r13)
+   ld  r11,PACA_EXSLB+EX_R11(r13)
+   ld  r12,PACA_EXSLB+EX_R12(r13)
+   ld  r13,PACA_EXSLB+EX_R13(r13)
+   RFI_TO_USER
+   b   .   /* prevent speculative execution */
```

Kernel space does not need to add pre reqs.
```
+   RFI_TO_KERNEL
```

[[6]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=aa8a5e0062ac940f7659394f4817c948dc8c0667) 
This time the already defined RFI in [[2]] were overwritten to use this new
**RFI_FLUSH_SLOT** engine, but looking at the its implementation is a simple 
couple of *nop*, ... so wtf(fantastic?) is doing? Nothing? OMG.
```
+ /*
+ * The nop instructions allow us to insert one or more instructions to flush the
+ * L1-D cache when returning to userspace or a guest.
+ */
+#define RFI_FLUSH_SLOT							\
+	RFI_FLUSH_FIXUP_SECTION;					\
+	nop;								\
+	nop;								\
+	nop
```
WAIT! It’s is part of magic of this patchset, we’ll see later in that patch what
it’ll happen.

- **enum l1d_flush_type** defines the four types of flushing of LD1 cache.
1. **L1D_FLUSH_NONE** : Flush disabled.
2. **L1D_FLUSH_FALLBACK** : Flush via a fallback.
3. **L1D_FLUSH_ORI** : Flush using a couple of OR imediate operant itself.
4. **L1D_FLUSH_MTTRIG** : Through a special operator for the Power Arch.

**TRAMP_REAL_BEGIN** starts a section to save some important
registers/structures as **PACA**, at beginnig with **SET_SCRATCH0** to backup the
register **r13** and so to be used getting the **PACA** and handle their infos.
At ending of this section the it back the content of **r13** from backup.

- **init_fallback_flush** : This fallback gets the sizeof LD1 cache to write 0's(zeros) in
  the cache memory space through its engines.

- **do_rfi_flush_fixups** (the magic): Do you remeber at beggining of this
  patch with that three **nop**'s ? This is the local to be set up at boot time.

The array of instructions **instrs[3]** is reserved to replace that three
**nop**'s with flushing and depends on flush type which is set up by the firmware of machine.
```
+   instrs[0] = 0x60000000; /* nop */
+   instrs[1] = 0x60000000; /* nop */
+   instrs[2] = 0x60000000; /* nop */
```
This piece of patch replaces in the memory space the three **nop**’s, which it
happens at boot time.
```
patch_instruction(dest, instrs[0]);
+       patch_instruction(dest + 1, instrs[1]);
+       patch_instruction(dest + 2, instrs[2]);
```

That section defines as a flush_fallback jumps to 2 instructions
after(after third **nop**) and use the (h)rfid.
```
+   if (types & L1D_FLUSH_FALLBACK)
+       /* b .+16 to fallback flush */
+       instrs[0] = 0x48000010;
```

Replaces with two _OR imediate operant itself_ if the machine is setup to use
this type of flush.
```
+   i = 0;
+   if (types & L1D_FLUSH_ORI) {
+       instrs[i++] = 0x63ff0000; /* ori 31,31,0 speculation barrier */
+       instrs[i++] = 0x63de0000; /* ori 30,30,0 L1d flush*/
+   }
```
In case of support by machine to setup this special kind of flush.
```
+   if (types & L1D_FLUSH_MTTRIG)
+       instrs[i++] = 0x7c12dba6; /* mtspr TRIG2,r0 (SPR #882) */
```

[[7]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=bc9c9304a45480797e13a8e1df96ffcf44fb62fe)
After all you can disable the flush via sysfs if you are secure to disable it,
and reduce the impact of flush in each context switch.
```
# echo 0 > /sys/kernel/debug/powerpc/rfi_flush

#  cat /sys/devices/system/cpu/vulnerabilities/meltdown
Vulnerable
```

# References

[[0] https://meltdownattack.com/](https://meltdownattack.com/)

[[1] https://meltdownattack.com/meltdown.pdf](https://meltdownattack.com/meltdown.pdf)

[[2] https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=50e51c13b3822d14ff6df4279423e4b7b2269bc3](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=50e51c13b3822d14ff6df4279423e4b7b2269bc3)

[[3]
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=222f20f140623ef6033491d0103ee0875fe87d35](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=222f20f140623ef6033491d0103ee0875fe87d35)

[[4] https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=b8e90cb7bc04a509e821e82ab6ed7a8ef11ba333](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=b8e90cb7bc04a509e821e82ab6ed7a8ef11ba333)

[[5] https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c7305645eb0c1621351cfc104038831ae87c0053](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=c7305645eb0c1621351cfc104038831ae87c0053)

[[6]
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=aa8a5e0062ac940f7659394f4817c948dc8c0667](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=aa8a5e0062ac940f7659394f4817c948dc8c0667)

[[7]
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=bc9c9304a45480797e13a8e1df96ffcf44fb62fe](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=bc9c9304a45480797e13a8e1df96ffcf44fb62fe)

firmware support (powervm)
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=8989d56878a7735dfdb234707a2fee6faf631085
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=191eccb1580939fb0d47deb405b82a85b0379070

firmware support (powernv)
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/?id=6e032b350cd1fdb830f18f8320ef0e13b4e24094
(edited)


