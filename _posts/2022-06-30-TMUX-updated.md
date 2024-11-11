---
layout: post
title: TMUX
categories: Coding
tags: console
year: 2022
month: 07
day: 1
published: true
author: Gustavo Walbon
summary: TMUX, after a while of using it.
---

With the daily usage of TMUX after a couple of years, I have my tips to install and customize it to help in the time of daily job.

Usually I have to open a new window into a guest to perform some tasks to test and check the installation of a new fresh machine, so tmux is welcomed when there is limited access for terminals for running parallel and independent jobs.

## Tmuxes
So, my localhost could use the PREFIX as CTRL+A because it is more comfortable for me, now the guests with new fresh installation should use the default CTRL+B.

How do you do that? Adding the following command into `~/.tmux.conf`
```
 #using C-a as prefix 
unbind C-b
set-option -g prefix C-a                                                                                      bind C-a send-prefix
``` 
## Split and conquer
Why not open a pane with the content from another window. Split the pane vertically/horizontally and choose the other window.
from `~/.tmux.conf`
```
# split and join other pane
bind < split-window -h \; choose-window 'kill-pane ; join-pane -hs %%'
bind = split-window -v \; choose-window 'kill-pane ; join-pane -vs %%'
```
## Like a browser
For navigation it could add the shortcuts to access the windows as in a browser ALT+Number and Shift+Arrow(Left,Right) to move through the windows.
```
# Shift arrow to switch windows
bind -n S-Left  previous-window
bind -n S-Right next-window
```
# Choosing window ALT+Number like a browser


```
bind -n M-0 select-window -t 0
bind -n M-1 select-window -t 1
bind -n M-2 select-window -t 2
bind -n M-3 select-window -t 3
bind -n M-4 select-window -t 4
bind -n M-5 select-window -t 5
bind -n M-6 select-window -t 6
bind -n M-7 select-window -t 7
bind -n M-8 select-window -t 8
bind -n M-9 select-window -t 9
``` 

That all are the most useful tricks I have used.




