# Clue Splash

Splash is a graphical term for representation of boot process, developed especially for Clue OS but
might be applicable to any Linux system. Splash can display graphics (JPG image format) or some
combinations of graphics with text messages (in graphical format). Splash program uses a stateful
memory buffer and is able to initiate and maintain the workflow even is the process is not running
in loop. The workflow is initiated in InitRAM area, creating a dedicated graphical console and then,
other workflow steps could be launched (by individual process execution) that will use existing
graphical console and updates display content: display new splashes or text messages.

Splash program is designed to support the following feature:

1. `--picture` = sets a splash image on the graphical screen. Though this command option you can specify 
		also the coordinates where the picture will be displayed.
2. `--message` = displays a message text on the graphical screen. Additionally, you can specify the 
		coordinates where the text message will be printed and also the font details (name and size).
3. `--console` = attach the new workflow to the current graphical console (if is defined, otherwise will 
		initiate the frame buffer without graphical format.
4. `--exit` = exit from the splash application. It accepts also a delay as an optional parameter

For the syntax details of each option please see **Running** chapter.

> **Note**: One one option could be specified for a workflow step (program execution)

# Prerequisites
In order to build `clue-splash` package you have to install required packages:

  apt-get install -y gcc uthash-dev libjpeg62-dev libfreetype6-dev libpng12-dev

# Running

To display a splash image you have run the following command line:

`/opt/clue/bin/splash --picture=AAAxBBB@image`

The possible option parts are:
- AAAxBBB = are the (x,y) coordinates (starting from left,top corner of the screen) where the image will be displayed.
			The numbers (horizontal and vertical dimensions) are separated by `x` sign.
- image   = it is the JPG image that will be displayed on the screen that is stored in `/opt/clue/share/splash/images`
			folder. You can refer any image from this folder without specifying the file extension or the full path.
			On the other side you can specify you can specify a full path image if your resources are not located on
			the base folder. If you want to specify the full path of the image you need to write it under double
			quotes (`"`).
- `@`        = it is the separator between image file name and coordinates.
- `*`        = this sign needs be specified when you want to ask the program to reset initial screen (to remove existing
			splash image or other text messages). The sign could be specify in front of coordinates or of image name or path.

## Examples for Splash

`/opt/clue/bin/splash --picture=*640x360@splash`

__or__

`/opt/clue/bin/splash --picture=640x360@*splash`

__or__

`/opt/clue/bin/splash --picture=*640x360@"/opt/clue/share/splash/images/splash.jpg"`


To display a text message you have run the following command line:

`/opt/clue/bin/splash --picture=AAAxBBB:fontname:size@message`

- AAAxBBB = are the (x,y) coordinates (starting from left,top corner of the screen) where the text message will be
			displayed. The numbers (horizontal and vertical dimensions) are separated by `x` sign.
- message  = it is the text message that will be displayed on the screen. If the text messages are requested to be
			displayed the previous message is deleted. If you want to keep it on the screen you have to put `*` in
			front of the message or in front of the coordinates.
- `@`        = it is the separator between message text and the other details.
- `:`        = separates the fond details by the others: you can specify only the font name or only the font size or
			both. The font name could be nominated as simple label identifying one of the font file stored in
			`/opt/clue/share/splash/fonts` folder or you can specify a full path choosing one of the font files
			from the local file system.
- `*`        = this sign needs be specified when you want to keep persistent the specified text message.

## Examples for Text Messages

`/opt/clue/bin/splash --message=640x360:regular:25@"Loading resources.."`

__or__

`/opt/clue/bin/splash --message=640x360:25@"Loading resources.."`

__or__

`/opt/clue/bin/splash --message=640x360:regular@"Starting services.."`

__or__

`/opt/clue/bin/splash --message=*1700x10:bold:25@"Clue OS"`


> **Note**: The workflow steps and execution details could be seen in the process related log file created during
process execution. The log file name is located in `/var/log/splash.log`.
