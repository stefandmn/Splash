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

1. To display a text message on the screen you have to prepare a command with the following options:
	TBD

___Examples___

`/opt/clue/bin/splash --message="Sample message" -x100 -y200`

__or__

`/opt/clue/bin/splash --message="Sample message" -x100 -y200`


2. To display a picture on the screen you have to build a command with the following options:
	TBD

___Examples___

`/opt/clue/bin/splash --image=splash`

__or__

`/opt/clue/bin/splash --image=/opt/clue/share/splash/images/splash.jpg"`

__or__

`/opt/clue/bin/splash -i /opt/clue/share/splash/images/splash.jpg"`


3. To display a graphical shape on the screen you have to run a command with the following options:
	TBD
