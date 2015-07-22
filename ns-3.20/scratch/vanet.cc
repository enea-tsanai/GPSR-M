/* 
 * File:   vanet.cc
 * Author: Enea Tsanai
 * e-mail: tsanaienea@gmail.com, tsanai@ceid.upatras.gr
 * Created on January 22, 2015, 5:03 PM
 */

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>      // std::setprecision

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/applications-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/internet-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"
#include "ns3/gpsr-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/energy-module.h"

//#include "src/core/model/object-base.h"
#include "ns3/flow-monitor-module.h"
#include "ns3/building-list.h"
#include "src/winner-models/model/building-map-model.h"
#include "ns3/winner-models-module.h"


#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"


using namespace ns3;
using namespace dsr;

NS_LOG_COMPONENT_DEFINE ("Vanets_and_Apps");


double NodeTotEnergyCons [200];
uint32_t iter = 0, TX_ = 0, RX_ = 0;
uint32_t numNodes_, RP_;

std::string OutDir = "logs/";
//std::string TXFile = "TX";
//std::string RXFile = "RX";
std::string ENFile = "EN";
 
std::string
RPToStr (int RP)
{
   switch (RP)
    {
      case 1:
        return "_rp_1.txt";
        break;
      case 2:
        return "_rp_2.txt";
        break;
      case 3:
        return "_rp_3.txt";
        break;
      case 4:
        return "_rp_4.txt";
        break;
       case 5:
        return "_rp_5.txt";
        break;
      default:
        NS_FATAL_ERROR ("No such protocol:" << RP);
        return "_rp_?.txt";
    }
}

void
TotalEnergy (double oldValue, double totalEnergy)
{
  int index_node = iter%numNodes_;
  NodeTotEnergyCons[index_node] = totalEnergy;
  iter ++;
}

void
ExportEnergyPerNode ()
{  
  std::ofstream logs;    
  logs.open ((OutDir + ENFile + RPToStr(RP_)).c_str(),std::ios::app);
  for (uint32_t i = 0; i < numNodes_; ++i) {
    logs << i << " " << NodeTotEnergyCons[i] << "\n";
  }
  logs.close();
}


int 
main (int argc, char *argv[])
{
  
  uint32_t numNodes; // num of vehicles
  bool buildings;
  std::string traceFile; //mobility file
  std::string buildingsFile; //buildings file
  uint32_t RoadsInX;
  uint32_t RoadsInY;
  double RoadLength;
  uint32_t numPackets; // Number of packets to send
  uint32_t packetSize; // bytes    
  double interval; // seconds
  Time interPacketInterval;
  double duration;    
  uint32_t RP; // 1: OLSR, 2: AODV, 3: DSDV, 4: DSR, 5: GPSR
  bool logging;
  bool tracing;
  bool AnimTest;
  
  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor;

  /*Countainers*/
  NodeContainer Nodes;
  NetDeviceContainer devices;
  /* Energy */
  EnergySourceContainer E_sources;
  DeviceEnergyModelContainer deviceModels;
  /* Ifaces */
  Ipv4InterfaceContainer IFContainer;
  /* Apps */
  ApplicationContainer serverApps; 
  ApplicationContainer clientApps;

  /*Helpers*/
  YansWifiPhyHelper wifiPhy;
  //    Ns2MobilityHelper ns2;
  YansWifiChannelHelper wifiChannel;

  /* Senders-Receivers */
//  Ptr<Socket> receivers[10];
//  Ptr<Socket> sources[10];
  
  /* Init default vars*/
  numNodes=2;
  buildings=true;
  traceFile="./VanetsJournal/BonMotion/scenario.ns_movements";
  buildingsFile="./scratch/buildings.txt";
  RoadsInX=6;
  RoadsInY=6;
  RoadLength=100;
  numPackets=400;
  packetSize=500;
  interval=0.2;
  duration=20;
  RP=5;
  logging=false; 
  tracing=false;
  AnimTest=false;
  
  /* Connections */
  const int conns = 1;  
  uint32_t srcs[conns];
  uint32_t rcvs[conns];
    
  
  /* Configure */
  SeedManager::SetSeed(12345);
  CommandLine cmd;
  cmd.AddValue ("numNodes", "Num of nodes ", numNodes); 
  cmd.AddValue ("buildings", "Bool buildings in scenario ", buildings);
  cmd.AddValue ("traceFile", "Mobility tracefile", traceFile);
  cmd.AddValue ("buildingsFile", "Buildings map", buildingsFile);
  cmd.AddValue ("RoadsInX", "Num of roads in X", RoadsInX);
  cmd.AddValue ("RoadsInY", "Num of roads in Y", RoadsInY);
  cmd.AddValue ("RoadLength", "Length of road", RoadLength);
  cmd.AddValue ("numPackets", "numPackets", numPackets);
  cmd.AddValue ("packetSize", "packetSize", packetSize); 
  cmd.AddValue ("interval", "interval", interval); 
  cmd.AddValue ("duration", "duration", duration); 
  cmd.AddValue ("RP", "Routing Protocol", RP); 
  cmd.AddValue ("logging", "Print logs on screen", logging);
  cmd.AddValue ("tracing", "Write logs to files", tracing);
  cmd.AddValue ("AnimTest", "Animation Test", AnimTest);
    
  cmd.Parse (argc, argv);
  
  numNodes_ = numNodes;
  RP_ = RP;
  
  /*CLear log files*/
  std::ofstream logf1;
  logf1.open ((OutDir + ENFile + RPToStr(RP)).c_str(),std::ios::trunc);
  logf1.close();
  
  
  
//  Config::Set("ns3::gpsr::RoutingProtocol::HelloInterval", DoubleValue (0.5));
  /* Logs */
//  LogComponentEnable("GpsrRoutingProtocol", LOG_LEVEL_LOGIC);
//  LogComponentEnable("GpsrRoutingProtocol", LOG_LEVEL_LOGIC);
//  LogComponentEnable("GpsrTable", LOG_LEVEL_LOGIC);
  LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
  LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);

  
  /* Nodes */  
  Nodes.Create (numNodes); 
  std::cout << "Created " << (unsigned)numNodes << " Nodes\n";


  /* Mobility */
  // Load external mobility file, e.g. from SUMO and configure the VANET mobility
  std::cout << "traceFile: " << traceFile << "\n";
  Ns2MobilityHelper ns2 = Ns2MobilityHelper::Default (traceFile); 
  ns2.Install();
  
  
  /* Set Map */
  GridMap gridRoadMap = GridMap(RoadsInX, RoadsInY, RoadLength);
  BuildingMapModel Map = BuildingMapModel(buildingsFile, &gridRoadMap);    
  ShadowingModel shadow = ShadowingModel();
  
  
  /* Create 802.11p Devices*/
  wifiPhy =  YansWifiPhyHelper::Default ();
  
  // std::string phyMode ("OfdmRate6MbpsBW10MHz");
  std::string phyMode ("OfdmRate3MbpsBW10MHz");
    
////   disable fragmentation for frames below 2200 bytes
//    Config::SetDefault ("ns3::WifiRemoteStationManager::FragmentationThreshold", StringValue ("2200"));
//    // turn off RTS/CTS for frames below 2200 bytes
//    Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", StringValue ("2200"));
//    // Fix non-unicast data rate to be the same as that of unicast
//    Config::SetDefault ("ns3::WifiRemoteStationManager::NonUnicastMode", StringValue (phyMode));
    
  // Channel - Physical Layer
    
  wifiChannel.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");    
    
  if (buildings)
  {
    std::cout << "Simulation with Buildings\n" ;
    wifiChannel.AddPropagationLoss("ns3::WinnerB1LossModel",
                                  "Frequency", DoubleValue(5.9e9),
                                  "EffEnvironmentHeight", DoubleValue(1),
                                  "VisibilityModel", PointerValue(&Map),
                                  "ShadowingModel", PointerValue(&shadow));
  }
  else
  {
    wifiChannel.AddPropagationLoss ("ns3::FriisPropagationLossModel");
  }

  // Wifi - Physical
  wifiPhy.SetChannel (wifiChannel.Create ());

  /* 300m */
  // wifiPhy.Set ("RxGain", DoubleValue (-10) ); // set it to zero; otherwise, gain will be added  
  // wifiPhy.Set ("TxPowerStart", DoubleValue(15));
  // wifiPhy.Set ("TxPowerEnd", DoubleValue(15));
  
  /* 250m */
  wifiPhy.Set ("EnergyDetectionThreshold", DoubleValue(-83));
  wifiPhy.Set ("CcaMode1Threshold", DoubleValue(-86));

  // Values for typical VANET scenarios according to 802.11p
  wifiPhy.Set ("TxPowerStart", DoubleValue(33));
  wifiPhy.Set ("TxPowerEnd", DoubleValue(33));
  wifiPhy.Set ("TxPowerLevels", UintegerValue(1));
  wifiPhy.Set ("TxGain", DoubleValue(0));
  wifiPhy.Set ("RxGain", DoubleValue(0));
              
  /*WIFI_80211p ADHOC*/
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  wifiMac.SetType ("ns3::AdhocWifiMac");

  WifiHelper wifi = WifiHelper::Default ();
  wifi.SetStandard (WIFI_PHY_STANDARD_80211_10MHZ);
  wifi.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                               "DataMode", StringValue (phyMode),
                               "ControlMode",StringValue (phyMode));
  /* Install WIFI */
  devices = wifi.Install (wifiPhy, wifiMac, Nodes);    
  if (logging) wifi.EnableLogComponents (); //Turn on Wifi 802.11p logging    

  if (tracing)
  {
    /** Energy Model **/
    /* energy source */
    BasicEnergySourceHelper basicSourceHelper;
    // configure energy source
    basicSourceHelper.Set ("BasicEnergySourceInitialEnergyJ", DoubleValue (1000000000000));
    // install source
    E_sources = basicSourceHelper.InstallAll();
    /* device energy model */
    WifiRadioEnergyModelHelper radioEnergyHelper;
    // configure radio energy model
    radioEnergyHelper.Set ("TxCurrentA", DoubleValue (0.0174));
    // install device model
    deviceModels = radioEnergyHelper.Install (devices, E_sources);
    std::cout << "Energy: " << deviceModels.Get(0)->GetCurrentA() << "\n";
  }
    
  if (!AnimTest)
  {
    /* SetRoutingAndInternet */
    AodvHelper aodv;
    OlsrHelper olsr;
    DsdvHelper dsdv;
    DsrHelper dsr;
    DsrMainHelper dsrMain;
    GpsrHelper gpsr;     

    //    Ipv4StaticRoutingHelper staticRouting;
    Ipv4ListRoutingHelper list;
    // list.Add (staticRouting, 0);
    std::string RoutingName;

    switch (RP)
    {
      case 1:
        list.Add (olsr, 1);
        RoutingName = "OLSR";
        break;
      case 2:
        list.Add (aodv, 1);
        RoutingName = "AODV";
        break;
      case 3:
        list.Add (dsdv, 1);
        RoutingName = "DSDV";
        break;
      case 4:
        RoutingName = "DSR";
        break;
       case 5:
        list.Add (gpsr, 1);
        RoutingName = "GPSR";
        break;
      default:
        NS_FATAL_ERROR ("No such protocol:" << RP);
    }

    InternetStackHelper internet;

    if (RP < 4)
    {
        internet.SetRoutingHelper (list);
        internet.Install (Nodes);        
    }
    else if (RP == 4)
    {
        internet.Install (Nodes);
        dsrMain.Install (dsr, Nodes);
    }
    else if (RP == 5)
    {
      internet.SetRoutingHelper (gpsr);
      internet.Install (Nodes);
    }

    Ipv4AddressHelper ipv4;
    NS_LOG_INFO ("Assign IP Addresses...");
    ipv4.SetBase ("10.1.1.0", "255.255.255.0");
    IFContainer = ipv4.Assign (devices);
    
    
    /* Install Apps */
    
    /* UDP Server Application */
    interPacketInterval = Seconds (interval);
    srcs[0] = 0;
    rcvs[0] = 1;
    
//    srcs[1] = 2;
//    rcvs[1] = 3;
//    
//    srcs[2] = 4;
//    rcvs[2] = 5;
       
    
    /* Servers */
    for (int i=0; i < conns; i++)
    {
      UdpServerHelper Server (9);
      serverApps.Add (Server.Install (Nodes.Get (rcvs[i])));      
    }          
        
    /* Clients */
    for (int i=0; i < conns; i++)
    {
      UdpClientHelper Client (IFContainer.GetAddress (rcvs[i]), 9);
      Client.SetAttribute ("MaxPackets", UintegerValue (numPackets));
      Client.SetAttribute ("Interval", TimeValue (interPacketInterval));
      Client.SetAttribute ("PacketSize", UintegerValue (packetSize));
      
      clientApps.Add(Client.Install (Nodes.Get (srcs[i])));
    }
    
    /* Start Servers */
    serverApps.Start (Seconds (1));
    serverApps.Stop (Seconds (duration));        
    
    /*Start Clients */
    clientApps.Start (Seconds (1+1.0));
    clientApps.Stop (Seconds (duration));
    
    /* Server - Client set Time*/
    serverApps.Get(0)->SetStartTime(Seconds(2));
    clientApps.Get(0)->SetStartTime(Seconds(3));
    
            
    if (RP == 5)
    {
      GpsrHelper gpsr;
      gpsr.Install ();
    }
    
    
//    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("RTable.txt", std::ios::out);
//    double s = 1;
//    switch (RP)
//    {
//      case 1:
////        olsr.PrintRoutingTableEvery(Seconds(s), Nodes.Get(0), routingStream);
//        break;
//      case 2:
////        aodv.PrintRoutingTableEvery(Seconds(s), Nodes.Get(0), routingStream);
//        break;
//      case 3:
////        dsdv.PrintRoutingTableEvery(Seconds(s), Nodes.Get(0), routingStream);
//        break;
//      case 4:
////        dsrMain.PrintRoutingTableEvery(Seconds(s), Nodes.Get(2), routingStream);
//        break;
//       case 5:
////        gpsr.PrintRoutingTableEvery(Seconds(s), Nodes.Get(2), routingStream);
////        gpsr.PrintRoutingTableAllEvery(Seconds(s), routingStream);
//        break;
//      default:
//        NS_FATAL_ERROR ("No such protocol:" << RP);
//    }    
  }
  
  if (tracing)
  {
    // Energy tracing
    for (uint32_t i = 0; i < numNodes; i++)
    {
      // Energy source
      Ptr<BasicEnergySource> basicSourcePtr = DynamicCast<BasicEnergySource> (E_sources.Get (i));
      // device energy model
      Ptr<DeviceEnergyModel> basicRadioModelPtr = basicSourcePtr->FindDeviceEnergyModels ("ns3::WifiRadioEnergyModel").Get (0);
      NS_ASSERT (basicRadioModelPtr != NULL);
      basicRadioModelPtr->TraceConnectWithoutContext ("TotalEnergyConsumption", MakeCallback (&TotalEnergy));
    }  
    Simulator::Schedule (Seconds (duration-2), &ExportEnergyPerNode);
  }    
  
  
  Simulator::Stop (Seconds (duration));
  Simulator::Run ();        
  Simulator::Destroy ();
  return 0; 
}
