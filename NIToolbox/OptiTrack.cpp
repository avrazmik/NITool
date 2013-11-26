#include "stdafx.h"
#include "OptiTrack.h"

sFrameOfMocapData * OptiTrack::current_frame = NULL;
float OptiTrack::g_unitConversion = 1;

/** 
 * Default constructor
 * @param char[]	Client IP address
 * @param char[]	Server IP address
 */
OptiTrack::OptiTrack(char local_ip[], char server_ip[])
{
	strcpy(localIPAddress, local_ip);
	strcpy(serverIPAddress, server_ip);
	connection_type = ConnectionType_Multicast;	
}

/** 
 * Method id used to set the conection type
 * @param	ConnectionType	Type to set
 * @return void
 */
void OptiTrack::SetConnectionType(ConnectionType type)
{
	connection_type = type;
}

/** 
 * Method id used to establish the connection
 * @return void
 */
void OptiTrack::Connect()
{	
	this->SetVerbosityLevel(Verbosity_Debug);	
	int respCode = this->Initialize(localIPAddress, serverIPAddress);
	if (respCode != ErrorCode_OK)
		throw runtime_error("Unable to connect to server. \n");

	oServerDescription;
	memset(&oServerDescription, 0, sizeof(oServerDescription));
    this->GetServerDescription(&oServerDescription);
	if(!oServerDescription.HostPresent)
    {
		throw runtime_error("Unable to connect to the server. Host not present. Exiting. \n");
    }
}

/** 
 * Method id used to print the server details,
 * should be used after the connection is established
 * @return void
 */
void OptiTrack::PrintServerDetails()
{
	printf("[SampleClient] Server application info:\n");
    printf("Application: %s (ver. %d.%d.%d.%d)\n", oServerDescription.szHostApp, oServerDescription.HostAppVersion[0],
        oServerDescription.HostAppVersion[1],oServerDescription.HostAppVersion[2],oServerDescription.HostAppVersion[3]);
    printf("NatNet Version: %d.%d.%d.%d\n", oServerDescription.NatNetVersion[0], oServerDescription.NatNetVersion[1],
        oServerDescription.NatNetVersion[2], oServerDescription.NatNetVersion[3]);
    printf("Client IP:%s\n", localIPAddress);
    printf("Server IP:%s\n", serverIPAddress);
    printf("Server Name:%s\n\n", oServerDescription.szHostComputerName);
}

/** 
 * Method id used to disconnect from the server
 * should be called at the end of code
 * @return void
 */
void OptiTrack::Disconnect()
{
	this->Uninitialize();
}

/** 
 * Method id used to set the unit of convertion
 * @param float	The convertion unit
 * @return void
 */
void OptiTrack::setUnitConvertion(float unit)
{
	g_unitConversion = unit;
}

/** 
 * Method id used to get the unit of convertion
 * @return float	The convertion unit
 */
float OptiTrack::getUnitConvertion()
{
	return g_unitConversion;
}