
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/wifi-module.h"
#include "ns3/mobility-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    uint32_t n = 5;

    CommandLine cmd;
    cmd.AddValue("n", "Number of wireless stations", n);
    cmd.Parse(argc, argv);

    // Create one AP and multiple stations
    NodeContainer wifiStaNodes;
    wifiStaNodes.Create(n);

    NodeContainer wifiApNode;
    wifiApNode.Create(1);

    // Configure Wi-Fi channel
    YansWifiChannelHelper channel =
        YansWifiChannelHelper::Default();

    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

    // Configure Wi-Fi
    WifiHelper wifi;
    wifi.SetStandard(WIFI_STANDARD_80211g);

    wifi.SetRemoteStationManager(
        "ns3::ConstantRateWifiManager",
        "DataMode", StringValue("ErpOfdmRate6Mbps"),
        "ControlMode", StringValue("ErpOfdmRate6Mbps"));

    // Create ESS SSID
    Ssid ssid = Ssid("MyWirelessNetwork");

    // Configure station MAC
    WifiMacHelper mac;

    mac.SetType("ns3::StaWifiMac",
                "Ssid", SsidValue(ssid),
                "ActiveProbing", BooleanValue(false));

    NetDeviceContainer staDevices =
        wifi.Install(phy, mac, wifiStaNodes);

    // Configure access point MAC
    mac.SetType("ns3::ApWifiMac",
                "Ssid", SsidValue(ssid));

    NetDeviceContainer apDevices =
        wifi.Install(phy, mac, wifiApNode);

    // Configure mobility
    MobilityHelper mobility;

    mobility.SetPositionAllocator(
        "ns3::GridPositionAllocator",
        "MinX", DoubleValue(0.0),
        "MinY", DoubleValue(0.0),
        "DeltaX", DoubleValue(5.0),
        "DeltaY", DoubleValue(5.0),
        "GridWidth", UintegerValue(3),
        "LayoutType", StringValue("RowFirst"));

    mobility.SetMobilityModel(
        "ns3::ConstantPositionMobilityModel");

    mobility.Install(wifiStaNodes);
    mobility.Install(wifiApNode);

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(wifiStaNodes);
    internet.Install(wifiApNode);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");

    Ipv4InterfaceContainer staInterfaces =
        address.Assign(staDevices);

    Ipv4InterfaceContainer apInterfaces =
        address.Assign(apDevices);

    // UDP server at AP
    uint16_t port = 9;

    Address sinkAddress(
        InetSocketAddress(Ipv4Address::GetAny(), port));

    PacketSinkHelper sink(
        "ns3::UdpSocketFactory", sinkAddress);

    ApplicationContainer sinkApp =
        sink.Install(wifiApNode.Get(0));

    sinkApp.Start(Seconds(0.5));
    sinkApp.Stop(Seconds(10.0));

    // UDP traffic from all wireless stations
    ApplicationContainer sourceApps;

    for (uint32_t i = 0; i < n; i++)
    {
        OnOffHelper source(
            "ns3::UdpSocketFactory",
            InetSocketAddress(apInterfaces.GetAddress(0), port));

        source.SetAttribute("DataRate", StringValue("1Mbps"));
        source.SetAttribute("PacketSize", UintegerValue(1024));

        source.SetAttribute("OnTime",
            StringValue("ns3::ConstantRandomVariable[Constant=1]"));

        source.SetAttribute("OffTime",
            StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        sourceApps.Add(source.Install(wifiStaNodes.Get(i)));
    }

    sourceApps.Start(Seconds(1.0));
    sourceApps.Stop(Seconds(9.0));

    // FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    monitor->CheckForLostPackets();

    auto stats = monitor->GetFlowStats();

    uint64_t totalTx = 0;
    uint64_t totalRx = 0;
    uint64_t totalLost = 0;
    uint64_t totalBytes = 0;

    for (auto &flow : stats)
    {
        totalTx += flow.second.txPackets;
        totalRx += flow.second.rxPackets;
        totalLost += flow.second.lostPackets;
        totalBytes += flow.second.rxBytes;
    }

    std::cout << "\n--- EXPERIMENT 13 RESULTS ---\n";

    std::cout << "Number of wireless stations: "
              << n << "\n";

    std::cout << "Packets Transmitted: "
              << totalTx << "\n";

    std::cout << "Packets Received: "
              << totalRx << "\n";

    std::cout << "Packets Lost: "
              << totalLost << "\n";

    double pdr = 0;

    if (totalTx > 0)
        pdr = (double)totalRx / totalTx * 100;

    std::cout << "Packet Delivery Ratio: "
              << pdr << "%\n";

    double throughput =
        (totalBytes * 8.0) / (8.0 * 1000000);

    std::cout << "Throughput: "
              << throughput << " Mbps\n";

    Simulator::Destroy();

    return 0;
}
./ns3 run scratch/exp13
./ns3 run "scratch/exp13 --n=8"
