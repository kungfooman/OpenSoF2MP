squashed commits of sof2 with splitted patches

```
# rebase all of kneirincks commits into one
# the nano editor should open, then you need to replace every "pick" with "squash"
# keycombo is: alt+r [enter] pick [enter] squash [enter]
# save file with: ctrl+x y [enter]
git rebase -i 7732cf93

# generate one big .patch file from last commit
git format-patch -1 HEAD

# split the patch into many files
apt-get install splitpatch
splitpatch 0001-Base-to-get-a-somewhat-working-UI.patch
```
