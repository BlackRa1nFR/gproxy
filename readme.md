
## GPROXY - `CITIZENHACK BEFORE IT EXISTED`
GProxy is an open source lua proxy for Garry's Mod. The goal is to allow full client state access from menu state.

GProxy open sourced the idea that the CitizenHack team took to turn a quick profit off children. The GProxy team are now taking back it's rightful throne as the best and most open lua state proxy. All free of charge.

## What is this?
GProxy allows you to run any lua you normally could run, but safer. It runs clientstate code in the menu state. 

## Why is GProxy open source?
GProxy was open sourced when MeepDarknessMeep decided they didn't have any reason to keep it private. It is now open for public changes and forks as well as future developments and problem solving. It was MeepDarknessMeep's belief that cheats should not be pay-for and definitely not subscription based. It will always be open source.

## How do I contribute?
Fork this and request a change. 

## How do I build this?
To build, you need [premake5](https://premake.github.io/download.html). Add that to your path and run `premake5 vs2015`. Change 2015 to whatever version you use currently. This will generate a `sln` file in the `project/` directory.

## Can I just download this?
[Yes.](https://github.com/meepdarknessmeep/gproxy/releases)

## How do I run it?
Currently, GProxy runs on Windows only and can be loaded through `require` in the menu state or by injecting it into Garry's Mod.

After you run the first time, a config file is created in your home directory. You can get there by typing `%USERPROFILE%` in Windows explorer. Edit the file to a folder where your lua will be ran from. Create a file `init.lua` in that folder you just created and edit the config in your home directory to point to that path.