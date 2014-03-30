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
