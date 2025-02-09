# Highsv

Is a graphical front-end for HiGHs linear model solving

## Licence

highsv is licenced under the GPLV3 licence, a copy has been shipped with your distro, or you can view it here: [gplv3](https://www.gnu.org/licenses/gpl-3.0.en.html)

## Project

is in early development at the moment, any help appreciated

## Example

```
max 0.5x1 + 2x2! maximize for one and a half times x1 and two times x2
st! or subject to
x1 < 2!x1 should be smaller than 2
x2 + x1 < 3! x2 and x1 together need to be smaller than 3
x2 - x1 <= 100! x2 minus x1 needs to be smaller than 100
x2 >= 3! x2 needs to be larger than or equal to 2
```

## Installation

### Windows:

Download the [setup](https://github.com/spykyvenator/highsv/releases/download/alpha/setup_highsv.exe) from [releases](https://github.com/spykyvenator/highsv/releases)
You might get a pop up from windows flagging the setup as malware, select "More info" and then "Run anyway"

### Linux:

Download the [tar archive](https://github.com/spykyvenator/highsv/releases/download/alpha/highsv.tar.gz) from [releases](https://github.com/spykyvenator/highsv/releases)
Running the build/highsv should work (I only briefly tested this)

## Build from source

You will need
* [make](https://www.gnu.org/software/make/)
* [HiGHS](https://highs.dev/)
* [flex](https://sourceforge.net/projects/flex/)
* [gtk4](https://www.gtk.org/)
* C library

all these except probably HiGHS are provided with your distro (Windows users can use [MSYS2](https://www.msys2.org/) and compile using clang64)
Once these are installed simply running ```make``` should build the gtk app and copy it to ./lps
