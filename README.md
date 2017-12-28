# Clue Splash

Splash is a graphical term for representation of boot process, developed especially for Clue OS but
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
In order to build `clue-splash` package you have to install required packages:

  apt-get install -y gcc uthash-dev libjpeg62-dev libfreetype6-dev libpng12-dev

# Running

1. To display **text messages** on the screen you have to prepare a command with the following options:
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

`/opt/clue/bin/splash --message="Sample message" -x100 -y200` -properties="fontcolor=#ff0000,fontsize=30,fontfile=teletext"`

__or__

`/opt/clue/bin/splash --message="Sample message" -x100 -y200` -properties="fontcolor=ff00ff,fontfile=/root/resources/arial.ttf"`


2. To display **pictures** on the screen you have to build a command with the options described below. The `--image` switch
can refer a file name already deployed on the application resources location (`/opt/clue/share/splash/images`) or a specific
file location. In case the file extension is not specified the PNG format is considered by default.
* `rotate` - rotate specified image with 90, 180 or 270 degrees - corresponding to input values: 1, 2, 3
* `enlarge` - displays the image to the whole area of the screen. It is a boolean value, accepts values, likes, `y`, `yes`, `true`
* `stretch` - displays the image to the whole area of the screen by in stretching mode. It is also a boolean value, he possible value
			are described above.

___Examples___

`/opt/clue/bin/splash --image=splash`

__or__

`/opt/clue/bin/splash --image=/opt/clue/share/splash/images/splash.jpg"`

__or__

`/opt/clue/bin/splash -i /opt/clue/share/splash/images/splash.jpg" --properties="rotate=1"`


3. To display a graphical shape on the screen you have to run a command with the following options:
	TBD

# Relative coordinates

# DIsplay options