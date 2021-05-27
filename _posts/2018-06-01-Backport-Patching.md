---
layout: post
title: Backporting patches
categories: Coding
tags: kernel git suse
year: 2018
month: 11
day: 27
published: false
---


I had chose randomly a single patch to explain the some ways I use to apply a
backporting patch. I have some tips which I am going to explain here, using
tools like git(<a title="I love it">😏</a>), vim, vimdiff and meld.
The choose was this:
[[1]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/mm?id=0f901dcbc31f88ae41a2aaa365f7802b5d520a28)
and the patch is to fix the kansas(KernelAddressSANitizer) tool
[[2]](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/dev-tools/kasan.rst)




# References
[[1]
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/mm?id=0f901dcbc31f88ae41a2aaa365f7802b5d520a28](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/commit/mm?id=0f901dcbc31f88ae41a2aaa365f7802b5d520a28)

[[2]
https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/dev-tools/kasan.rst](https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/Documentation/dev-tools/kasan.rst)


