#pragma once
#include <string>
#include "NatNetTypes.h"
#include "NatNetClient.h"

using namespace std;

class OptiTrack : public NatNetClient
{
public:
	OptiTrack(char local_ip[], char server_ip[]);
	void SetConnectionType(ConnectionType);
	void Connect();
	void PrintServerDetails();
	void Disconnect();
	static void setUnitConvertion(float unit);
	static float getUnitConvertion();
	static sFrameOfMocapData* current_frame;
private:
	char localIPAddress[128];
	char serverIPAddress[128];
	ConnectionType connection_type;
	sServerDescription oServerDescription;
	static float g_unitConversion;
};

