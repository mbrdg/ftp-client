# RC - Network Configuration

# Experience 1

In both computers we run the following command: `ifconfig eth0 down`.

After that we run `ifconfig eth0 up 172.16.40.1/24` on *tux3* and `ifconfig
eth0 up 172.16.40.254/24` on *tux4*. Now we ensure that both computers are
connected with a `ping` command. The command used in *tux3* is the following:
`ping 172.16.40.254`. The following outputmeans that we successfully connected
both computers:

```sh
# ping 172.16.40.254
PING 172.16.40.254 (172.16.40.254) 56(84) bytes of data
64 bytes from 172.16.40.254: icmp_seq=1 ttl=64 time=0.315 ms
64 bytes from 172.16.40.254: icmp_seq=2 ttl=64 time=0.156 ms
64 bytes from 172.16.40.254: icmp_seq=3 ttl=64 time=0.143 ms
```

Now we check the forwarding and ARP tables with the commands provided. After a
brief inspection we can say that the only route that exists on *tux3* is the
one refering to the network configured above.
That also means that the ARP can only resolve the *tux4* ip, since it is the
only one in that same network.

```sh
# route -n
Kernel IP routing table
Destination     Gateway     Genmask         Flags Metric Ref    Use Iface
172.16.40.0     0.0.0.0     255.255.255.0   U     0      0        0 eth0
#arp -a
? (172.16.40.254) at 00:21:5a:c3:78:76 [ether] on eth0
```

TODO:
analyse and answer the questions @ home

# Experience 2

Again we setup a network but this time is for *tux2* with the the same
commands as the ones stated above - `ifconfig eth0 down` and `ifconfig eth0 up
172.16.42.1/24`.

After that we'll start configuring the VLANs (Virtual Local Area Network).
First we create a `VLAN 40` with the commands provided and add the ports
accordingly, this means that we should run the commands available in the guide.

## VLAN configuration

The configuration of a VLAN is pretty staright forward once we understood its
concept. Basically, it allows us to group machine logically in a restrict
broadcast domain. This logical division can be visualized in a very simpe way.
Imagine an organization with a lot of departments. Logically the infrastructure
is the same for all the departments, however VLANs allow to have multiple
networks with that same infrastucture. So here is how to configure a VLAN with
the CISCO Catalyst 2960 - assuming a that the switch is configuration is the
default:

```
sw> enable
sw# configure terminal
sw# vlan 40
sw(config)# end
sw# configure terminal
sw# vlan 41
sw(config)# end
```

This code block shows how to create the VLANs. Then we need to assign the
correponding ports of each of the *tux* computers.

```
sw# configure terminal
sw(config)# interface fastethernet 0/<port for tux3>
sw(config-if)# switchport mode access
sw(config-if)# switchport access vlan 40
sw(config-if)# end
sw(config)# interface fastethernet 0/<port for tux4>
sw(config-if)# switchport mode access
sw(config-if)# switchport access vlan 40
sw(config-if)# end
sw(config)# interface fastethernet 0/<port for tux2>
sw(config-if)# switchport mode access
sw(config-if)# switchport access vlan 41
sw(config-if)# end
```

After that the configuration can be checked with the following command:
`sw# show vlan brief`, where it it possible to see which VLANs are currently
configured and the respective ports.

## Pings

Now it is time to test the VLANs with pings. As expected pinging *tux4* from
*tux3* is possible, however when we ping *tux2* from *tux3* we get the message
that netwok is unreachable.

So as one last test we pinged from both *tux3* and *tux2* using the broadcast
flag `-b` and captured the packets received. After a brief analysis of the
packet's logs we can conclude that to each VLAN corresponds a broadcast domain.
That means that when we ping the broadcast adress inside a VLAN only the
devices inside that VLAN will see that ICMP request packet and proceed to reply
if the machine configuration allows it[^1].

[^1]: With the command `#echo 0 > /proc/sys/ipv4/icmp_echo_ignore_broadcasts`.


# Experience 3

After reading the configuration for the CISCO router the answer to the proposed
questions:

## Analysing the configuration

1. Router name is `gnu-rtr1` that can be seen in the line `hostname gnu-rtr1`;
2. There 2 FastEthernet ports available with the numbers 0 and 1. Here is the
code block that tells us that:

    2.1 
    ```
    interface FastEthernet0/0
     ...
    interface FastEthernet0/1
     ...
    ```
3. The configured IP adresses are also available in the same section of the 
router's configuration. We can conclude that the mask of each IP adress is 24
bits.
    
    3.1
    ```
     ip address 172.16.30.1 255.255.255.0
     ...
     ip address 172.16.254.45 255.255.255.0
     ...
    ```

4. The configured routes are also available in the lines below. These are
static routes. In the example below, all the packets with the destination
of 172.16.40.0/24 go to 172.16.30.2 (outside), otherwise they go to 
172.16.254.1 (inside).
    
    4.1
    ```
    ip route 0.0.0.0 0.0.0.0 172.16.254.1
    ip route 172.16.40.0 255.255.255.0 172.16.30.2
    ```

## NAT Configuration

1. The connected interface to the internet is the one in which NAT is set to
outside meaning that in the case of the provided configuration it is visible
with the code block below.

    1.1
    ```
    interface  FastEthernet0/1
    ...
    nat outside
    ...
    ```

2. The available IP adresses for NATing are given by the command `ip nat pool`.
In the given file configuration we can see that the pool ranges from 
`172.16.254.45/24` to `172.16.254.45/24` meaning that there is only 1 adress
available.

3. The router is using overloading that can be checked in the following line:
`ip nat inside source list 1 pool ovrld overload`.

## DNS setup

DNS stands for Domain Name Resolution. Is is a mechanism that transform human
readable adresses in numeric adresses. On the first test (`ping youtubas`)
there were no DNS packets since we inserted the line `142.250.200.142 youtubas`
in the `/etc/hosts` file, which is a static DNS resolver in ever host. Then,
on the second test we could intercept DNS query packets to the default DNS
server, usually provided by the host internet service provider. Finally, in the
third test we switched the DNS server to 9.9.9.9 (provided by Quad9) and
captured some packets, again we can see DNS query packets with the destination
of the recently configured DNS server (9.9.9.9).

## Linux Routing

To check the current routes in the system we shall use the `route -n` command.
The output of the command is the follwing:

```
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
0.0.0.0         192.168.1.1     0.0.0.0         UG    100    0        0 eno2
192.168.1.0     0.0.0.0         255.255.255.0   U     100    0        0 eno2
```

As it is visible the default gateway here is `192.168.1.1`. Now it is requested
to delete the dafault route which can be performed with `route del default`,
this command requires elevated priveliges. After that the output of `route -n`
is similiar to:

```
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
192.168.1.0     0.0.0.0         255.255.255.0   U     100    0        0 eno2
```

That also means that if we try to check connectivity with `traceroute` we will
confirm that it is not possible to reach any adress whose destination is
`192.168.1.0` since that there is no default route. The output given by
`traceroute` is:

```
traceroute to 104.17.113.188 (104.17.113.188), 30 hops max, 60 byte packets
connect: Network is unreachable
```

Now, let's add a route to `104.17.113.188`. It can be done with:
`route add 104.17.113.188 gw 192.168.1.1 eno2`[^2]. And get the following
output from `route -n`:

```
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
104.17.113.188  192.168.1.1     255.255.255.255 UGH   0      0        0 eno2
192.168.1.0     0.0.0.0         255.255.255.0   U     100    0        0 eno2
```

[^2]: The last argument represents a network interface. It might change from
computer to computer.

After that we can run traceroute and verify that we can indeed connect use the
recently created route:

```
traceroute to 104.17.113.188 (104.17.113.188), 30 hops max, 60 byte packets
 1  192.168.1.1  2.214 ms  3.307 ms  0.588 ms
 2  * * *
 3  * * *
 4  * * *
 5  104.17.113.188  9.292 ms  9.869 ms  10.355 ms
```

Capturing the packets we can verify that exist ICMP packets. Basically, we must
know how `traceroute` works in order to understand such existence.
This packets are the responses sent by the multiple hops on the way due to the
fact that the packet "died" along the way because of TTL (Time To Leave). As a
plus we were able to check that those ICMP packets message are the following:

`Time-to-live exceeded (Time to live exceeded in transit)`

Recal that this `traceroute` is essencially an "hack" of the protocol!

Another important thing about the ICMP packets is that it contains the MAC and
IP adresses from both the source and the target to allow the correct
distribution of the packets inside source's and target's LANs.

At last, the routes in a machine can be visualized with the command `route -n`.
That command provides a table of routes which consist in directing a group of
packets whose destination fall in a given range in the correct gateway. If we
take a look at the following route:

```
Kernel IP routing table
Destination     Gateway         Genmask         Flags Metric Ref    Use Iface
104.17.113.188  192.168.1.1     255.255.255.255 UGH   0      0        0 eno2
```

we are able to determine that the packets directed to `104.17.113.188` should
go via the gateway `192.168.1.1`. The Genmask determines the destination
adresses range, in this case, since it is `255.255.255.255` that means that all
32 bits of the destination must match meaning that `104.17.113.188` is the only
and only one adress which falls in the range.

