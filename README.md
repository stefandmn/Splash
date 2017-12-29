# Clue Splash

**Splash** is a graphical term for representation of boot process, developed especially for Clue OS but
might be applicable to any Linux system. Splash can display graphics (JPG image format) or some
combinations of graphics with text messages (in graphical format). Splash program uses a stateful
memory buffer and is able to initiate and maintain the workflow even is the process is not running
in loop. The workflow is initiated in InitRAM area, creating a dedicated graphical console and then,
other workflow steps could be launched (by individual process execution) that will use existing
graphical console and updates display content: display new splashes or text messages.

Splash program is designed to support the following feature:

1. `--image`   = prints a splash image on the graphical term.
2. `--message` = displays a text message on the graphical screen.
3. `--shape`   = draws graphical shapes and basic controls for data display, loading, etc.
4. `--exit`    = exit from the splash application. It accepts also a delay as an optional parameter

For the syntax details of each option please see **Running** chapter.

> **Note**: One one option could be specified for a workflow step (program execution)

# Prerequisites
In order to build `clue-splash` package from source code you have to install required packages mentioned below. **Splash** is a 
static-compiled application, so it is not need to install additional libraries or package to run it or to bundle it in your 
RamFS image.

`apt-get install -y gcc uthash-dev libjpeg62-dev libfreetype6-dev libpng12-dev`

# Running
As a general structure, each drawing command has the following componwents:
1. initial coordinates - an (x,y) point where the object is located on the screen
2. type of the object - describes what type of object will be drawn on the screen; the options have been mentioned above: 
`image`, `shape` and `message`.
3. graphical formating options - graphical properties declared through `--properties` option to define the format of the object
that is going to be drawn or displayed
4. display options - dedicated options  that allows you to build __an workflow__ using consecutive execution command of Splash 
application. With this option you can memorize object to use them from one session to another without to repeat the command, to 
overdrawn graphical objects from the previous location, etc. - all these options are explained in ___Display Options___ chapter.

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

`/opt/clue/bin/splash --message="Sample message" -x100 -y200`

__or__

`/opt/clue/bin/splash --message="Sample message" -x100 -y200 -properties="fontcolor=#ff0000,fontsize=30,fontfile=teletext"`

__or__

`/opt/clue/bin/splash --message="Sample message" -x100 -y200 -properties="fontcolor=ff00ff,fontfile=/root/resources/arial.ttf"`

> **Note**: The coordinates options like `-x | --xpoint` or `-y | --ypoint` are described in ___Display Options___ chapter.


## Images
To display **images** on the screen you have to build a command with the options described below. The `--image` switch
can refer a file name already deployed on the application resources location (`/opt/clue/share/splash/images`) or a specific
file location. In case the file extension is not specified the PNG format is considered by default.
* `rotate` - rotate specified image with 90, 180 or 270 degrees - corresponding to input values: 1, 2, 3
* `enlarge` - displays the image to the whole area of the screen. It is a boolean value, accepts values, likes, `y`, `yes`, `true`
* `stretch` - displays the image to the whole area of the screen by in stretching mode. It is also a boolean value, the possible value are described above.

> **Note**: Only `bmp`, `jpg` or `png` file formats are supported!

___Examples___

`/opt/clue/bin/splash --image=splash`

__or__

`/opt/clue/bin/splash --image=/opt/clue/share/splash/images/splash.jpg" -x100 -y100`

__or__

`/opt/clue/bin/splash -i /opt/clue/share/splash/images/splash.jpg" --xpoint=100 -ypoint= --properties="rotate=1"`


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

`/opt/clue/bin/splash --shape=line -x100 -y100 --properies="x2point=200,y2point=200,linecolor=#00FF00"`

__or__

`/opt/clue/bin/splash --shape=hline -x100 -y100 --properies="x2point=200,linecolor=#00FF00"`

The graphical formating options for `rectangle` shape are:
* `linecolor` - specifies the color of the line in HEX format
* `fillcolor` - specifies the color of the rectangle area in HEX format
* `width` - describes the width of the rectangle
* `height` - describes the height of the rectangle
* `linewidth` - specifies the thickness of the rectangle border

___Examples___

`/opt/clue/bin/splash --shape=rectangle -x100 -y100 --properies="width=200,height=100,linecolor=#00FF00"`

__or__

`/opt/clue/bin/splash --shape=rectangle -x500 -y500 --properies="width=200,height=200,linecolor=#000FF0,linewidth=5,fillcolor=#FF0000"`

The graphical formating options for `circle` shape are:
* `linecolor` - specifies the color of the line in HEX format
* `fillcolor` - specifies the color of the circle area in HEX format
* `radius` - describes the radius of the circle
* `linewidth` - specifies the thickness of the circle border

___Examples___

`/opt/clue/bin/splash --shape=circle -x500 -y500 --properies="radius=200,linecolor=#00FF00"`

__or__

`/opt/clue/bin/splash --shape=circle -x500 -y500 --properies="radius=200,linecolor=#00FF00,linewidth=5"`

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

`/opt/clue/bin/splash --shape=progressbar -x500 -y500 --properies="width=400,height=50,linecolor=#0000FF,linewidth=4,barcolor=#F0F0F0"`

__or__

# Display options


# Relative coordinates
