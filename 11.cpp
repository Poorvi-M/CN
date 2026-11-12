
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
    uint32_t packetCount = 100;

    CommandLine cmd;
    cmd.AddValue("packetCount", "Number of packets", packetCount);
    cmd.Parse(argc, argv);

    // Create six nodes
    NodeContainer nodes;
    nodes.Create(6);

    // Create point-to-point links
    PointToPointHelper p2p;
    p2p.SetDeviceAttribute("DataRate", StringValue("1Mbps"));
    p2p.SetChannelAttribute("Delay", StringValue("10ms"));

    // Make one link a bottleneck
    p2p.SetQueue("ns3::DropTailQueue<Packet>",
                 "MaxSize",
                 QueueSizeValue(QueueSize("5p")));

    NetDeviceContainer devices[5];

    for (int i = 0; i < 5; i++)
    {
        devices[i] = p2p.Install(nodes.Get(i), nodes.Get(i + 1));
    }

    // Internet stack
    InternetStackHelper internet;
    internet.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;

    for (int i = 0; i < 5; i++)
    {
        std::string base = "10.1." + std::to_string(i + 1) + ".0";
        address.SetBase(base.c_str(), "255.255.255.0");
        address.Assign(devices[i]);
    }

    Ipv4GlobalRoutingHelper::PopulateRoutingTables();

    // UDP Echo Server at node 5
    uint16_t port = 9;

    UdpEchoServerHelper server(port);
    ApplicationContainer serverApp = server.Install(nodes.Get(5));

    serverApp.Start(Seconds(0.5));
    serverApp.Stop(Seconds(10.0));

    // UDP Echo Client at node 0
    Ipv4Address destination =
        nodes.Get(5)->GetObject<Ipv4>()->GetAddress(1, 0).GetLocal();

    UdpEchoClientHelper client(destination, port);

    client.SetAttribute("MaxPackets", UintegerValue(packetCount));
    client.SetAttribute("Interval", TimeValue(Seconds(0.001)));
    client.SetAttribute("PacketSize", UintegerValue(1024));

    ApplicationContainer clientApp = client.Install(nodes.Get(0));

    clientApp.Start(Seconds(1.0));
    clientApp.Stop(Seconds(8.0));

    // FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    monitor->CheckForLostPackets();

    Ptr<Ipv4FlowClassifier> classifier =
        DynamicCast<Ipv4FlowClassifier>(flowHelper.GetClassifier());

    auto stats = monitor->GetFlowStats();

    std::cout << "\n--- EXPERIMENT 11 RESULTS ---\n";
    std::cout << "Six-node network\n";

    for (auto &flow : stats)
    {
        auto tuple = classifier->FindFlow(flow.first);

        std::cout << "\nSource: " << tuple.sourceAddress << "\n";
        std::cout << "Destination: " << tuple.destinationAddress << "\n";
        std::cout << "Packets Sent: " << flow.second.txPackets << "\n";
        std::cout << "Packets Received: " << flow.second.rxPackets << "\n";
        std::cout << "Packets Dropped: "
                  << flow.second.lostPackets << "\n";
    }

    Simulator::Destroy();

    return 0;
}

./ns3 run scratch/exp12
./ns3 run "scratch/exp12 --n=8"

//Plot using Python
  import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("cwnd.csv")

for socket in data["Socket"].unique():
    subset = data[data["Socket"] == socket]
    plt.plot(subset["Time"], subset["NewCwnd"],
             label=socket)

plt.xlabel("Time (seconds)")
plt.ylabel("Congestion Window (bytes)")
plt.title("TCP Congestion Window")
plt.legend()
plt.grid()
plt.show()
