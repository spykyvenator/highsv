# Highsv

Is a graphical front-end for HiGHs linear model solving

## Licence

highsv is licenced under the GPLV3 licence, a copy has been shipped with your distro, or you can view it here: [gplv3](https://www.gnu.org/licenses/gpl-3.0.en.html)

## Project

is in early development at the moment, any help appreciated

## Usage
### syntax

Files should start with min or max.
This is followed by the objective function's variables separated by either + or - and can end with an optional offset.
Variables can be separated by spaces, newlines, tabs, + and - so x1+x2 is the same as x1 + x2.
Values should start either with a number or a . for the comma so .5 0.5 and 5 are all three correct.
Variables must start with a letter and can contain numbers commas and slashes.

Separating the objective function from the constraints is st or subject to.
The constraints are again variables followed by a comparator (< or <= or =< or > or >= or =>) and the value compared against.
Constraints are best separated by newlines but spaces or tabs will work just fine.

The constraints are optionally followed by end which marks the last line of the model.
Everything after this will be ignored.
End should be written at the beginning of a line, so no tabs or spaces can be in front of it.

Comments start with ! and end with newlines
No multiplication mark can be used but values can be separated from the variable or written right next to it so both 1.1x1 and 1.1 x1 are correct.
"min", "max" and "st" or "subject to" have ignored case, end should be written lowercase at the moment.

If this sounds complicated, maybe the example below can help you get started.

### Example

```
max 0.5x1 + 2x2 + 4 ! this is a comment
st ! or subject to
0.5x1 < 2 ! 1/2 times x1 should be smaller than 2
x2 + x1 < 3
.123 x2 - 5 x1 <= 100
x2 >= 3
end
everything after end is ignored
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
