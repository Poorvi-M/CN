
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    std::string bandwidth = "1Mbps";
    uint32_t queueSize = 10;

    CommandLine cmd;
    cmd.AddValue("bandwidth", "Link bandwidth", bandwidth);
    cmd.AddValue("queueSize", "Queue size in packets", queueSize);
    cmd.Parse(argc, argv);

    // Create three nodes
    NodeContainer nodes;
    nodes.Create(3);

    // Create point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue(bandwidth));
    p2p.SetChannelAttribute("Delay", StringValue("5ms"));
    p2p.SetQueue("ns3::DropTailQueue<Packet>",
                 "MaxSize",
                 QueueSizeValue(QueueSize(std::to_string(queueSize) + "p")));

    // Connect node 0-1 and node 1-2
    NetDeviceContainer d1 = p2p.Install(nodes.Get(0), nodes.Get(1));
    NetDeviceContainer d2 = p2p.Install(nodes.Get(1), nodes.Get(2));

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;

    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer i1 = address.Assign(d1);

    address.SetBase("10.1.2.0", "255.255.255.0");
    Ipv4InterfaceContainer i2 = address.Assign(d2);

    // Enable routing
    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // UDP packet sink at node 2
    uint16_t port = 9;

    Address sinkAddress(
        InetSocketAddress(Ipv4Address::GetAny(), port));

    PacketSinkHelper sink("ns3::UdpSocketFactory", sinkAddress);

    ApplicationContainer sinkApp = sink.Install(nodes.Get(2));
    sinkApp.Start(Seconds(0.0));
    sinkApp.Stop(Seconds(10.0));

    // Generate UDP traffic from node 0
    OnOffHelper source(
        "ns3::UdpSocketFactory",
        InetSocketAddress(i2.GetAddress(1), port));

    source.SetAttribute("DataRate", StringValue("5Mbps"));
    source.SetAttribute("PacketSize", UintegerValue(1024));
    source.SetAttribute("OnTime",
                        StringValue("ns3::ConstantRandomVariable[Constant=1]"));
    source.SetAttribute("OffTime",
                        StringValue("ns3::ConstantRandomVariable[Constant=0]"));

    ApplicationContainer sourceApp = source.Install(nodes.Get(0));
    sourceApp.Start(Seconds(1.0));
    sourceApp.Stop(Seconds(9.0));

    // FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    auto stats = monitor->GetFlowStats();

    std::cout << "\n--- EXPERIMENT 10 RESULTS ---\n";

    for (auto &flow : stats)
    {
        auto tuple = classifier->FindFlow(flow.first);

        std::cout << "Source: " << tuple.sourceAddress << "\n";
        std::cout << "Destination: " << tuple.destinationAddress << "\n";
        std::cout << "Packets Sent: " << flow.second.txPackets << "\n";
        std::cout << "Packets Received: " << flow.second.rxPackets << "\n";
        std::cout << "Packets Dropped: "
                  << flow.second.lostPackets << "\n";
    }

    Simulator::Destroy();

    return 0;
}

./ns3 run "scratch/exp10"
./ns3 run "scratch/exp10 --bandwidth=500Kbps --queueSize=5"
./ns3 run "scratch/exp10 --bandwidth=2Mbps --queueSize=20"
