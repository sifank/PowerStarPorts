/***************************************************************************
File:           PSports.cpp
Version:        202110208
Author:         Sifan
Desc:           PowerStar Ports INDI driver
Requires:       PScontrol.cpp hidapi.c
***************************************************************************/
//#include <stdio.h>
//#include <memory>
//#include <string.h>

#include "PSports.h"

// We declare an auto pointer to kobsRelay
std::unique_ptr<kobsRelay> KOBSrelay(new kobsRelay());
    
void ISPoll(void *p);

/***************************************************************/
void ISInit()
{
	static int isInit = 0;

	if (isInit == 1)
		return;
	if(KOBSrelay.get() == 0)
	{
		isInit = 1;
		KOBSrelay.reset(new kobsRelay());
	}
}

/***************************************************************/
void ISGetProperties(const char *dev)
{
        ISInit();
        KOBSrelay->ISGetProperties(dev);
}

/***************************************************************/
void ISNewSwitch(const char *dev, const char *name, ISState *states, char *names[], int num)
{
        ISInit();
        KOBSrelay->ISNewSwitch(dev, name, states, names, num);
}

/***************************************************************/
void ISNewText(	const char *dev, const char *name, char *texts[], char *names[], int num)
{
        ISInit();
        KOBSrelay->ISNewText(dev, name, texts, names, num);
}

/***************************************************************/
void ISNewNumber(const char *dev, const char *name, double values[], char *names[], int num)
{
        ISInit();
        KOBSrelay->ISNewNumber(dev, name, values, names, num);
}

/***************************************************************/
void ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int num)
{
	INDI_UNUSED(dev);
	INDI_UNUSED(name);
	INDI_UNUSED(sizes);
	INDI_UNUSED(blobsizes);
	INDI_UNUSED(blobs);
	INDI_UNUSED(formats);
	INDI_UNUSED(names);
	INDI_UNUSED(num);
}

/***************************************************************/
void ISSnoopDevice (XMLEle *root)
{
	ISInit();
	KOBSrelay->ISSnoopDevice(root);
}




/***************************************************************/
kobsRelay::kobsRelay()
{
	setVersion(VERSION_MAJOR,VERSION_MINOR);
}

/***************************************************************/
kobsRelay::~kobsRelay()
{
	// Delete BCM Pins options
	//deleteProperty(BCMpinsNP.name);
}

/***************************************************************/
bool kobsRelay::checkFaults()
{
    return true;
}

/***************************************************************/
int kobsRelay::relayCTL(string &relay, const std::string &action)
{    
    //TODO call faults function
    //LOGF_INFO("DIAG: Requesting power for %s to %s", relay, action);
    string acmd = action;
    if (! psctl.setPowerState(relay, acmd))
        return IPS_ALERT;

    return IPS_OK;    
}
    
/***************************************************************/
bool kobsRelay::Connect()
{	
    PSCTL psctl;
    
    if ( ! psctl.Connect() )  //this does the unlock as well
    {
        LOG_ERROR("No PowerStar focuser found.");
        return false;
    }
    else
    {
        uint16_t psversion = psctl.getVersion();
        LOGF_INFO("PowerStar Firmware Version: %i.%i", (psversion & 0xFF00) >> 8, psversion & 0xFF);
        
        // read config file and retrieve port names
        FILE* fin = fopen("/etc/powerstar.config", "r");
        int rc = fread(&curProfile, sizeof(PowerStarProfile), 1, fin);     
        if (!rc) {
            LOG_ERROR("Error: could not read profile\n");
            return false;
        }
        fclose(fin);
        
        LOGF_INFO("Humidity @ opening: %#.1f", psctl.getHumidity());
        LOGF_INFO("Temperature @ opening: %#.1f", psctl.getTemperature());
    }
    
    // Lock BCM Pins setting
	//BCMpinsNP.s=IPS_BUSY;
	//IDSetNumber(&BCMpinsNP, nullptr);

	LOG_INFO("PoweStar Ports connected successfully.");
    
	return true;
}

/***************************************************************/
bool kobsRelay::Disconnect()
{	// Close GPIO
	BCMpinsNP.s=IPS_IDLE;
	IDSetNumber(&BCMpinsNP, nullptr);

	DEBUG(INDI::Logger::DBG_SESSION, "KOBS Relays disconnected successfully.");
	return true;
}

/***************************************************************/
const char * kobsRelay::getDefaultName()
{
        return (char *)"Power*Star Ports";
}

/***************************************************************/
bool kobsRelay::initProperties()
{
	// We init parent properties first
	INDI::DefaultDevice::initProperties();
    
    /******Options Tab********/ 
    IUFillText(&ScriptsT[0], "RELAY_SCRIPT", "KOBS Relay Script", "/usr/share/indi/scripts/INDIrelay.py");
    IUFillTextVector(&ScriptsTP, ScriptsT, 1, getDeviceName(), "RELAY_SCRIPT", "Script", OPTIONS_TAB, IP_RW, 200, IPS_IDLE);
    
    IUFillText(&RnameT[0], "RELAY_1", "Out 1 Name", "Relay 1");
    IUFillText(&RnameT[1], "RELAY_2", "Out 2 Name", "Relay 2");
    IUFillText(&RnameT[2], "RELAY_3", "Out 3 Name", "Relay 3");
    IUFillText(&RnameT[3], "RELAY_4", "Out 4 Name", "Relay 4");
    IUFillTextVector(&RnameTP, RnameT, 4, getDeviceName(), "RELAY_NAMES", "Names", OPTIONS_TAB, IP_RW, 200, IPS_IDLE);
    
    DEBUGF(INDI::Logger::DBG_SESSION, "Port 1 name: %s", RnameT[0].text);
    
    //IUFillNumber(&BCMpinsN[0], "BCMPIN01", RnameT[0].text, "%0.0f", 1, 27, 0, 5); // BCM5 = PIN29
	//IUFillNumber(&BCMpinsN[1], "BCMPIN02", "Relay 2", "%0.0f", 1, 27, 0, 6); // BCM6 = PIN31
	//IUFillNumber(&BCMpinsN[2], "BCMPIN03", "Relay 3", "%0.0f", 1, 27, 0, 12); // BCM13 = PIN33
	//IUFillNumber(&BCMpinsN[3], "BCMPIN04", "Relay 4", "%0.0f", 1, 27, 0, 13); // BCM19 = PIN35
	//IUFillNumberVector(&BCMpinsNP, BCMpinsN, 4, getDeviceName(), "BCMPINS", "BCM Pins", OPTIONS_TAB, IP_RW, 0, IPS_IDLE);

    /******Main Tab********/
    //memset(relayLabel, 0, MAXINDILABEL);
    //int portRC = IUGetConfigText(getDeviceName(), RnameTP.name, RnameT[0].text, relayLabel, MAXINDILABEL);
    //DEBUGF(INDI::Logger::DBG_ERROR, "TP %s T %s label %s", RnameTP.name, RnameT[0].text, relayLabel);
    //DEBUGF(INDI::Logger::DBG_SESSION, "Rname %s", RnameT[0].text);
    //relayLabel = RnameT[0].text;

    kobsRelay::checkFaults();
    psctl.getStatus();
    // Relay 1
    if ( psctl.statusMap["Out1"].state) {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_OFF);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_ON);
    }
    else   {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_ON);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_OFF);
    }
	IUFillSwitchVector(&Switch1SP, Switch1S, 2, getDeviceName(), "SWITCH_1", RnameT[0].text, MAIN_CONTROL_TAB, IP_RW, ISR_1OFMANY, 0, IPS_IDLE);

    // Relay 2
    if ( psctl.statusMap["Out2"].state) {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_OFF);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_ON);
    }
    else   {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_ON);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_OFF);
    }
	IUFillSwitchVector(&Switch2SP, Switch2S, 2, getDeviceName(), "SWITCH_2", RnameT[1].text, MAIN_CONTROL_TAB, IP_RW, ISR_1OFMANY, 0, IPS_IDLE);

    // Relay 3
	if ( psctl.statusMap["Out3"].state) {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_OFF);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_ON);
    }
    else   {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_ON);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_OFF);
    }
	IUFillSwitchVector(&Switch3SP, Switch3S, 2, getDeviceName(), "SWITCH_3", RnameT[2].text, MAIN_CONTROL_TAB, IP_RW, ISR_1OFMANY, 0, IPS_IDLE);

    // Relay 4
    if ( psctl.statusMap["Out4"].state) {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_OFF);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_ON);
    }
    else   {
        IUFillSwitch(&Switch1S[0], "SW1ON", "ON", ISS_ON);
        IUFillSwitch(&Switch1S[1], "SW1OFF", "OFF", ISS_OFF);
    }
	IUFillSwitchVector(&Switch4SP, Switch4S, 2, getDeviceName(), "SWITCH_4", RnameT[3].text, MAIN_CONTROL_TAB, IP_RW, ISR_1OFMANY, 0, IPS_IDLE);


	// Load BCM Pins
	defineNumber(&BCMpinsNP);	
	loadConfig();

	return true;
}

/***************************************************************/
bool kobsRelay::updateProperties()
{
	// Call parent update properties first
	INDI::DefaultDevice::updateProperties();

	if (isConnected())
	{
        // We're connected
		defineSwitch(&Switch1SP);
		defineSwitch(&Switch2SP);
		defineSwitch(&Switch3SP);
		defineSwitch(&Switch4SP);
	}
	else
	{
		// We're disconnected
		deleteProperty(Switch1SP.name);
		deleteProperty(Switch2SP.name);
		deleteProperty(Switch3SP.name);
		deleteProperty(Switch4SP.name);
	}
	return true;
}

/***************************************************************/
void kobsRelay::ISGetProperties(const char *dev)
{
	INDI::DefaultDevice::ISGetProperties(dev);
    static bool once = true;
    if (once)
    {
        once = false;
        defineText(&ScriptsTP);
        defineText(&RnameTP);
        loadConfig(false, "KOBS_RELAY_SCRIPTS");
        loadConfig(false, "KOBS_RELAY_NAMES");
    }
}

/***************************************************************/
bool kobsRelay::ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n)
{
	// first we check if it's for our device
	if(strcmp(dev,getDeviceName())==0)
	{
	        // handle BCMpins
	        if (!strcmp(name, BCMpinsNP.name))
	        {
			unsigned int valcount = 4;

			if (isConnected())
			{
				DEBUG(INDI::Logger::DBG_WARNING, "Cannot set BCM Pins while device is connected.");
				return false;
			} else {
				for (unsigned int i = 0; i < valcount; i++)
				{
					// verify a number is a valid BCM Pin
					if ( values[i] < 1 || values[i] > 27 )
					{
						BCMpinsNP.s=IPS_ALERT;
						IDSetNumber(&BCMpinsNP, nullptr);
						DEBUGF(INDI::Logger::DBG_ERROR, "Value %0.0f is not a valid BCM Pin number!", values[i]);
						return false;
					}

					// Verify unique BCM Pin assignement
					for (unsigned j = i + 1; j < valcount; j++)
					{
						if ( values[i] == values[j] )
						{
							BCMpinsNP.s=IPS_ALERT;
							IDSetNumber(&BCMpinsNP, nullptr);
							DEBUG(INDI::Logger::DBG_ERROR, "You cannot assign the same BCM Pin twice!");
							return false;
						}
					}
				}

				IUUpdateNumber(&BCMpinsNP,values,names,n);

				BCMpinsNP.s=IPS_OK;
				IDSetNumber(&BCMpinsNP, nullptr);
				DEBUGF(INDI::Logger::DBG_SESSION, "KOBS Relays BCM Pins set to Relay1: BCM%0.0f, Relay2: BCM%0.0f, Relay3: BCM%0.0f, Relay4: BCM%0.0f", BCMpinsN[0].value, BCMpinsN[1].value, BCMpinsN[2].value, BCMpinsN[3].value);
				return true;
			}
        	}
	}

	return INDI::DefaultDevice::ISNewNumber(dev,name,values,names,n);
}

/***************************************************************/
bool kobsRelay::ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n)
{
	// first we check if it's for our device
	if (!strcmp(dev, getDeviceName()))
	{
		// handle relay 1
		if (!strcmp(name, Switch1SP.name))
		{
            device = "out1";
            if ( Switch1S[0].s == ISS_ON )
                kobsRelay::relayCTL(device, "yes");
            else
                kobsRelay::relayCTL(device, "no");
                
			IUUpdateSwitch(&Switch1SP, states, names, n);
		}

		// handle relay 2
		if (!strcmp(name, Switch2SP.name))
        {
            device = "out2";
            if ( Switch1S[0].s == ISS_ON )
                kobsRelay::relayCTL(device, "yes");
            else
                kobsRelay::relayCTL(device, "no");
             
			IUUpdateSwitch(&Switch2SP, states, names, n);
        }

		// handle relay 3
		if (!strcmp(name, Switch3SP.name))
		{
            device = "out3";
            if ( Switch1S[0].s == ISS_ON )
                kobsRelay::relayCTL(device, "yes");
            else
                kobsRelay::relayCTL(device, "no");
             
			IUUpdateSwitch(&Switch3SP, states, names, n);
		}

		// handle relay 4
		if (!strcmp(name, Switch4SP.name))
		{
            device = "out4";
            if ( Switch1S[0].s == ISS_ON )
                kobsRelay::relayCTL(device, "yes");
            else
                kobsRelay::relayCTL(device, "no");
             
			IUUpdateSwitch(&Switch4SP, states, names, n);
		}
	}
	return INDI::DefaultDevice::ISNewSwitch (dev, name, states, names, n);
}

/***************************************************************/
bool kobsRelay::ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n)
{
    if (dev != nullptr && strcmp(dev, getDeviceName()) == 0)
    {
        if (strcmp(name, ScriptsTP.name) == 0)
            {
                ScriptsTP.s = IPS_OK;
                IUUpdateText(&ScriptsTP, texts, names, n);
                // update client display
                IDSetText(&ScriptsTP, nullptr);
                return true;
            }
        if (strcmp(name, RnameTP.name) == 0)
            {
                RnameTP.s = IPS_OK;
                IUUpdateText(&RnameTP, texts, names, n);
                // update client display
                IDSetText(&RnameTP, nullptr);
                return true;
            }

    }
	return INDI::DefaultDevice::ISNewText (dev, name, texts, names, n);
}

/***************************************************************/
bool kobsRelay::ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n)
{
	return INDI::DefaultDevice::ISNewBLOB (dev, name, sizes, blobsizes, blobs, formats, names, n);
}

/***************************************************************/
bool kobsRelay::ISSnoopDevice(XMLEle *root)
{
	return INDI::DefaultDevice::ISSnoopDevice(root);
}

/***************************************************************/
bool kobsRelay::saveConfigItems(FILE *fp)
{
	IUSaveConfigNumber(fp, &BCMpinsNP);
	IUSaveConfigSwitch(fp, &Switch1SP);
	IUSaveConfigSwitch(fp, &Switch2SP);
	IUSaveConfigSwitch(fp, &Switch3SP);
	IUSaveConfigSwitch(fp, &Switch4SP);
    IUSaveConfigText(fp, &ScriptsTP);
    IUSaveConfigText(fp, &RnameTP);

	return true;
}
