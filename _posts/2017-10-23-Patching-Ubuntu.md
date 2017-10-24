---
layout: post
title: Patching Ubuntu Kernel
categories: Coding
tags: ubuntu git
year: 2017
month: 10
day: 23
---

## Obtain ther kernel source
Which distro? Which version?
```
git clone git://kernel.ubuntu.com/ubuntu/ubuntu-artful.git
```
After cloning or updating its folder already cloned.
```
cd ubuntu-artful
git clone fetch --all --tags
```

Choice a bug number to identify the job you are working, for example _bug12345_, and so create a branch to track it. A tip, use a github interface to archive and
work.
After alreay created a repository in the github, you can add a remote branch from your computer to send them to github.

```
git remote add github https://github.com/walbon/ubuntu-xenial.git
git checkout -b bug12345 origin/master-next
git push -u github bug12345
```
Why master-next? Well, to align with the latest updates from the kernel team then to suggest your updates.

## Identify which patches are needed
Width the main patches on hands already tracked on the Linus tree, next steps are
using cherry-pick or format-patch.
Order them since the oldest to youngest to apply them.
</ul>

## Git Cherry-pick [[1]](https://git-scm.com/docs/git-cherry-pick)
When you use the this the command will bring the modifies and the needed files
from other commits to complete it. Look the sample above.

### Sample
```
#     (G)+stuff1.c
#      |
#     (F)<-----(E)+Modified stuff1.c
#      |        |
#     (D)      (C)+Created stuff1.c and stuff2.c
#      \       /
# Master\     / Feature
# Branch  (B)   Branch
#          |
#         (A)

# I'm in the HASH_F
git cherry-pick HASH_E
# Resolving some possible conflicts(using git merge tool), because the brach master does
# not have the stuff1.c file and the cherry-pick will bring it alone instead of all HASH_C.
git cherry-pick --continue
# Finish
ls
...
stuff1.c
...

```

## Git Format-patch [[2]](https://git-scm.com/docs/git-format-patch)
My format-patch way is more hands-on, because via `git format-patch ` I extracted the the patch in that especific patch and just test it on the current branch to apply them.

```
git format-patch -k -1 --stdout HASH_E | git am -k
Applying: Modified stuff1.c
error: stuff1.c1: does not exist in index
Patch failed at 0001 Modified stuff1.c
The copy of the patch that failed is found in: .git/rebase-apply/patch
When you have resolved this problem, run "git am --continue".
If you prefer to skip this patch, run "git am --skip" instead.
To restore the original branch and stop patching, run "git am --abort".

#Conflicts ^o^
#So, you should add by hand the stuff1.c(opt 1) or to bring the HASH_C then HASH_E(opt 2)
##Option 1
###pushing the stuff1.c one step before the HASH_E
git checkout HASH_C stuff1.c
###retry the patching
git am --continue
Applying: Modified stuff1.c

ls
...
stuff1.c
...
#FINISH

##Option 2
###cancel the am and so add the patch that add the stuff1.c file.
git am --abort # to cancel the current status of git am
git format-patch -k -1 --stdout HASH_C | git am -k
Applying: Created stuff1.c and stuff2.c

git format-patch -k -1 --stdout HASH_E | git am -k
Applying: Modified stuff1.c

ls
...
stuff1.c
stuff2.c
...
#FINISH

```

## Sending patches to the Kernel Team Maillist

First, adopt some tips which they are related to send-email from [Git Tips [3]]({{ site.url }}/git).
Now, you must create a bug tracking in the
Launchpad [[4]](https://help.ubuntu.com/community/ReportingBugs)
and add its link in each patch of your backport, I suggest git rebase to re-edit
the commits, look:
```
git rebase -i HEAD~N # N= number of commits to be edited
  1 pick 39943c4 +A
  2 pick febba76 +B
  3 pick 2410879 Modified stuff1.c
  4 ¬
  5 # Rebase 19b82d4..2410879 onto 19b82d4 (3 commands)¬
  6 #¬
  7 # Commands:¬
  8 # p, pick = use commit¬
  9 # r, reword = use commit, but edit the commit message¬
 10 # e, edit = use commit, but stop for amending¬
 11 # s, squash = use commit, but meld into previous commit¬
 12 # f, fixup = like "squash", but discard this commit's log message¬
 13 # x, exec = run command (the rest of the line) using shell¬
 14 # d, drop = remove commit¬
 15 #¬
 16 # These lines can be re-ordered; they are executed from top to bottom.¬
 17 #¬
 18 # If you remove a line here THAT COMMIT WILL BE LOST.¬
 19 #¬
 20 # However, if you remove everything, the rebase will be aborted.¬
 21 #¬
 22 # Note that empty commits are commented out¬
:wq
#Stopped at 2410879...  Modified stuff1.c
#You can amend the commit now, with
git commit --amend
#Added its http://bugs.launchpad.net/bugs/<bug-id> in the third line of
#commit, it must be the first line of describe message.
...
#Once you are satisfied with your changes, run
git rebase --continue
...
#Then generate the patche files.
git format-patch origin/master-next.. --cover-letter --subject-prefix="Artful][PATCH"
...
# Edit and describe the cover-letter and do not forget to add the Buglink there.
vim 0000-cover-letter.patch
...

#Sent them to kernel mail list.
git send-email --to kernel-team@lists.ubuntu.com 00*patch

```
Example :
[https://lists.ubuntu.com/archives/kernel-team/2017-May/084443.html](https://lists.ubuntu.com/archives/kernel-team/2017-May/084443.html)


# References
- [1][https://git-scm.com/docs/git-cherry-pick](https://git-scm.com/docs/git-cherry-pick)
- [2][https://git-scm.com/docs/git-format-patch](https://git-scm.com/docs/git-format-patch)
- [3][{{ site.url }}/git]( {{ site.url }}/git )
- [4][https://wiki.ubuntu.com/Kernel/Dev/StablePatchFormat](https://wiki.ubuntu.com/Kernel/Dev/StablePatchFormat)


***
