
/**
 * Clue Media Experience
 * Splash.c - Splash configuration and workflow implementation
 */

#include <stdio.h>
#include <locale.h>
#include <getopt.h>
#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#include "utils.h"
#include "buffer.h"
#include "images.h"
#include "shapes.h"
#include "text.h"


void SaveDataInMemory(CmdData data)
{
	int ShmID;
	key_t ShmKEY;
	CmdData *ShmDAP;

	ShmKEY = ftok(".", 'x');
	ShmID = shmget(ShmKEY, sizeof(CmdData), IPC_CREAT | 0666);

	if (ShmID < 0)
	{
		 ERROR("    Shared memory can not be reserved for writing");
		 return;
	}

	ShmDAP = (CmdData*) shmat(ShmID, NULL, 0);

	if ((int) ShmDAP == -1)
	{
		 ERROR("    Shared memory cannot be attached for writing");
		 return;
	}
	else DEBUG("    Attached shared memory for writing");

	//set data
	ShmDAP->id = data.id;
	strcpy(ShmDAP->value, data.value);
	strcpy(ShmDAP->props, data.props);
	ShmDAP->xpoint = data.xpoint;
	ShmDAP->ypoint = data.ypoint;
	DEBUG("    Writing command in memory: [%d] = %s", data.id, data.value != NULL ? data.value : "");
	DEBUG("                              @point = (%d,%d)", data.xpoint, data.ypoint);
	DEBUG("                              >props = %s", data.props);

	shmdt((void *) ShmDAP);
	INFO("    Detached shared memory for writing");
}

CmdData GetMemoryData(void)
{
	int ShmID;
	key_t ShmKEY;
	CmdData data;
	CmdData *ShmDAP;

	ShmKEY = ftok(".", 'x');
	ShmID = shmget(ShmKEY, sizeof(CmdData), 0666);

	if (ShmID < 0)
	{
		 ERROR("    Shared memory can not be reserved for reading");
		 return data;
	}

	ShmDAP = (CmdData*) shmat(ShmID, NULL, 0);

	if ((int) ShmDAP == -1)
	{
		 ERROR("    Shared memory cannot be attached for reading");
		 return data;
	}
	else DEBUG("    Attached shared memory for reading");

	//set data
	data.id = ShmDAP->id;
	strcpy(data.value, ShmDAP->value);
	strcpy(data.props, ShmDAP->props);
	data.xpoint = ShmDAP->xpoint;
	data.ypoint = ShmDAP->ypoint;

	DEBUG("    Reading command from memory: [%d] = %s", data.id, data.value != NULL ? data.value : "");
	DEBUG("                                @point = (%d,%d)", data.xpoint, data.ypoint);
	DEBUG("                                >props = %s", data.props);

	shmdt((void *) ShmDAP);
	INFO("    Detached shared memory for reading");

	return data;
}

void CleanDataInMemory(void)
{
	int ShmID;
	key_t ShmKEY;

	ShmKEY = ftok(".", 'x');
	ShmID = shmget(ShmKEY, sizeof(CmdData), IPC_RMID | 0666);

	if (ShmID >= 0)
	{
		shmctl(ShmID, IPC_RMID, NULL);
		DEBUG("    Released shared memory reference for writing");
	}
}

bool HasDataInMemory(void)
{
	int ShmID;
	key_t ShmKEY;

	ShmKEY = ftok(".", 'x');
	ShmID = shmget(ShmKEY, sizeof(CmdData), 0666);

	if (ShmID < 0) return false;
		else return true;
}

/**
 * Transform input parameters into a specific structure for data processing
 *
 * @param argc list of input arguments
 * @param argv number of arguments in the list
 *
 * @return CmdData structure
 */
CmdData GetInputData(int argc, char argv[])
{
	CmdData data;
	char value[1000];
	int opt;

	//Command line arguments
	static struct option options[] =
	{
		{"reset", no_argument, NULL, 'r'},
		{"sticky", no_argument, NULL, 's'},
		{"exit", optional_argument, NULL, 'e'},
		{"quit", optional_argument, NULL, 'q'},
		{"draw", optional_argument, NULL, 'd'},
		{"image", optional_argument, NULL, 'i'},
		{"message", optional_argument, NULL, 'm'},
		{"props", optional_argument, NULL, 'p'},
		{"xpoint", optional_argument, NULL, 'x'},
		{"ypoint", optional_argument, NULL, 'y'},
		{NULL, 0, NULL, 0}
	};

	//initializing command properties
	data.id = 0;
	data.xpoint = 0;
	data.ypoint = 0;
	strcpy(data.value, "");
	strcpy(data.props, "");

	while((opt = getopt_long(argc, argv, "rse::q::x::y::i:m:p:d:", options, NULL)) != -1)
	{
		if (optarg != NULL)
		{
			if(sizeof (optarg) < 255) strcpy(value, optarg);
				else strncpy(value, optarg, 255);

			strcpy(value, strtrim(value));
		}
		else strcpy(value, "");
		
		switch (opt)
		{
			case 'p':
				if(strlen(data.props) > 0) strcat(data.props, ", ");
				strcat(data.props, value);
				break;

			case 'r':
				if(strlen(data.props) > 0) strcat(data.props, ", ");
				strcat(data.props, "reset=true");
				break;

			case 's':
				if(strlen(data.props) > 0) strcat(data.props, ", ");
				else strcat(data.props, "sticky=true");
				break;

			case 'x':
				if(strcmp(value, "") > 0) data.xpoint = atoi(value);
					else data.xpoint = 0;
				break;

			case 'y':
				if(strcmp(value, "") > 0) data.ypoint = atoi(value);
					else data.ypoint = 0;
				break;

			case 'e':
			case 'q':
				if (data.id == 0) 
				{
					data.id = opt;
					if(strlen(data.props) > 0) strcat(data.props, ", ");
					if(strcmp(value, "") != 0)
					{
						strcat(data.props, "exitdelay=");
						strcat(data.props, value);
					}
					else strcat(data.props, "exitdelay=0");
				}
				else 
				{
					ERROR("Multiple commands specified, previous was [%s] ", data.id);
					exit(1);
				}
				break;

			case 'm':
			case 'i':
			case 'd':
				if (data.id == 0) 
				{
					data.id = opt;
					strcpy(data.value, value);
				}
				else 
				{
					ERROR("Multiple commands specified, previous was [%s] ", data.id);
					exit(1);
				}
				break;
		}
	}
	
	DEBUG("    Reading command from input: [%d] = %s", data.id, data.value != NULL ? data.value : "");
	DEBUG("                               @point = (%d,%d)", data.xpoint, data.ypoint);
	DEBUG("                               >props = %s", data.props);
	
	return data;
}

/**
 * Synchronize input command data with graphical environment
 * 
 * @param data inpuc command data structure
 * @return improved and synched data structure
 */
CmdData GetSyncData(CmdData data)
{
	//update properties based on the command that will be executed
	if (data.id == 'm')
	{
		data = SetTextProps(data);
	}
	else if(data.id == 'i') 
	{
		data = SetImageProps(data);
	}
	else if(data.id == 'd') 
	{
		data = SetShapeProps(data);
	}
	
	DEBUG("    Synchronizing input command: [%d] = %s", data.id, data.value != NULL ? data.value : "");
	DEBUG("                                @point = (%d,%d)", data.xpoint, data.ypoint);
	DEBUG("                                >props = %s", data.props);
	
	return data;
}

/**
 *  System function to run the program
 *
 * @param argc number of input arguments
 * @param argv pointer to the list of arguments
 * @return  0 if the execution is correct, otherwise a non 0 value.
 */
int main(int argc, char **argv)
{
	CmdData data, prevdata;

	//set file channel
	setlocale(LC_ALL, "");
	sprintf(LOGFILE, "%s%s%s", "/var/log/", basename(argv[0]), ".log");

	//Extract absolute path of executable in order to identify related resources: picture and font files
	readlink("/proc/self/exe", RESPATH, sizeof (RESPATH));
	sprintf(RESPATH, "%s%s", dirname(dirname(RESPATH)), "/share/splash");

	//read input arguments
	DEBUG("Reading input data");
	data = GetInputData(argc, argv);

	// Main Workflow
	if(!HasDataInMemory())
	{
		INFO("*** New process CREATEs console");

		//open graphical console
		OpenGraphicsOnConsole(&console);

		//initialize the screen
		OpenBuffer();

		//set black screen
		ResetScreen();
	}
	else
	{
		INFO("*** New process OPENs console");

		//initialize the screen
		OpenBuffer();
		
		//reset the screen
		if(getBoolDataProperty(data, "reset"))
		{
			LOGGER("    Reset the screen");
			ResetScreen();
			
			//delete reserved memory buffer
			DEBUG("Deleting memory buffer");
			CleanDataInMemory();
		}
		else
		{
			//read old data from memory and parse it
			DEBUG("Reading memory buffer");
			prevdata = GetMemoryData();

			//evaluate persistency
			if(!getBoolDataProperty(prevdata, "sticky"))
			{	
				//remove old message
				if(data.id == 'm' && prevdata.id == 'm')
				{
					DEBUG("Removing old text message from screen");
					OverdrawText(prevdata);
				}
				else if(data.id == 'd' && prevdata.id == 'd')
				{
					DEBUG("Removing old shape from screen");
					OverdrawShape(prevdata);
				}
				else if(data.id == 'i' && prevdata.id == 'i')
				{
					DEBUG("Removing old image from screen");
					OverdrawImage(prevdata);
				}
			}
		}
	}
	
	// synchronize command with graphical environment
	data = GetSyncData(data);

	// run actual command
	if(data.id == 'i')
	{
		//display new image
		INFO("Request for Picture..");
		DrawImage(data);
	}
	else if(data.id == 'm')
	{
		//display new text message
		INFO("Request for Message..");
		DrawText(data);
	}
	else if(data.id == 'd')
	{
		//display new shape
		INFO("Request for Drawing..");
		DrawShape(data);
	}	

	//process EXIT-REQUEST event
	if(data.id == 'e' || data.id == 'q')
	{
		INFO("Request for Exit..");

		if(getIntDataProperty(data, "exitdelay") > 0)
		{
			int delay = getIntDataProperty(data, "exitdelay");
			LOGGER("    Sleep %d seconds for exit", delay);
			sleep(delay);
		}

		//delete reserved memory buffer
		DEBUG("Deleting memory buffer");
		CleanDataInMemory();

		//clear screen
		DEBUG("Clearing frame buffer and closing graphical console");
		OpenGraphicsOnConsole(&console);
		CloseConsole(&console);

		CloseBuffer();
		INFO("Workflow EXIT.");
	}
	else
	{
		// write the current transaction in memory
		DEBUG("Writing memory buffer");
		SaveDataInMemory(data);

		INFO("Process DONE.");
	}

	exit(0);
}
