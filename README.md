# UMExamples
UMExamples provides sample applications for users of Ultra Messaging

Go to https://ultramessaging.github.io/UMExamples/ so see them rendered
and user-friendly.


## Semlit

The examples use a documentation toolset called "semlit".
Go to https://github.com/fordsfords/semlit to get it.
Place ``semlit.sh`` and ``semlit.pl`` into your PATH.


## Creating Examples

Here is how I created the "ss_frag" example:

```mkdir ss_frag
cd ss_frag
mkdir c
cp ../cancelling_timers/README.txt .
vi README.txt```

Change appropriately.

```cp ../cancelling_timers/intro.sldoc .
vi intro.sldoc```

Change appropriately.

```cp ../cancelling_timers/c/cancelling_timers.sldoc c/ss_frag.sldoc
cp ../cancelling_timers/c/cancelling_timers_c.slsrc c/ss_frag_c.slsrc
cd c
vi ss_frag.sldoc```

In a separate window:

```cd c
vi ss_frag_c.slsrc```

