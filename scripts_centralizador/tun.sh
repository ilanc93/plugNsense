sudo ip tuntap add name tun0 mode tun
sudo ip addr add fe90::0001/64 dev tun0
sudo ip link set dev tun0 up
sudo sysctl -w net.ipv6.conf.all.forwarding=1
sudo ip -6 route add fe90::/64 dev tun0
