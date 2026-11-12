
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/flow-monitor-module.h"

#include <fstream>

using namespace ns3;

std::ofstream cwndFile;

void CwndChange(std::string context,
                uint32_t oldCwnd,
                uint32_t newCwnd)
{
    cwndFile << Simulator::Now().GetSeconds() << ","
             << context << ","
             << newCwnd << "\n";
}

int main(int argc, char *argv[])
{
    uint32_t n = 6;

    CommandLine cmd;
    cmd.AddValue("n", "Number of LAN nodes", n);
    cmd.Parse(argc, argv);

    if (n < 2)
    {
        std::cerr << "At least two nodes are required.\n";
        return 1;
    }

    // Create Ethernet LAN nodes
    NodeContainer nodes;
    nodes.Create(n);

    // Create CSMA LAN
    CsmaHelper csma;
    csma.SetChannelAttribute("DataRate", StringValue("5Mbps"));
    csma.SetChannelAttribute("Delay", TimeValue(NanoSeconds(6560)));

    NetDeviceContainer devices = csma.Install(nodes);

    // Install Internet stack
    InternetStackHelper internet;
    internet.Install(nodes);

    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("192.168.1.0", "255.255.255.0");

    Ipv4InterfaceContainer interfaces = address.Assign(devices);

    uint16_t port = 5000;
    uint32_t pairs = n / 2;

    ApplicationContainer serverApps;
    ApplicationContainer clientApps;

    // Create multiple TCP source/destination pairs
    for (uint32_t i = 0; i < pairs; i++)
    {
        uint32_t sourceNode = i;
        uint32_t destinationNode = n - 1 - i;

        // TCP server
        Address sinkAddress(
            InetSocketAddress(Ipv4Address::GetAny(), port + i));

        PacketSinkHelper sink(
            "ns3::TcpSocketFactory", sinkAddress);

        serverApps.Add(sink.Install(nodes.Get(destinationNode)));

        // TCP client
        OnOffHelper source(
            "ns3::TcpSocketFactory",
            InetSocketAddress(interfaces.GetAddress(destinationNode),
                              port + i));

        source.SetAttribute("DataRate", StringValue("3Mbps"));
        source.SetAttribute("PacketSize", UintegerValue(1024));
        source.SetAttribute("OnTime",
            StringValue("ns3::ConstantRandomVariable[Constant=1]"));
        source.SetAttribute("OffTime",
            StringValue("ns3::ConstantRandomVariable[Constant=0]"));

        clientApps.Add(source.Install(nodes.Get(sourceNode)));
    }

    serverApps.Start(Seconds(0.5));
    serverApps.Stop(Seconds(10.0));

    clientApps.Start(Seconds(1.0));
    clientApps.Stop(Seconds(9.0));

    // Open congestion window CSV file
    cwndFile.open("cwnd.csv");
    cwndFile << "Time,Socket,NewCwnd\n";

    // Connect congestion window trace for TCP sockets
    Simulator::Schedule(Seconds(1.2), []()
    {
        Config::Connect(
            "/NodeList/*/$ns3::TcpL4Protocol/SocketList/*/CongestionWindow",
            MakeCallback(&CwndChange));
    });

    // FlowMonitor
    FlowMonitorHelper flowHelper;
    Ptr<FlowMonitor> monitor = flowHelper.InstallAll();

    Simulator::Stop(Seconds(10.0));
    Simulator::Run();

    monitor->CheckForLostPackets();

    auto stats = monitor->GetFlowStats();

    std::cout << "\n--- EXPERIMENT 12 RESULTS ---\n";
    std::cout << "Number of LAN nodes: " << n << "\n";

    for (auto &flow : stats)
    {
        std::cout << "\nPackets Sent: "
                  << flow.second.txPackets << "\n";

        std::cout << "Packets Received: "
                  << flow.second.rxPackets << "\n";

        std::cout << "Packets Lost: "
                  << flow.second.lostPackets << "\n";

        std::cout << "Throughput: "
                  << (flow.second.rxBytes * 8.0 / 8.0 / 1000000)
                  << " Mbps\n";
    }

    cwndFile.close();

    Simulator::Destroy();

    std::cout << "\nCongestion window data saved to cwnd.csv\n";

    return 0;
}
./ns3 run scratch/exp12
./ns3 run "scratch/exp12 --n=8"

  //Plotter

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
