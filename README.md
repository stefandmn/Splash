# Clue Splash

**Splash** is a graphical term for representation of boot process, developed especially for _Clue OS_
('the father' of this project) but might be applicable to any Linux system. **Splash** application is
currently use to control the boot process of the _Clue OS_ (_Debian Jessie_ fork) and to implement
a graphical view of different utilities from **Setup** project (part of **Clue** main project as main
utility for system configuration).
**Splash** can display graphics (JPG image format and others) or some combinations of graphics with
text messages (in graphical format). Splash program uses a stateful memory buffer and is able to
initiate and maintain the workflow even is the process is not running in loop.
Typically, the workflow for boot or other management screens is initiated in InitRAM area, creating
a dedicated graphical console and then, other workflow steps could be launched (by individual process
execution) that will use existing graphical console and updates display content: display new splashes
or text messages.

Splash program is designed to support the following feature:

1. `--image`   = prints a splash image on the graphical term.
2. `--message` = displays a text message on the graphical screen.
3. `--shape`   = draws graphical shapes and basic controls for data display, loading, etc.
4. `--exit`    = exit from the splash application. It accepts also a delay as an optional parameter

For the syntax details of each option please see **Running** chapter.

> **Note**: One one option could be specified for a workflow step (program execution)

# Prerequisites
In order to build `clue-splash` package from source code you have to install required packages mentioned
below. **Splash** is a static-compiled application, so it is not need to install additional libraries or
package to run it or to bundle it in your ___RamFS___ image.

`apt-get install -y gcc uthash-dev libjpeg62-dev libfreetype6-dev libpng12-dev`

# Running
As a general structure, each drawing command has the following componwents:
1. initial coordinates - an (x,y) point where the object is located on the screen
2. type of the object - describes what type of object will be drawn on the screen; the options have been
mentioned above:
`image`, `shape` and `message`.
3. graphical formating options - graphical properties declared through `--properties` option to define the
format of the object that is going to be drawn or displayed
4. display options - dedicated options  that allows you to build __an workflow__ using consecutive execution
command of Splash application. With this option you can memorize object to use them from one session to another
without to repeat the command, to overdrawn graphical objects from the previous location, etc. - all these
options are explained in ___Display Options___ chapter.

## Messages
To display **text messages** on the screen you have to prepare a command with the following options:
* `fontfile` - refers a TTF file which should be the font name to display the required text message.
				In case the font file is already deployed in `share` location of **Splash** module
				(`/opt/clue/share/splash/fonts`) you can refer only the file name (not thw whole path).
				By default three font files are includes: `bold.ttf`, `regular.ttf` and `teletext.ttf`
* `fontsize` - indicates the height of the text letters
* `fontcolor` - sets the font color for the specified message. The application accept only HEX color
				format (with or without `hash` sign)
* `fontspacing` - sets the space between letters (by default the value is 3 horizontal pixels)

___Examples___

`splash --message="Sample message" -x100 -y200`

__or__

`splash --message="Sample message" -x100 -y200 -properties="fontcolor=#ff0000,fontsize=30,fontfile=teletext"`

__or__

`splash --message="Sample message" -x100 -y200 -properties="fontcolor=ff00ff,fontfile=/root/resources/arial.ttf"`

> **Note**: The coordinates options like `-x | --xpoint` or `-y | --ypoint` are described in ___Display Options___ chapter.


## Images
To display **images** on the screen you have to build a command with the options described below. The `--image`
switch can refer a file name already deployed on the application resources location (`/opt/clue/share/splash/images`)
or a specific file location. In case the file extension is not specified the PNG format is considered by default.
* `rotate` - rotate specified image with 90, 180 or 270 degrees - corresponding to input values: 1, 2, 3
* `enlarge` - displays the image to the whole area of the screen. It is a boolean value, accepts values, likes,
`y`, `yes`, `true`
* `stretch` - displays the image to the whole area of the screen by in stretching mode. It is also a boolean value,
the possible value are described above.

> **Note**: Only `bmp`, `jpg` or `png` file formats are supported!

___Examples___

`splash --image=splash`

__or__

`splash --image=/opt/clue/share/splash/images/splash.jpg" -x100 -y100`

__or__

`splash -i /opt/clue/share/splash/images/splash.jpg" --xpoint=100 -ypoint= --properties="rotate=1"`


## Shapes
To display **graphical shapes** on the screen you have to prepare a command to specify what shape is needed and
additional options (dedicated per type of shape) for specific formatting: The following shape types are implemented
* `line` - draw a simple line from point (x,y) to point (x2,y2)
* `hline` - draw a horizontal line (having the same `X` for both points - not needed to be specified for the second point)
* `vline` - draw a vertical line (having the same `Y` for both points - not needed to be specified for the second point)
* `circle` - draw a circle giving the center coordinates - single (x,y) point, and the radius
* `rectangle` - draw a rectangle providing a initial position and `width` and `height` dimensions.
* `progressbar` - draw a progress bar control as a derived shape from rectangle.

All graphical formating options of `line`, `hline` and `vline` shapes are:
* `linecolor` - specifies the color of the line in HEX format
* `x2point` - horizontal target coordinate
* `y2point` - vertical target coordinate

___Examples___

`splash --shape=line -x100 -y100 --properies="x2point=200,y2point=200,linecolor=#00FF00"`

__or__

`splash --shape=hline -x100 -y100 --properies="x2point=200,linecolor=#00FF00"`

The graphical formating options for `rectangle` shape are:
* `linecolor` - specifies the color of the line in HEX format
* `fillcolor` - specifies the color of the rectangle area in HEX format
* `width` - describes the width of the rectangle
* `height` - describes the height of the rectangle
* `linewidth` - specifies the thickness of the rectangle border

___Examples___

`splash --shape=rectangle -x100 -y100 --properies="width=200,height=100,linecolor=#00FF00"`

__or__

`splash --shape=rectangle -x500 -y500 --properies="width=200,height=200,linecolor=#000FF0,linewidth=5,fillcolor=#FF0000"`

The graphical formating options for `circle` shape are:
* `linecolor` - specifies the color of the line in HEX format
* `fillcolor` - specifies the color of the circle area in HEX format
* `radius` - describes the radius of the circle
* `linewidth` - specifies the thickness of the circle border

___Examples___

`splash --shape=circle -x500 -y500 --properies="radius=200,linecolor=#00FF00"`

__or__

`splash --shape=circle -x500 -y500 --properies="radius=200,linecolor=#00FF00,linewidth=5"`

The graphical formating options for `progressbar` shape are:
* `percent` - shows how much from the total area is ocupied by `barcolor` color. 
* `linecolor` - specifies the color of the line in HEX format
* `barcolor` - specifies the color of the rectangle area in HEX format which is already filled in, and described the `percent` 
part of internal rectangle area described by the progressbar control. if this color is not specified than it will be considered 
white (`#FFFFFF` color to fill it in when the `percent` value is bigger than 0)
* `backgroundcolor` - describe the color that suppose to be in the inner part of the progressbar control when the fill in 
percentage is 0 or less than 0 (when is less the color area is figure it out in the right side of the control). If not color 
is specified than it will be considered by default black color (`#000000`)
* `width` - describes the width of the progressbar control
* `height` - describes the height of the progressbar control
* `linewidth` - specifies the thickness of the progressbar border

___Examples___

`splash --shape=progressbar -x500 -y500 --properies="width=400,height=50,linecolor=#0000FF,linewidth=4,barcolor=#F0F0F0"`

__or__

`splash --shape=progressbar --ypoint=-34 --properties="width=500,height=50,linewidth=4,percent=-30"`

> **Note**: Negative value of `percept` option indicate that the progressbar control will not be redrawn, only `barcolor`
area will be adjusted according to the absolute value of the `percent` option. The negative value of `xpoint` option is
fully described in ___Relative coordinates___ chapter.


# Display options
Display options make the application 'to appear' dynamic and to build simple execution workflows to create graphical screens
from shell commands. Below are described the execution options that, together with specific object options make the execution
process flowless. In addition, the application is designed to keep in memory each executed command and if two consecutive
commands are executed specifying the same object type the previous one will be wiped from the screen (it is considered that
the position and the graphical format properties have been changed and the object will be moved or overdrawn with other
coordinates). To prevent such behaviour various display options from the list below might be combined to obtain a different
experience.

* `reset` - resets the entire screen area (delete all drawn objects) and make the screen black.
* `console` - creates a graphical terminal over frame buffer (a canvas area able to render graphical objects). In case this
option is not specified the command will use the existent graphical console.
* `keep` - use this option to keep the object on the screen even if the command is repeated for the same object type with or
without the same graphical coordinates and properties. This options allow the internal workflow to be execute but will avoid
wiping of previous object type. The command will still go in memory to know previous execution step but will not be used for
wiping.
* `wipe` - wipes whatever object type was drawn before (not necessary to be drawn previously). The color properties are not
important for this type of command but it is really important to specify the same content (message text or image or shape type)
* `head` - it has the same behaviour like `--keep`, the difference is that the command keeps the object on the screen but is
not asking the workflow because is skip it. So, when you are using this option the command will not go in memory, will just
simple display the object on the screen.

___Examples___
The commands below are use-cases from **Setup** utility.

`/splash --message="Clue OS" --xpoint=-2021 -y5 --head`
> Displays "Clue OS" text message on the top-right corner of the
screen (relative coordinates). This message will not be removed by the next command and will not be stored in memory.

`splash --message="System Setup" -x5 -y5 --keep`
> Displays "System Setup" message in the top-left corner of the screen and
keeps it on the screen and in the system memory even if the next command is for the text message display.

`splash --message="System Setup" -x5 -y5 --wipe`
> Deletes "System Setup" text message located at point (5,5) - if it was
written there before using or not using display options (meaning using `--head` or `--keep` options).

`/opt/clue/bin/splash --image=splash --console --reset`
> Displays splash.png image (located in resources repository of the application) creating previously a new console and reset it
by whatever data filled in.


# Relative coordinates
Just to have flexibility render the graphical objects (messages, shapes and images) on any graphical console (display - monitors,
TVs, custom displays) it was implemented a mechanism to render the object using relative coordinates. The way to define such
relative coordinates considering the total height and width of the screen; the relative coordinate is actually a fraction between
two numbers; just to differentiate it by regular coordinates the relative coordinates have negative values.
The fraction contains at least two digits and a value like `-12` means 1/2 from screen height or width. So mean `-12` screen center.
A number like `-34` means 3/4 from the height or width; `-2021` means 20/21 from screen width or height, so very close to the right/down
limit of the screen. A number like `-110` means 1/10 from the height or width of the screen, so very close to the left/top limit
of the screen.
The relative coordinate are applicable only to the origin coordinates options `-x` or `--xpoint`, and `-y` or `--ypoint`, and in case
no values are are specified to these options, relative values will be considered - always referring mid of the screen (`-12`)