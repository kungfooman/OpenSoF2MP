This engine is currently capable of:
 - start a map e.g. via `map mp_shop` and play on it, but it crashes once you shoot a bullet (some netmsg crash)
 - however, you can compile this ioquake3-sof2 engine https://github.com/sof2plus/sof2plus-engine and play on it nicely
 
Current issues:
 - cg_thirdperson 1, the player won't have a weapon in his hands

To compile sof2plus engine/game, just do:
 
```
mkdir ~/sof2plus
cd ~/sof2plus

git clone https://github.com/sof2plus/sof2plus-engine engine
cd engine; make; cd ..

git clone https://github.com/sof2plus/sof2plus-game game
cd game; make; cd ..

# now we compiled all the needed binaries

# copy sof2plus executable in sof2plus directory
cp engine/build/release-linux-x86_64/sof2plus.x86_64 .

# you have to put all the original .pk3 files into this directory
mkdir base

# now copy all the .so files into base directory
cp game/build/release-linux-x86_64/*.so base

# start the server like:
./sof2plus.x86_64 +set sv_pure 0 +rconPassword asd +devmap mp_shop
```

If you want to run your SoF2 server permanently you can use `screen`:

```
apt-get install screen
screen -AmdS sof2
screen -x sof2
./sof2plus.x86_64 +set sv_pure 0 +rconPassword asd +devmap mp_shop
# your terminal is inside the screen now, you can leave with hold-ctrl-key + a + d
```

Compile/build instructions:
 - simply clone or download this repo
 - open OpenSoF2MP\codemp\OpenJK.sln with Visual Studio
 - right-click on SoF2MP project and fix the command line argument in the "Debugging" section
    - e.g. to: `+set fs_basepath F:\sof2_gold +set sv_pure 0`
 - just press F5 to run it

Old/original description of kneirinck:

Getting Soldier of Fortune 2 Multiplayer to work by using a heavily modified JK engine.  
Progress can be found at [http://omgwtflol.rivercrew.net](http://omgwtflol.rivercrew.net).
