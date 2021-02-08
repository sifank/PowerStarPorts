/***********************************************************************
*  Program:      PSports.h
*  Version:      20210122
*  Author:       Sifan S. Kahale
*  Description:  Power^Star PSports.h file

***********************************************************************/
#pragma once

#ifndef kobsRelay_H
#define kobsRelay_H

/* Define Driver version */
#define VERSION_MAJOR 0
#define VERSION_MINOR 1

#include <string.h>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <defaultdevice.h>
#include "PScontrol.h"

using namespace std;

PSCTL psctl;

class kobsRelay : public INDI::DefaultDevice
{
public:
	kobsRelay();
	virtual ~kobsRelay();
	virtual const char *getDefaultName();
    virtual int relayCTL(string &relay, const std::string& action);
    //virtual int relaySTAT(string &relay);
    bool checkFaults();
	virtual bool initProperties();
	virtual bool updateProperties();
	virtual void ISGetProperties(const char *dev);
	virtual bool ISNewNumber (const char *dev, const char *name, double values[], char *names[], int n);
	virtual bool ISNewSwitch (const char *dev, const char *name, ISState *states, char *names[], int n);
	virtual bool ISNewText (const char *dev, const char *name, char *texts[], char *names[], int n);
	virtual bool ISNewBLOB (const char *dev, const char *name, int sizes[], int blobsizes[], char *blobs[], char *formats[], char *names[], int n);
	virtual bool ISSnoopDevice(XMLEle *root);
    string      device = "";
    string      action = "";
    
protected:
	virtual bool saveConfigItems(FILE *fp);
    
private:
	virtual bool Connect();
	virtual bool Disconnect();
    
	INumber BCMpinsN[4];
	INumberVectorProperty BCMpinsNP;
	ISwitch Switch1S[2];
	ISwitchVectorProperty Switch1SP;
	ISwitch Switch2S[2];
	ISwitchVectorProperty Switch2SP;
	ISwitch Switch3S[2];
	ISwitchVectorProperty Switch3SP;
	ISwitch Switch4S[2];
	ISwitchVectorProperty Switch4SP;
    
    IText ScriptsT[1] {};
    ITextVectorProperty ScriptsTP;
    
    IText RnameT[4] {};
    ITextVectorProperty RnameTP;
    
    char *relayLabel;
    int rc;
    char fullCmd[40];
    char whichrelay[30];
	int counter;

    PowerStarProfile curProfile;
};

#endif
