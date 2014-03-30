# Abstract

*anmem* combines my three other projects, [analloc](https://github.com/unixpickle/analloc), [anpages](https://github.com/unixpickle/anpages), and [anlock](https://github.com/unixpickle/analloc) to make a multipurpose memory allocator for operating system kernels. This allocator provides page allocation for basic kernel structures, aligned block allocation for PCI buffers, and dynamic, easy to use free routines.

# Requirements

In order for *anmem* to work to its fullest capacity, you must use a kernel virtual memory mapping such that the system's physical memory is mapped (with no gaps) to linear virtual memory. Although *anmem* deals with a virtual memory mapping, it uses the information you pass it to provide you with aligned *physical* addresses.

# Subtrees

Subtrees were added like the following example for `analloc`

    git remote add analloc https://github.com/unixpickle/analloc.git
    git fetch analloc
    git checkout -b analloc analloc/master
    git checkout master
    git read-tree --prefix=libs/analloc -u analloc

Update subtrees as follows:

    git checkout analloc
    git pull
    git checkout master
    git merge --squash -s subtree --no-commit analloc
